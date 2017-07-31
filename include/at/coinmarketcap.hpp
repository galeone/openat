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

namespace at {

// Cumulative Ticker
typedef struct {
    std::string id, name, symbol;
    int rank;
    double price_usd, price_btc;
    unsigned long long int day_volume_usd, market_cap_usd, available_supply,
        total_supply;
    float percent_change_1h, percent_change_24h, percent_change_7d;
    std::time_t last_updated;
} cm_ticker_t;

// Global markets data
typedef struct {
    unsigned long long int total_market_cap_usd, total_24h_volume_usd;
    float bitcoin_percentage_of_market_cap;
    int active_currencies, active_assets, active_markets;
} gm_data_t;

// Cumulative market info per currency
typedef struct {
    std::string name;
    currency_pair_t pair;
    unsigned long long int day_volume_usd;
    double day_volume_btc;
    double price_usd, price_btc;
    float percent_volume;
} cm_market_t;

inline void to_json(json& j, const gm_data_t& t)
{
    j = json{{"total_market_cap_usd", t.total_market_cap_usd},
             {"total_24h_volume_usd", t.total_24h_volume_usd},
             {"bitcoin_percentage_of_market_cap",
              t.bitcoin_percentage_of_market_cap},
             {"active_currencies", t.active_currencies},
             {"active_assets", t.active_assets},
             {"active_markets", t.active_markets}};
}

inline void from_json(const json& j, gm_data_t& t)
{
    t.total_market_cap_usd = static_cast<unsigned long long int>(
        j.at("total_market_cap_usd").get<double>());
    t.total_24h_volume_usd = static_cast<unsigned long long int>(
        j.at("total_24h_volume_usd").get<double>());
    t.bitcoin_percentage_of_market_cap =
        j.at("bitcoin_percentage_of_market_cap").get<float>();
    t.active_currencies = j.at("active_currencies").get<int>();
    t.active_assets = j.at("active_assets").get<int>();
    t.active_markets = j.at("active_markets").get<int>();
}

inline void to_json(json& j, const cm_ticker_t& t)
{
    j = json{{"id", t.id},
             {"name", t.name},
             {"symbol", t.symbol},
             {"rank", t.rank},
             {"price_usd", t.price_usd},
             {"price_btc", t.price_btc},
             {"24h_volume_usd", t.day_volume_usd},
             {"market_cap_usd", t.market_cap_usd},
             {"available_supply", t.available_supply},
             {"total_supply", t.total_supply},
             {"percent_change_1h", t.percent_change_1h},
             {"percent_change_24h", t.percent_change_24h},
             {"percent_change_7d", t.percent_change_7d},
             {"last_updated", t.last_updated}};
}

inline void from_json(const json& j, cm_ticker_t& t)
{
    t.id = j.at("id").get<std::string>();
    t.name = j.at("name").get<std::string>();
    t.symbol = j.at("symbol").get<std::string>();
    t.rank = std::stoi(j.at("rank").get<std::string>());
    t.price_usd = std::stod(numeric_string(j.at("price_usd")));
    t.price_btc = std::stod(numeric_string(j.at("price_btc")));
    t.day_volume_usd = std::stoull(numeric_string(j.at("24h_volume_usd")));
    t.market_cap_usd = std::stoull(numeric_string(j.at("market_cap_usd")));
    t.available_supply = std::stoull(numeric_string(j.at("available_supply")));
    t.total_supply = std::stoull(numeric_string(j.at("total_supply")));
    t.percent_change_1h = std::stof(numeric_string(j.at("percent_change_1h")));
    t.percent_change_24h =
        std::stof(numeric_string(j.at("percent_change_24h")));
    t.percent_change_7d = std::stof(numeric_string(j.at("percent_change_7d")));
    t.last_updated = std::stol(numeric_string(j.at("last_updated")));
}

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

public:
    CoinMarketCap() {}
    ~CoinMarketCap() {}

    std::vector<cm_ticker_t> ticker();
    std::vector<cm_ticker_t> ticker(uint32_t limit);
    cm_ticker_t ticker(std::string currency_name);
    std::vector<cm_market_t> markets(std::string currency_name);
    gm_data_t global();
};

}  // end namespace at

#endif  // AT_COINMARKETCAP_H_
