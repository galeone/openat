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

#ifndef AUTOT_EXCEPTIONS_H_
#define AUTOT_EXCEPTIONS_H_

#include <exception>
#include <stdexcept>

namespace at {

class server_error : public std::runtime_error {
public:
    server_error(std::string message) : runtime_error(message) {}
};

class response_error : public std::runtime_error {
public:
    response_error(std::string message) : runtime_error(message) {}
};
}  // end namespace at

#endif  // AUTOT_EXCEPTIONS_H_
