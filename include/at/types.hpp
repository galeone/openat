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

#ifndef AUTOT_TYPE_H_
#define AUTOT_TYPE_H_

#include <json.hpp>
#include <string>
#include <utility>

namespace at {

using json = nlohmann::json;

// Handle user defined types & json:
// https://github.com/nlohmann/json#basic-usage
//
// Functions included in multiple source files must be inline
// http://en.cppreference.com/w/cpp/language/inline

typedef std::pair<std::string, std::string> currency_pair_t;

inline void to_json(json& j, const currency_pair_t c)
{
    j = json{c.first, c.second};
}
inline void from_json(const json& j, currency_pair_t& c)
{
    c.first = j[0].get<std::string>();
    c.second = j[1].get<std::string>();
}

typedef std::string address_t;
inline void to_json(json& j, const address_t& a) { j = json::parse(a); }
inline void from_json(const json& j, address_t& a) { a = j.get<std::string>(); }

typedef struct {
    std::string name, symbol, image, status;
} coin_t;
inline void to_json(json& j, const coin_t& c)
{
    j = json{{"name", c.name},
             {"symbol", c.symbol},
             {"image", c.image},
             {"status", c.status}};
}
inline void from_json(const json& j, coin_t& c)
{
    c.name = j["name"].get<std::string>();
    c.symbol = j["symbol"].get<std::string>();
    c.image = j["image"].get<std::string>();
    c.status = j["status"].get<std::string>();
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
    d.min = j["min"].get<double>();
    d.max = j["max"].get<double>();
}

typedef struct {
    deposit_limit_t limit;
    double rate;
    double miner_fee;
} market_info_t;
inline void to_json(json& j, const market_info_t& m)
{
    j = json{{"limit", m.limit}, {"rate", m.rate}, {"miner_fee", m.miner_fee}};
}
inline void from_json(const json& j, market_info_t& m)
{
    m.limit = j["limit"].get<deposit_limit_t>();
    m.rate = j["rate"].get<double>();
    m.miner_fee = j["miner_fee"].get<double>();
}

}  // end namespace at

#endif  // AUTOT_TYPE_H_
