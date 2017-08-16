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

#include <at/shapeshift.hpp>

namespace at {

double Shapeshift::rate(currency_pair_t pair)
{
    Request req;
    std::ostringstream url;
    url << _host;
    url << "rate/";
    url << pair;
    json res = req.get(url.str());
    _throw_error_if_any(res);
    return std::stod(res["rate"].get<std::string>());
}

deposit_limit_t Shapeshift::depositLimit(currency_pair_t pair)
{
    Request req;
    std::ostringstream url;
    url << _host;
    url << "limit/";
    url << pair;
    json res = req.get(url.str());
    _throw_error_if_any(res);
    // {"limit":"1.81514557","min":"0.000821","pair":"btc_eth"}
    return deposit_limit_t{.min = std::stod(res["min"].get<std::string>()),
                           .max = std::stod(res["limit"].get<std::string>())};
}

std::vector<exchange_info_t> Shapeshift::info()
{
    Request req;
    json res = req.get(_host + "marketinfo/");
    _throw_error_if_any(res);
    // ,{"limit":0.43007489,"maxLimit":0.43007489,"min":0.01802469,"minerFee":0.01,"pair":"NMC_PPC","rate":"1.06196283"}
    std::vector<exchange_info_t> markets;
    for (const auto& market : res) {
        auto pair_str = market["pair"].get<std::string>();
        size_t delimiter_it = pair_str.find_last_of("_");
        auto pair = currency_pair_t(pair_str.substr(0, delimiter_it),
                                    pair_str.substr(delimiter_it + 1));

        markets.push_back(exchange_info_t{
            .pair = pair,
            .limit = deposit_limit_t{.min = market["min"].get<double>(),
                                     .max = market["limit"].get<double>()},
            .rate = std::stod(
                market["rate"].get<std::string>()),  // rate is a string
            .miner_fee = market["minerFee"].get<double>()});
    }
    return markets;
}

exchange_info_t Shapeshift::info(currency_pair_t pair)
{
    Request req;
    std::ostringstream url;
    url << _host;
    url << "marketinfo/";
    url << pair;
    json market = req.get(url.str());
    _throw_error_if_any(market);

    // {"limit":0.43558867,"maxLimit":0.43558867,"minerFee":0.01,"minimum":0.01753086,"pair":"nmc_ppc","rate":1.04852027}
    // shapeshift API is inconsistent as hell
    return exchange_info_t{
        .pair = pair,
        .limit =
            deposit_limit_t{
                .min =
                    market["minimum"].get<double>(),  // minumum instead of min
                .max = market["limit"].get<double>()},
        .rate = market["rate"].get<double>(),  // rate is no more a string
        .miner_fee = market["minerFee"].get<double>()};
}

json Shapeshift::recentTransaction(uint32_t max)
{
    Request req;
    std::ostringstream stream;
    stream << max;
    json res = req.get(_host + "recenttx/" + stream.str());
    _throw_error_if_any(res);
    return res;
}

status_t Shapeshift::depositStatus(hash_t address)
{
    Request req;
    json res = req.get(_host + "txStat/" + address);
    _throw_error_if_any(res);
    std::string status = res["status"].get<std::string>();
    if (status == "complete") {
        return status_t::complete;
    }
    if (status == "received") {
        return status_t::received;
    }
    if (status == "no_deposits") {
        return status_t::no_deposists;
    }

    return status_t::failed;
}
std::pair<status_t, uint32_t> Shapeshift::timeRemeaningForTransaction(
    hash_t address)
{
    Request req;
    json res = req.get(_host + "timeremaining/" + address);
    _throw_error_if_any(res);

    std::string status = res["status"];
    status_t s = status_t::expired;
    if (status == "pending") {
        s = status_t::pending;
    }

    return std::pair(s, res["seconds_remaining"].get<uint32_t>());
}

std::map<std::string, coin_t> Shapeshift::coins()
{
    Request req;
    json res = req.get(_host + "getcoins/");
    _throw_error_if_any(res);
    return res;
}

std::vector<shapeshift_tx_t> Shapeshift::transactionsList()
{
    if (_affiliate_private_key.empty()) {
        throw std::runtime_error(
            "transactionsList require an affiliate private key");
    }
    Request req;
    json res = req.get(_host + "txbyapikey/" + _affiliate_private_key);
    _throw_error_if_any(res);
    return res;
}

std::vector<shapeshift_tx_t> Shapeshift::transactionsList(hash_t address)
{
    if (_affiliate_private_key.empty()) {
        throw std::runtime_error(
            "transactionList require an affiliate private key");
    }
    Request req;
    json res = req.get(_host + "txbyaddress/" + address + "/" +
                       _affiliate_private_key);
    _throw_error_if_any(res);
    return res;
}

std::map<std::string, std::string> Shapeshift::_shift_params(
    currency_pair_t pair, hash_t return_addr, hash_t withdrawal_addr)
{
    std::map<std::string, std::string> body;
    if (!_affiliate_private_key.empty()) {
        body["apiKey"] = _affiliate_private_key;
    }
    body["withdrawal"] = withdrawal_addr;
    body["returnAddress"] = return_addr;
    body["pair"] = pair.str();
    return body;
}

hash_t Shapeshift::shift(currency_pair_t pair, hash_t return_addr,
                         hash_t withdrawal_addr)
{
    std::map<std::string, std::string> body =
        _shift_params(pair, return_addr, withdrawal_addr);
    json data = json(body);
    Request req;
    json res = req.post(_host + "shift", data);
    _throw_error_if_any(res);
    return hash_t(res["deposit"]);
}

hash_t Shapeshift::shift(currency_pair_t pair, hash_t return_addr,
                         hash_t withdrawal_addr, double amount)
{
    std::map<std::string, std::string> body =
        _shift_params(pair, return_addr, withdrawal_addr);
    body["amount"] = std::to_string(amount);
    json data = json(body);
    Request req;
    json res = req.post(_host + "sendamount", data);
    _throw_error_if_any(res);
    return hash_t(res["success"]["deposit"]);
}

json Shapeshift::quotedPrice(currency_pair_t pair, double amount)
{
    std::map<std::string, std::string> body;
    if (!_affiliate_private_key.empty()) {
        body["apiKey"] = _affiliate_private_key;
    }
    body["amount"] = std::to_string(amount);
    body["pair"] = pair.str();
    Request req;
    json data = json(body);
    json res = req.post(_host + "sendamount", data);
    _throw_error_if_any(res);
    return res["success"];
}

void Shapeshift::cancel(hash_t deposit_address)
{
    Request req;
    json data = {{"address", {deposit_address}}};
    json res = req.post(_host + "cancelpending", data);
    _throw_error_if_any(res);
}

bool Shapeshift::sendReceipt(std::string email, hash_t txid)
{
    Request req;
    json data = {{"email", email}, {"txid", txid}};
    json res = req.post(_host + "mail", data);
    _throw_error_if_any(res);
    return res["status"].get<std::string>() == "success";
}

}  // end at namespace
