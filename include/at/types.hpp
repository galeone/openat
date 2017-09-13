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

#ifndef AT_TYPE_H_
#define AT_TYPE_H_

#include <exception>
#include <json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace at {

static inline void toupper(std::string& str)
{
    transform(str.begin(), str.end(), str.begin(), ::toupper);
}

static inline void tolower(std::string& str)
{
    transform(str.begin(), str.end(), str.begin(), ::tolower);
}

// Remember:
// Functions included in multiple source files must be inline
// http://en.cppreference.com/w/cpp/language/inline
//
// Handle user defined types & json:
// https://github.com/nlohmann/json#basic-usage

using json = nlohmann::json;

// Returns a std::string if field is a string
// Otherwise returns the string "0.0"
// Throws runtime_error otherwise
inline std::string numeric_string(const json& field)
{
    if (field.is_string()) {
        return field.get<std::string>();
    }
    if (field.is_null()) {
        return std::string("0.0");
    }
    std::ostringstream stream;
    stream << "field " << field << " is not string or null";
    throw std::runtime_error(stream.str());
}

class currency_pair_t {
private:
    std::pair<std::string, std::string> _pair;

public:
    std::string first, second;
    currency_pair_t() {}
    currency_pair_t(const std::string& first, const std::string& second)
    {
        std::string ucFirst = first;
        std::string ucSecond = second;
        toupper(ucFirst);
        toupper(ucSecond);
        _pair = std::pair<std::string, std::string>(ucFirst, ucSecond);
        this->first = _pair.first;
        this->second = _pair.second;
    }

    std::string str() const { return first + "_" + second; }
};

// overload of << between ostream and currency_pair_t
inline std::ostream& operator<<(std::ostream& o, const currency_pair_t& pair)
{
    return o << pair.str();
}

inline void to_json(json& j, const currency_pair_t c)
{
    j = json{c.first, c.second};
}
inline void from_json(const json& j, currency_pair_t& c)
{
    c.first = j.at(0).get<std::string>();
    c.second = j.at(1).get<std::string>();
}

typedef std::string hash_t;
inline void to_json(json& j, const hash_t& a) { j = json::parse(a); }
inline void from_json(const json& j, hash_t& a) { a = j.get<std::string>(); }

typedef struct {
    std::string name, symbol, status;
} coin_t;
inline void to_json(json& j, const coin_t& c)
{
    j = json{{"name", c.name}, {"symbol", c.symbol}, {"status", c.status}};
}
inline void from_json(const json& j, coin_t& c)
{
    c.name = j.at("name").get<std::string>();
    c.symbol = j.at("symbol").get<std::string>();
    c.status = j.at("status").get<std::string>();
}

typedef struct {
    double min;
    double max;
} deposit_limit_t;

inline void to_json(json& j, const deposit_limit_t& d)
{
    j = json{{"min", d.min}, {"max", d.max}};
}
inline void from_json(const json& j, deposit_limit_t& d)
{
    d.min = j.at("min").get<double>();
    d.max = j.at("max").get<double>();
}

typedef struct {
    deposit_limit_t limit;
    double fee;
    std::string currency;
    std::string method;
} deposit_info_t;

inline void to_json(json& j, const deposit_info_t& d)
{
    j = json{{"limit", d.limit},
             {"fee", d.fee},
             {"currency", d.currency},
             {"method", d.method}};
}
inline void from_json(const json& j, deposit_info_t& d)
{
    d.limit.max = j["limit"]["max"].get<double>();
    d.limit.min = j["limit"]["min"].get<double>();
    d.fee = j.at("fee").get<double>();
    d.currency = j.at("currency").get<std::string>();
    d.method = j.at("method").get<std::string>();
}

typedef struct {
    currency_pair_t pair;
    deposit_limit_t limit;
    double rate;
    double miner_fee;
} exchange_info_t;
inline void to_json(json& j, const exchange_info_t& m)
{
    j = json{{"pair", m.pair.str()},
             {"limit", m.limit},
             {"rate", m.rate},
             {"miner_fee", m.miner_fee}};
}
inline void from_json(const json& j, exchange_info_t& m)
{
    m.limit = j.at("limit").get<deposit_limit_t>();
    m.rate = j.at("rate").get<double>();
    m.miner_fee = j.at("miner_fee").get<double>();
    auto pair_str = j.at("pair").get<std::string>();
    std::size_t delimiter_it = pair_str.find_last_of("_");
    if (delimiter_it != std::string::npos) {
        m.pair = currency_pair_t(pair_str.substr(0, delimiter_it),
                                 pair_str.substr(delimiter_it + 1));
    }
}

