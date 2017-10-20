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

#include <at/fiat.hpp>

namespace at {

void Fiat::_update()
{
    // save the document & today date, at the 14:15 CET (12:15 GMT), ref:
    // https://www.ecb.europa.eu/stats/policy_and_exchange_rates/euro_reference_exchange_rates/html/index.en.html
    const char *precise_format = "%Y-%m-%d %H:%M";
    std::time_t now = std::time(0);
    std::tm *now_tm = std::gmtime(&now);
    std::ostringstream oss;
    const char *day_format = "%Y-%m-%d";
    oss << std::put_time(now_tm, day_format);
    std::tm tm{};  // initialize value
    std::stringstream ss;
    ss << oss.str();
    ss << " 12:15";
    ss >> std::get_time(&tm, precise_format);
    auto current_date = std::mktime(&tm);

    if (labs(_current_date - current_date) < 24 * 60 * 60) {
        Request req;
        std::string page =
            req.getHTML(_host + "stats/eurofxref/eurofxref-daily.xml");

        rapidxml::xml_document<char> doc;
        auto constant_page = page.c_str();
        char *ptr_page = new char[page.length() + 1];
        std::memcpy(ptr_page, constant_page, page.length() + 1);
        doc.parse<0>(ptr_page);
        auto cube = doc.first_node()->first_node("Cube");
        if (!cube) {
            throw std::runtime_error("Unable to find Cube element on " + _host +
                                     "stats/eurofxref/eurofxref-daily.xml");
        }

        cube = cube->first_node("Cube");
        if (!cube) {
            throw std::runtime_error(
                "Unable to find Cube element under Cube element on " + _host +
                "stats/eurofxref/eurofxref-daily.xml");
        }
        oss.str("");
        oss.clear();

        ss.str("");
        ss.clear();

        tm = {};
        ss << cube->first_attribute("time")->value();
        ss << " 12:15";
        ss >> std::get_time(&tm, precise_format);

        // parse cubes
        auto cubes = cube->first_node("Cube");
        if (!cubes) {
            throw std::runtime_error(
                "Unable to find Cube element list under Cube->Cube tag on " +
                _host + "stats/eurofxref/eurofxref-daily.xml");
        }

        for (auto cube = cubes; cube; cube = cube->next_sibling()) {
            auto currency =
                std::string(cube->first_attribute("currency")->value());
            toupper(currency);
            auto rate = std::stod(cube->first_attribute("rate")->value());
            _eur_to_currency_rate[currency] = rate;
        }
        _eur_to_currency_rate["EUR"] = 1.;
        _current_date = std::mktime(&tm);
    }
}

// rate returns the exchange rate of the fiat pair.
// eg. pair(eur,usd) -> amount of usd to buy/sell eur
double Fiat::rate(const currency_pair_t &pair)
{
    _update();
    std::string base, quote;
    base = pair.first;
    quote = pair.second;

    toupper(base);
    toupper(quote);

    // First convert eveything to EUR quotation, than change from
    // base to quote
    if (base == "EUR") {
        return _eur_to_currency_rate[quote];
    }

    if (quote == "EUR") {
        return 1. / _eur_to_currency_rate[base];
    }

    // (1 / number of base for eur) * (number of quote for eur)
    // number of quote for base
    return _eur_to_currency_rate[quote] / _eur_to_currency_rate[base];
}

}  // namespace at
