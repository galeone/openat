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

#ifndef AT_CRYPT_H_
#define AT_CRYPT_H_

#include <at/exceptions.hpp>
#include <string>
#include <vector>

namespace at::crypt {

std::vector<unsigned char> sha256(const std::string& data);
std::vector<unsigned char> base64_decode(const std::string& data);
std::string base64_encode(const std::vector<unsigned char>& data);
std::vector<unsigned char> hmac_sha512(const std::vector<unsigned char>& data,
                                       const std::vector<unsigned char>& key);

}  // end namespace at::crypt

#endif  // AT_CRYPT_H_
