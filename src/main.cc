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

#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>
#include <spdlog/spdlog.h>
#include <at/coinmarketcap.hpp>
#include <at/crypt/namespace.hpp>
#include <at/exchange.hpp>
#include <at/kraken.hpp>
#include <at/market.hpp>
#include <at/namespace.hpp>
#include <at/request.hpp>
#include <at/shapeshift.hpp>
#include <chrono>
#include <ctime>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <memory>
#include <set>
#include <thread>

using namespace at;
using namespace std::chrono_literals;

// Stupid but fast hash function
constexpr int _hash(const char* value)
{
    if (value == NULL) {
        return 0;
    }
    int ret = 0, i = 0;
    while (value[i] != '\0') {
        ret |= value[i] << (i % 8);
        i++;
    }
    return ret;
}

// _markets returns a map of initialized markets from the configuration file
std::map<std::string, std::unique_ptr<Market>> _markets(json config)
{
    std::map<std::string, std::unique_ptr<Market>> ret;
    json markets = config["markets"];

    for (json::iterator it = markets.begin(); it != markets.end(); ++it) {
        switch (_hash(it.key().c_str())) {
            case _hash("kraken"): {
                auto value = *it;
                if (value.find("otp") != value.end()) {
                    ret.insert(std::pair(
                        "kraken", std::make_unique<Kraken>(value["apiKey"],
                                                           value["apiSecret"],
                                                           value["otp"])));
                }
                else {
                    ret.insert(std::pair(
                        "kraken", std::make_unique<Kraken>(
                                      value["apiKey"], value["apiSecret"])));
                }
            } break;
            default:
                std::stringstream ss;
                ss << it.key() << " is not a valid key";
                throw std::runtime_error(ss.str());
        }
    }

    return ret;
}

// _exchanges returns a map of initialized exchanges from the configuration file
std::map<std::string, std::unique_ptr<Exchange>> _exchanges(json config)
{
    std::map<std::string, std::unique_ptr<Exchange>> ret;
    json exchanges = config["exchanges"];

    for (json::iterator it = exchanges.begin(); it != exchanges.end(); ++it) {
        switch (_hash(it.key().c_str())) {
            case _hash("shapeshift"): {
                auto value = *it;
                if (value.find("affiliatePrivateKey") != value.end()) {
                    ret.insert(std::pair(
                        "shapeshift",
                        std::make_unique<Shapeshift>(
                            value["affiliatePrivateKey"].get<std::string>())));
                }
            } break;
            default:
                std::stringstream ss;
                ss << it.key() << " is not a valid key";
                throw std::runtime_error(ss.str());
        }
    }

    // Shapeshift is always available because it does not require any
    // credentials
    if (ret.find("shapeshift") == ret.end()) {
        ret.insert(std::pair("shapeshift", std::make_unique<Shapeshift>()));
    }
    return ret;
}