typedef struct {
    currency_pair_t pair;
    deposit_limit_t limit;
    double maker_fee, taker_fee;
} market_info_t;
inline void to_json(json& j, const market_info_t& m)
{
    j = json{{"pair", m.pair.str()},
             {"limit", m.limit},
             {"taker_fee", m.taker_fee},
             {"maker_fee", m.maker_fee}};
}
inline void from_json(const json& j, market_info_t& m)
{
    m.limit = j.at("limit").get<deposit_limit_t>();
    m.maker_fee = j.at("maker_fee").get<double>();
    m.taker_fee = j.at("taker_fee").get<double>();
    auto pair_str = j.at("pair").get<std::string>();
    std::size_t delimiter_it = pair_str.find_last_of("_");
    if (delimiter_it != std::string::npos) {
        m.pair = currency_pair_t(pair_str.substr(0, delimiter_it),
                                 pair_str.substr(delimiter_it + 1));
    }
}

enum class status_t : char {
    no_deposists = 'a',
    initial,   // initial
    received,  // received
    complete,  // = success
    settled,   // settled
    pending,   // pending
    failed,    // failure
    partial,   // partial
    expired,   // experied
};

inline std::ostream& operator<<(std::ostream& o, const status_t& s)
{
    switch (s) {
        case status_t::no_deposists:
            return o << "no_deposists";
        case status_t::initial:
            return o << "initial";
        case status_t::received:
            return o << "received";
        case status_t::complete:
            return o << "complete";
        case status_t::settled:
            return o << "settled";
        case status_t::pending:
            return o << "pending";
        case status_t::failed:
            return o << "failed";
        case status_t::partial:
            return o << "partial";
        default:
            return o << "expired";
    }
}

typedef struct {
    double price;
    double amount;
    time_t time;
} quotation_t;

typedef struct {
    quotation_t bid;
    quotation_t ask;
} ticker_t;

typedef struct {
    hash_t txid;            // transaction ID
    std::string status;     // open, closed, cancelled
    std::string ordertype;  // limit & co
    std::string type;       // buy/sell
    currency_pair_t pair;
    time_t open;
    time_t close;
    double volume;
    double cost;
    double fee;
    double price;
} order_t;

typedef struct {
    std::string inputTXID, inputAddress, inputCurrency;
    double inputAmount;
    std::string outputTXID, outputAddress, outputCurrency, outputAmount,
        shiftRate, status;
} shapeshift_tx_t;

inline void to_json(json& j, const shapeshift_tx_t& t)
{
    j = json{{"inputTXID", t.inputTXID},
             {"inputAddress", t.inputAddress},
             {"inputCurrency", t.inputCurrency},
             {"inputAmount", t.inputAmount},
             {"outputTXID", t.outputTXID},
             {"outputAddress", t.outputAddress},
             {"outputCurrency", t.outputCurrency},
             {"outputAmount", t.outputAmount},
             {"status", t.status}};
}

inline void from_json(const json& j, shapeshift_tx_t& t)
{
    t.inputTXID = j.at("inputTXID").get<std::string>();
    t.inputAddress = j.at("inputAddress").get<std::string>();
    t.inputCurrency = j.at("inputCurrency").get<std::string>();
    t.inputAmount = j.at("inputAmount").get<double>();

    t.outputAmount = j.at("outputTXID").get<std::string>();
    t.outputAmount = j.at("outputAddress").get<std::string>();
    t.outputAmount = j.at("outputCurrency").get<std::string>();
    t.outputAmount = j.at("outputAmount").get<std::string>();

    t.shiftRate = j.at("shiftRate").get<std::string>();
    t.status = j.at("status").get<std::string>();
}

// Cumulative Ticker
typedef struct {
    std::string id, name, symbol;
    int rank;
    double price_usd, price_btc;
    long long int day_volume_usd, market_cap_usd, available_supply,
        total_supply;
    float percent_change_1h, percent_change_24h, percent_change_7d;
    std::time_t last_updated;
} cm_ticker_t;

// Global markets data
typedef struct {
    long long int total_market_cap_usd, total_24h_volume_usd;
    float bitcoin_percentage_of_market_cap;
    int active_currencies, active_assets, active_markets;
} gm_data_t;

// Cumulative market info per currency
typedef struct {
    std::string name;
    currency_pair_t pair;
    long long int day_volume_usd;
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
    t.total_market_cap_usd =
        static_cast<long long int>(j.at("total_market_cap_usd").get<double>());
    t.total_24h_volume_usd =
        static_cast<long long int>(j.at("total_24h_volume_usd").get<double>());
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

}  // end namespace at

#endif  // AT_TYPE_H
