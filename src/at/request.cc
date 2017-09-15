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

#include <at/exceptions.hpp>
#include <at/request.hpp>
#include <at/types.hpp>

namespace at {

using namespace curlpp::options;

std::string Request::getHTML(std::string url) { return _get(url); }

json Request::get(std::string url) { return json::parse(_get(url)); }

std::string Request::_get(std::string url)
{
    curlpp::Easy req;

    req.setOpt(Url(url));
    req.setOpt(FollowLocation(true));
    req.setOpt(SslVersion(CURL_SSLVERSION_TLSv1_2));

    if (!_headers.empty()) {
        req.setOpt(HttpHeader(_headers));
    }
    std::ostringstream stream;
    req.setOpt(WriteStream(&stream));
    req.perform();

    long code = curlpp::infos::ResponseCode::get(req);
    if (code == 200L) {
        return stream.str();
    }

    stream.str("");
    stream.clear();
    stream << "GET " << url << "; status = " << code;
    throw server_error(stream.str());
}

json Request::post(std::string url, json params)
{
    curlpp::Easy req;

    req.setOpt(Url(url));
    req.setOpt(FollowLocation(true));
    req.setOpt(SslVersion(CURL_SSLVERSION_TLSv1_2));

    std::list<std::string> headers({"Content-Type: application/json"});
    if (!_headers.empty()) {
        headers.insert(headers.end(), _headers.begin(), _headers.end());
    }
    req.setOpt(HttpHeader(headers));

    // Write params to a stream and use the stream to
    // convert to string
    std::ostringstream stream;
    stream << params;
    std::string data = stream.str();
    req.setOpt(PostFields(data));
    req.setOpt(PostFieldSize(data.length()));

    stream.str("");
    stream.clear();
    req.setOpt(WriteStream(&stream));
    req.perform();

    long code = curlpp::infos::ResponseCode::get(req);
    if (code == 200L) {
        return json::parse(stream.str());
    }

    stream.str("");
    stream.clear();
    stream << "POST JSON" << url << "; status = " << code;
    throw server_error(stream.str());
}

json Request::post(std::string url,
                   std::vector<std::pair<std::string, std::string>> params)
{
    curlpp::Easy req;

    req.setOpt(Url(url));
    req.setOpt(FollowLocation(true));
    req.setOpt(SslVersion(CURL_SSLVERSION_TLSv1_2));

    std::list<std::string> headers(
        {"Content-Type: application/x-www-form-urlencoded"});
    if (!_headers.empty()) {
        headers.insert(headers.end(), _headers.begin(), _headers.end());
    }
    req.setOpt(HttpHeader(headers));

    std::ostringstream stream;
    for (auto& pair : params) {
        std::string key(pair.first);
        std::string value(pair.second);
        stream << key << "=" << curlpp::escape(value) << "&";
    }

    auto postFields = stream.str();
    postFields.pop_back();  // remove last &
    req.setOpt(PostFields(postFields));
    req.setOpt(PostFieldSize(postFields.length()));

    stream.str("");
    stream.clear();
    req.setOpt(WriteStream(&stream));
    req.perform();

    long code = curlpp::infos::ResponseCode::get(req);
    if (code == 200L) {
        return json::parse(stream.str());
    }

    stream.str("");
    stream.clear();
    stream << "POST " << url << "; status = " << code;
    throw server_error(stream.str());
}

}  // end namespace at
