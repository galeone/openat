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

#include <at/coinmarketcap.hpp>

namespace at {

std::vector<cm_ticker_t> CoinMarketCap::ticker()
{
    Request req;
    json res = req.get(_host + "ticker/");
    _throw_error_if_any(res);
    return res;
}

std::vector<cm_ticker_t> CoinMarketCap::ticker(uint32_t limit)
{
    Request req;
    std::ostringstream stream;
    stream << limit;
    json res = req.get(_host + "ticker/?limit=" + stream.str());
    _throw_error_if_any(res);
    return res;
}

cm_ticker_t CoinMarketCap::ticker(std::string currency_name)
{
    Request req;
    json res = req.get(_host + "ticker/" + currency_name)[0];
    _throw_error_if_any(res);
    return res;
}

gm_data_t CoinMarketCap::global()
{
    Request req;
    json res = req.get(_host + "global");
    _throw_error_if_any(res);
    return res;
}

}  // end at namespace