int main()
{
    // If we cant' create table, let the process die brutally
    SQLite::Database db("db.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db.exec(
        "CREATE TABLE IF NOT EXISTS monitored_pairs("
        "market text,"
        "base text,"
        "quote text,"
        "day_volume_usd big int,"
        "day_volume_btc double,"
        "price_usd double,"
        "price_btc double,"
        "percent_volume real,"
        "time datetime default current_timestamp)");
    db.exec(
        "CREATE TABLE IF NOT EXISTS monitored_currencies("
        "currency text,"
        "time datetime,"
        "price_btc double,"
        "price_usd double,"
        "day_volume_usd big int,"
        "market_cap_usd big int,"
        "percent_change_1h real,"
        "percent_change_24h real,"
        "percent_change_7d real)");

    // Parse ./config.json
    json config;
    try {
        std::ifstream ifconf("config.json");
        ifconf >> config;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << e.what() << ": missing ./config.json file?" << std::endl;
        return 1;
    }

    // From config, instantiate configured markets and exchanges
    auto markets = _markets(config);
    auto exchanges = _exchanges(config);

    // If, instead, we're here, we handle the execution of everything,
    // logging every exception to the error_logger. When an exceptin occurs,
    // log it on the error_logger, wait for 2 seconds and retry.
    auto error_logger = spdlog::rotating_logger_mt(
        "file_error_logger", "error.log", 1024 * 1024 * 5, 3);

    // console logger is used to show messages, instead of cout
    auto console_logger = spdlog::stdout_color_mt("console");

    // begin currencies monitor function
    auto currencies_monitor = [&]() {
        console_logger->info(
            "Started currencies_monitor thread\nMonitoring: {}",
            config["monitor"]["currencies"].dump());

        std::vector<std::string> currencies = config["monitor"]["currencies"];
        SQLite::Statement query(
            db,
            "INSERT INTO monitored_currencies"
            "(currency,time,price_btc,price_usd,"
            "day_volume_usd,market_cap_usd,percent_change_1h,"
            "percent_change_24h,percent_change_7d) VALUES ("
            "?, datetime(?, 'unixepoch'), ?, ?, ?, ?, ?, ?, ?)");
        auto cmc = CoinMarketCap();
        while (true) {
            auto i = 0;
            for (const auto& currency : currencies) {
                auto tick = cmc.ticker(currency);
                SQLite::bind(query,
                             tick.symbol,  // currency
                             static_cast<long long int>(
                                 tick.last_updated),  // time, requires
                                                      // a well known
                                                      // type
                             tick.price_btc, tick.price_usd,
                             tick.day_volume_usd, tick.market_cap_usd,
                             tick.percent_change_1h, tick.percent_change_24h,
                             tick.percent_change_24h);
                query.exec();
                // Reset prepared statement, so it's ready to be
                // re-executed
                query.reset();

                i++;
                if ((i % 10) == 0) {
                    i = 0;
                    std::this_thread::sleep_for(1min);
                }
            }
            std::this_thread::sleep_for(15min);
        }
    };
    // end currencies monitor function

    // begin pairs monitor function
    auto pairs_monitor = [&]() {
        console_logger->info("Started paris_monitor thread\nMonitoring: {}",
                             config["monitor"]["pairs"].dump());
        std::vector<currency_pair_t> pairs;
        std::map<std::string, std::set<std::string>> aggregator;
        for (const auto& pair : config["monitor"]["pairs"]) {
            auto real_pair = currency_pair_t(pair.at(0), pair.at(1));
            pairs.push_back(real_pair);
            aggregator[real_pair.first].insert(real_pair.second);
        }

        SQLite::Statement query(
            db,
            "INSERT INTO monitored_pairs("
            "market,base,quote,day_volume_usd, day_volume_btc,"
            "price_usd,price_btc,percent_volume)"
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        auto cmc = CoinMarketCap();
        while (true) {
            for (const auto & [ base, quotes ] : aggregator) {
                auto markets = cmc.markets(base);
                for (const auto& market : markets) {
                    if (quotes.find(market.pair.second) != quotes.end()) {
                        SQLite::bind(query, market.name, market.pair.first,
                                     market.pair.second, market.day_volume_usd,
                                     market.day_volume_btc, market.price_usd,
                                     market.price_btc, market.percent_volume);
                        query.exec();
                        // Reset prepared statement, so it's ready to be
                        // re-executed
                        query.reset();
                    }
                }
                std::this_thread::sleep_for(2s);
            }
            std::this_thread::sleep_for(15min);
        }
    };
    // end pairs monitor function

    // begin intramarket profit maker
    auto intramarket_profit_maker = [&]() {};
    // end intramarket profit maker
    while (true) {
        try {
            // Thread that monitors coinmarketcap and saves infos about
            // monitored pairs
            std::thread pairs_monitor_thread(pairs_monitor);

            // Thread that monitors coinmarketcap and saves info about
            // monitored currencies
            std::thread currencies_monitor_thread(currencies_monitor);

            // Daemonize: wait for endless threads
            currencies_monitor_thread.join();
            pairs_monitor_thread.join();
        }
        catch (const at::response_error& e) {
            // pair unavailable for trade, for instance
            error_logger->error("at::response_error: {}", e.what());
        }
        catch (const at::server_error& e) {
            error_logger->error("at::server_error: {}", e.what());
        }
        catch (const curlpp::RuntimeError& e) {
            error_logger->error("curlpp::RuntimeError: {}", e.what());
        }
        catch (const curlpp::LogicError& e) {
            error_logger->error("curlpp::LogicError: {}", e.what());
        }
        catch (const std::logic_error& e) {
            error_logger->error("std::logic_error: {}", e.what());
        }
        catch (...) {
            error_logger->error("{}: unknown failure",
                                typeid(std::current_exception()).name());
        }
    }

    return 0;
}
