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

#ifndef AUTOT_REQUEST_H_
#define AUTOT_REQUEST_H_

#include <curl/curl.h>
#include <at/types.hpp>
#include <cstring>
#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <iostream>
#include <json.hpp>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace at {

class Request {
private:
    std::list<std::string> _headers;
    std::string _get(std::string url);

public:
    Request() {}
    Request(std::list<std::string> headers) : _headers(headers) {}

    json get(std::string);
    std::string getHTML(std::string url);
    json post(std::string, json);
    json post(std::string, std::vector<std::pair<std::string, std::string>>);
};

}  // end namespace at

#endif  // AUTOT_REQUEST_H_
