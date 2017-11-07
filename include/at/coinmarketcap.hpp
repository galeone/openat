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

#ifndef AT_COINMARKETCAP_H_
#define AT_COINMARKETCAP_H_

#include <query/src/Document.h>
#include <query/src/Node.h>
#include <at/exceptions.hpp>
#include <at/market.hpp>
#include <at/types.hpp>
#include <chrono>

namespace at {

/* Client for CoinMarketCap JSON API
 * Documentation: https://coinmarketcap.com/api/
 *
 * Every method can throw a response_error or a server_error.
 * A response_error is when the API handles the request but for some
 * reason
 * an error occuurs.
 *
 * A server_error is when the status code of the request is != 200.
 * */
class CoinMarketCap : private Thrower {
private:
    const std::string _host = "https://api.coinmarketcap.com/v1/";
    const std::string _reverse_host = "https://coinmarketcap.com/";
    std::map<std::string, std::string> _symbol_to_id;

public:
    CoinMarketCap()
    {
        auto infos = ticker();
        for (const auto& info : infos) {
            _symbol_to_id[info.symbol] = info.id;
        }
    }
    ~CoinMarketCap() {}

    std::vector<cm_ticker_t> ticker();
    std::vector<cm_ticker_t> ticker(uint32_t limit);
    cm_ticker_t ticker(std::string currency_symbol);
    std::vector<cm_market_t> markets(std::string currency_symbol);
    gm_data_t global();
};

}  // end namespace at

#endif  // AT_COINMARKETCAP_H_
