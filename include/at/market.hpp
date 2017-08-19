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

#ifndef AT_MARKET_H_
#define AT_MARKET_H_

#include <at/request.hpp>
#include <at/types.hpp>
#include <json.hpp>
#include <map>
#include <string>
#include <utility>

namespace at {

class Market {
public:
    // Only common methods among markets
    // List defined using shapeshift.io API
    // skipping methods that use private keys
    // or are shapeshift specific (to define in a specific
    // market implementation class)
    virtual ~Market() {}

    // Pure virtual methods
    virtual deposit_info_t depositInfo(std::string currency) = 0;
    virtual std::vector<market_info_t> info() = 0;
    virtual market_info_t info(currency_pair_t) = 0;
    virtual ticker_t ticker(currency_pair_t) = 0;
    virtual std::vector<ticker_t> orderBook(currency_pair_t) = 0;
    virtual std::map<std::string, coin_t> coins() = 0;
    virtual std::map<std::string, double> balance() = 0;
    virtual double balance(std::string currency) = 0;
    virtual std::vector<order_t> closedOrders() = 0;
    virtual void place(order_t&) = 0;
    virtual void cancel(order_t&) = 0;
};

}  // end namespace at

#endif  // AT_MARKET_H_
