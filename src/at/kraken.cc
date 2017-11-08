/* Copyright 2017 Paolo Galeone <nessuno@nerdz.eu>. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.*/

#include <at/kraken.hpp>

namespace at {

// private methods

std::string Kraken::_nonce() const
{
    std::ostringstream oss;

    timespec tp;
    if (clock_gettime(CLOCK_REALTIME, &tp) != 0) {
        oss << "clock_gettime() failed: " << strerror(errno);
        throw std::runtime_error(oss.str());
    }
    else {
        // format output string
        oss << std::setfill('0') << std::setw(10) << tp.tv_sec << std::setw(9)
            << tp.tv_nsec;
    }
    return oss.str();
}

std::string Kraken::_sign(const std::string& path, const std::string& nonce,
                          const std::string& postdata) const
{
    std::vector<unsigned char> data(path.begin(), path.end());
    std::vector<unsigned char> nonce_postdata =
        at::crypt::sha256(nonce + postdata);
    data.insert(data.end(), nonce_postdata.begin(), nonce_postdata.end());
    return at::crypt::base64_encode(
        at::crypt::hmac_sha512(data, at::crypt::base64_decode(_api_secret)));
}

std::vector<std::string> Kraken::_symbols()
{
    if (_available_symbols.size() == 0) {
        for (const auto& pair : _minimumLimits) {
            _available_symbols.push_back(pair.first);
        }
        return _available_symbols;
    }
    return _available_symbols;
}

// Kraken uses XBT while other uses BTC.
// Replace inputs symbol BTC with XBT
void Kraken::_sanitize_pair(currency_pair_t& pair)
{
    if (pair.first == "BTC") {
        pair.first = "XBT";
    }
    if (pair.second == "BTC") {
        pair.second = "XBT";
    }
}

double Kraken::_minTradable(const std::string& symbol)
{
    auto it = _minimumLimits.find(symbol);
    if (it != _minimumLimits.end()) {
        return it->second;
    }
    if (symbol[0] == 'X' && symbol.length() == 4) {  // XXBTC
        it = _minimumLimits.find(symbol.substr(1, 3));
        if (it != _minimumLimits.end()) {
            return it->second;
        }
    }
    return 0;
}

currency_pair_t Kraken::_str2pair(std::string str)
{
    std::string first = str.substr(0, 3);
    auto symbols = _symbols();
    if (std::find(symbols.begin(), symbols.end(), first) != symbols.end()) {
        return currency_pair_t(first, str.substr(3, str.size()));
    }
    first = str.substr(0, 4);
    if (std::find(symbols.begin(), symbols.end(), first) != symbols.end()) {
        return currency_pair_t(first, str.substr(4, str.size()));
    }
    throw std::runtime_error("Unable to extract pair from str" + str);
}

json Kraken::_request(std::string method,
                      std::vector<std::pair<std::string, std::string>> params)
{
    if (_api_key.empty() || _api_secret.empty()) {
        throw std::runtime_error("API KEY/SECRET required for private methods");
    }
    auto private_method = "private/" + method;
    auto path = "/" + _version + "/" + private_method;
    auto nonce = _nonce();
    params.push_back({"nonce", nonce});
    if (_otp != "") {
        params.push_back({"otp", _otp});
    }

    std::list<std::string> headers;
    headers.push_back("API-Key: " + _api_key);
    headers.push_back("API-Sign: " + _sign(path, nonce, [&]() {
                          std::string ret;
                          for (const auto& key_value : params) {
                              ret.append(key_value.first + "=" +
                                         key_value.second + "&");
                          }
                          ret.pop_back();
                          return ret;
                      }()));
    Request req(headers);
    return req.post(_host + private_method, params);
}

// end private methods

std::time_t Kraken::time() const
{
    Request req;
    json res = req.get(_host + "public/Time");
    _throw_error_if_any(res);
    res = res["result"];
    std::time_t timestamp = res.at("unixtime").get<uint32_t>();
    return timestamp;
}

std::map<std::string, coin_t> Kraken::coins()
{
    // "BCH":{"aclass":"currency","altname":"BCH","decimals":10,"display_decimals":5}
    Request req;
    json res = req.get(_host + "public/Assets?aclass=currency");
    _throw_error_if_any(res);
    res = res["result"];
    std::map<std::string, coin_t> ret;
    for (auto it = res.begin(); it != res.end(); ++it) {
        auto value = *it;
        auto coin = coin_t{.name = value["altname"],
                           .symbol = it.key(),
                           .status = "available"};
        ret[value["altname"]] = coin;
    }
    return ret;
}

std::vector<market_info_t> Kraken::info()
{
    Request req;
    json res = req.get(_host + "public/AssetPairs");
    _throw_error_if_any(res);

    res = res["result"];
    std::vector<market_info_t> markets;
    for (auto it = res.begin(); it != res.end(); ++it) {
        auto market = *it;
        // Skip darkpool markets
        if (market.at("altname").get<std::string>().rfind(".d") !=
            std::string::npos) {
            continue;
        }

        markets.push_back(market_info_t{
            .pair = currency_pair_t(market["base"], market["quote"]),
            .limit =
                deposit_limit_t{.min = _minTradable(market["base"]),
                                .max = std::numeric_limits<double>::infinity()},
            .maker_fee = market["fees_maker"][0][1].get<double>(),  // low
            .taker_fee = market["fees"][0][1].get<double>()});      // high
    }
    return markets;
}

market_info_t Kraken::info(currency_pair_t pair)
{
    _sanitize_pair(pair);
    Request req;
    std::ostringstream url;
    url << _host;
    url << "public/AssetPairs?pair=";
    // << pair == c1_c2. Kraken needs c1c2, thus
    url << pair.first;
    url << pair.second;
    json res = req.get(url.str());
    _throw_error_if_any(res);
    json market = res["result"].begin().value();

    return market_info_t{
        .pair = pair,
        .limit =
            deposit_limit_t{.min = _minTradable(market["base"]),
                            .max = std::numeric_limits<double>::infinity()},
        .maker_fee = market["fees_maker"][0][1].get<double>(),
        .taker_fee = market["fees"][0][1].get<double>()};
}

deposit_info_t Kraken::depositInfo(std::string currency)
{
    toupper(currency);
    json res = _request("DepositMethods", {{"asset", currency}});
    _throw_error_if_any(res);
    res = res["result"][0];
    // [{"fee":"0.0000000000","gen-address":true,"limit":false,"method":"Zcash
    // (Transparent)"}]
    double limit = std::numeric_limits<double>::infinity();
    try {
        limit = std::stod(res.at("limit").get<std::string>());
    }
    catch (const std::domain_error&) {
        // if here, limit = false = no limits
    }
    return deposit_info_t{
        .limit = deposit_limit_t{.min = _minTradable(currency), .max = limit},
        .fee = std::stod(res.at("fee").get<std::string>()),
        .currency = currency,
        .method = res.at("method").get<std::string>(),
    };
}

std::map<std::string, double> Kraken::balance()
{
    json res = _request("Balance", {});
    _throw_error_if_any(res);
    res = res["result"];
    std::map<std::string, double> ret;

    for (auto it = res.begin(); it != res.end(); ++it) {
        ret[it.key()] = std::stod((*it).get<std::string>());
    }
    return ret;
}

double Kraken::balance(std::string currency)
{
    toupper(currency);
    auto balances = balance();
    if (balances.find(currency) != balances.end()) {
        return balances[currency];
    }
    if (currency.size() < 4) {
        auto xcurrency = "X" + currency;
        if (balances.find(xcurrency) != balances.end()) {
            return balances[xcurrency];
        }
        auto zcurrency = "Z" + currency;
        if (balances.find(zcurrency) != balances.end()) {
            return balances[zcurrency];
        }
    }
    return 0;
}

ticker_t Kraken::ticker(currency_pair_t pair)
{
    _sanitize_pair(pair);

    Request req;
    std::ostringstream url;
    url << _host;
    url << "public/Ticker?pair=";
    // << pair == c1_c2. Kraken needs c1c2, thus
    url << pair.first;
    url << pair.second;
    json res = req.get(url.str());
    _throw_error_if_any(res);
    res = res["result"].begin().value();
    auto now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto ask = quotation_t{
        .price = std::stod(res["a"][0].get<std::string>()),
        .amount = std::stod(res["a"][2].get<std::string>()),
        .time = now,
    };
    auto bid = quotation_t{
        .price = std::stod(res["b"][0].get<std::string>()),
        .amount = std::stod(res["b"][2].get<std::string>()),
        .time = now,
    };

    ticker_t ret;
    ret.ask = ask;
    ret.bid = bid;
    return ret;
}

std::vector<ticker_t> Kraken::orderBook(currency_pair_t pair)
{
    _sanitize_pair(pair);
    Request req;
    std::ostringstream url;
    url << _host;
    url << "public/Depth?pair=";
    // << pair == c1_c2. Kraken needs c1c2, thus
    url << pair.first;
    url << pair.second;
    json res = req.get(url.str());
    _throw_error_if_any(res);
    res = res["result"].begin().value();

    std::vector<ticker_t> ret;
    size_t idx = 0;
    for (auto it = res["asks"].begin(); it != res["asks"].end(); ++idx, ++it) {
        auto ask = quotation_t{
            .price = std::stod(res["asks"][idx][0].get<std::string>()),
            .amount = std::stod(res["asks"][idx][1].get<std::string>()),
            .time = res["asks"][idx][2].get<uint32_t>(),
        };
        auto bid = quotation_t{
            .price = std::stod(res["bids"][idx][0].get<std::string>()),
            .amount = std::stod(res["bids"][idx][1].get<std::string>()),
            .time = res["bids"][idx][2].get<uint32_t>(),
        };

        ticker_t ticker;
        ticker.ask = ask;
        ticker.bid = bid;
        ret.push_back(ticker);
    }
    return ret;
}

std::vector<order_t> Kraken::closedOrders()
{
    json res = _request("ClosedOrders", {});
    _throw_error_if_any(res);
    res = res["result"]["closed"];
    std::vector<order_t> ret;

    for (auto it = res.begin(); it != res.end(); it++) {
        auto row = it.value();
        order_t order;
        order.txid = it.key();
        order.status = row.at("status").get<tx_status_t>();
        order.open = row.at("opentm").get<double>();
        order.close = row.at("closetm").get<double>();
        order.pair = _str2pair(row["descr"]["pair"].get<std::string>());
        order.type = row["descr"]["type"].get<std::string>();
        order.ordertype = row["descr"]["ordertype"].get<std::string>();
        order.volume = std::stod(row.at("vol_exec").get<std::string>());
        order.cost = std::stod(row.at("cost").get<std::string>());
        order.fee = std::stod(row.at("fee").get<std::string>());
        order.price = std::stod(row["descr"]["price"].get<std::string>());
        ret.push_back(order);
    }
    return ret;
}

std::vector<order_t> Kraken::openOrders()
{
    json res = _request("OpenOrders", {});
    _throw_error_if_any(res);
    res = res["result"]["open"];
    std::vector<order_t> ret;

    for (auto it = res.begin(); it != res.end(); it++) {
        auto row = it.value();
        order_t order;
        order.txid = it.key();
        order.status = row.at("status").get<tx_status_t>();
        order.open = row.at("opentm").get<double>();
        order.close = 0;
        order.pair = _str2pair(row["descr"]["pair"].get<std::string>());
        order.type = row["descr"]["type"].get<std::string>();
        order.ordertype = row["descr"]["ordertype"].get<std::string>();
        order.volume = std::stod(row.at("vol").get<std::string>());
        order.cost = std::stod(row.at("cost").get<std::string>());
        order.fee = std::stod(row.at("fee").get<std::string>());
        order.price = std::stod(row["descr"]["price"].get<std::string>());
        ret.push_back(order);
    }
    return ret;
}

void Kraken::place(order_t& order)
{
    _sanitize_pair(order.pair);
    if (order.type.empty()) {
        throw std::runtime_error("order.type can't be empty: BUY/SELL");
    }
    if (order.ordertype.empty()) {
        throw std::runtime_error(
            "order.ordertype can't be empty: limit/market");
    }
    std::vector<std::pair<std::string, std::string>> params;
    params.push_back({"pair", order.pair.first + order.pair.second});
    params.push_back({"type", order.type});
    params.push_back({"ordertype", order.ordertype});
    params.push_back({"volume", std::to_string(order.volume)});

    auto ordertype = order.ordertype;
    toupper(ordertype);
    if (ordertype == "MARKET") {
        if (order.volume <= 0) {
            throw std::runtime_error("order.volume can't be <= 0");
        }
    }
    else {
        if (order.volume * order.price <= 0) {
            throw std::runtime_error(
                "order.volume * order.price can't be <= 0");
        }
        params.push_back({"price", std::to_string(order.price)});
    }

    json res = _request("AddOrder", params);
    _throw_error_if_any(res);
    res = res["result"];
    order.txid = res["txid"][0].get<std::string>();
}

void Kraken::cancel(order_t& order)
{
    json res = _request("CancelOrder", {{"txid", order.txid}});
    _throw_error_if_any(res);
    order = {};
}

}  // namespace at
