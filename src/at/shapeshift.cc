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
    json res = req.get(_host + "rate/" + pair.first + "_" + pair.second);
    _throw_error_if_any(res);
    return std::stod(res["rate"].get<std::string>());
}

deposit_limit_t Shapeshift::depositLimit(currency_pair_t pair)
{
    Request req;
    json res = req.get(_host + "limit/" + pair.first + "_" + pair.second);
    _throw_error_if_any(res);
    // {"limit":"1.81514557","min":"0.000821","pair":"btc_eth"}
    return deposit_limit_t{.min = std::stod(res["min"].get<std::string>()),
                           .max = std::stod(res["limit"].get<std::string>())};
}

std::vector<market_info_t> Shapeshift::info()
{
    Request req;
    json res = req.get(_host + "marketinfo/");
    _throw_error_if_any(res);
    // ,{"limit":0.43007489,"maxLimit":0.43007489,"min":0.01802469,"minerFee":0.01,"pair":"NMC_PPC","rate":"1.06196283"}
    std::vector<market_info_t> markets(res.size());
    for (const auto& market : res) {
        markets.push_back(market_info_t{
            .limit = deposit_limit_t{.min = market["min"].get<double>(),
                                     .max = market["limit"].get<double>()},
            .rate = std::stod(
                market["rate"].get<std::string>()),  // rate is a string
            .miner_fee = market["minerFee"].get<double>()});
    }
    return markets;
}

market_info_t Shapeshift::info(currency_pair_t pair)
{
    Request req;
    json market =
        req.get(_host + "marketinfo/" + pair.first + "_" + pair.second);
    _throw_error_if_any(market);

    // {"limit":0.43558867,"maxLimit":0.43558867,"minerFee":0.01,"minimum":0.01753086,"pair":"nmc_ppc","rate":1.04852027}
    // shapeshift API is inconsistent as hell
    return market_info_t{
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

json Shapeshift::depositStatus(address_t address)
{
    Request req;
    json res = req.get(_host + "txStat/" + address);
    _throw_error_if_any(res);
    return res;
}
std::pair<std::string, uint32_t> Shapeshift::timeRemeaningForTransaction(
    address_t address)
{
    Request req;
    json res = req.get(_host + "timeremaining/" + address);
    _throw_error_if_any(res);
    std::cout << res << std::endl;
    return std::pair(res["status"], res["seconds_remaining"].get<uint32_t>());
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
            "transactionList require an affiliate private key");
    }
    Request req;
    json res = req.get(_host + "txbyapikey/" + _affiliate_private_key);
    _throw_error_if_any(res);
    return res;
}

std::vector<shapeshift_tx_t> Shapeshift::transactionsList(address_t address)
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
}  // end at namespace
