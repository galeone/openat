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

cm_ticker_t CoinMarketCap::ticker(std::string currency_symbol)
{
    Request req;
    toupper(currency_symbol);
    auto id = _symbol_to_id.find(currency_symbol);
    currency_symbol = id != _symbol_to_id.end() ? id->second : currency_symbol;
    json res = req.get(_host + "ticker/" + currency_symbol + "/")[0];
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

std::vector<cm_market_t> CoinMarketCap::markets(std::string currency_symbol)
{
    Request req;
    toupper(currency_symbol);
    auto id = _symbol_to_id.find(currency_symbol);
    currency_symbol = id != _symbol_to_id.end() ? id->second : currency_symbol;
    std::string page =
        req.getHTML(_reverse_host + "currencies/" + currency_symbol + "/");

    CDocument doc;
    doc.parse(page.c_str());
    CSelection table = doc.find("#markets-table tbody");
    if (table.nodeNum() == 0) {
        throw std::runtime_error(
            "Unable to find table with ID markets-table, on " + _host +
            "currencies/" + currency_symbol);
    }

    std::vector<cm_market_t> ret;
    CSelection rows = table.nodeAt(0).find("tr");
    auto now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    for (size_t i = 0; i < rows.nodeNum(); ++i) {
        CNode row = rows.nodeAt(i);
        CSelection fields = row.find("td");
        if (fields.nodeNum() != 7) {
            throw std::runtime_error("Expected 6 columns, got " +
                                     std::to_string(fields.nodeNum()));
        }

        // Skip markets not updated recently
        // 6: updated
        std::string updated_string = fields.nodeAt(6).text();
        at::tolower(updated_string);
        if (updated_string != "recently") {
            continue;
        }

        // 0: rank, unused because we insert in the return vector following this
        // order
        // 1: <a link>name</a>
        std::string name = fields.nodeAt(1).find("a").nodeAt(0).text();
        // 2: <a link>cur1/cur2</a>
        std::string pair_string = fields.nodeAt(2).find("a").nodeAt(0).text();
        auto split_pos = pair_string.find("/");
        auto first = pair_string.substr(0, split_pos);
        auto second = pair_string.substr(split_pos + 1, pair_string.length());

        // 3: volumes <span data-usd="value", data-btc="value">
        CNode span = fields.nodeAt(3).find("span").nodeAt(0);
        long long int day_volume_usd = std::stoull(span.attribute("data-usd"));
        double day_volume_btc = std::stod(span.attribute("data-btc"));

        // 4: prices <span data-usd, data-btc
        span = fields.nodeAt(4).find("span").nodeAt(0);
        double price_usd = std::stod(span.attribute("data-usd"));
        double price_btc = std::stod(span.attribute("data-btc"));

        // 5: xx.yy% percentage
        std::string percentage_string = fields.nodeAt(5).text();
        // remove %
        percentage_string.pop_back();
        float percent_volume = std::stof(percentage_string);

        cm_market_t market{
            .name = name,
            .pair = currency_pair_t(first, second),
            .day_volume_usd = day_volume_usd,
            .day_volume_btc = day_volume_btc,
            .price_usd = price_usd,
            .price_btc = price_btc,
            .percent_volume = percent_volume,
            .last_updated = now,
        };
        ret.push_back(market);
    }

    return ret;
}

}  // namespace at
