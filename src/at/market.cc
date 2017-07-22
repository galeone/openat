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

#include <at/market.hpp>

namespace at {

bool Market::isValid(std::string symbol, address_t address)
{
    std::ostringstream stream;
    stream << "https://shapeshift.io/validateAddress/" << address << "/"
           << symbol;

    Request req;
    try {
        json result = req.get(stream.str());
        return result["isvalid"];  // shapeshift API claim is isValid, but
                                   // is not
    }
    catch (std::runtime_error& e) {
        return false;
    }
}

}  // end namespace at
