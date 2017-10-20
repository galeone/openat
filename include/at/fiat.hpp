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

#ifndef AT_FIAT_H_
#define AT_FIAT_H_

#include <at/exceptions.hpp>
#include <at/market.hpp>
#include <at/types.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <rapidxml.hpp>

namespace at {

/* Client for the daily fiat exchange rates at
 * https://www.ecb.europa.eu
 *
 * Every method can throw a response_error or a server_error.
 * A response_error is when the API handles the request but for some
 * reason an error occuurs.
 *
 * A server_error is when the status code of the request is != 200.
 * */
class Fiat : private Thrower {
private:
    const std::string _host = "https://www.ecb.europa.eu/";
    std::time_t _current_date = std::time(0);
    std::map<std::string, double> _eur_to_currency_rate;

    // _update fetch and updates the XML with the update time and the
    // exhange rates
    void _update();

public:
    Fiat() { _update(); }
    ~Fiat() {}

    double rate(const currency_pair_t&);
};

}  // end namespace at

#endif  // AT_FIAT_H_
