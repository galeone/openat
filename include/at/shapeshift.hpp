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

#ifndef AT_SHAPESHIFT_H_
#define AT_SHAPESHIFT_H_

#include <at/exceptions.hpp>
#include <at/exchange.hpp>
#include <at/types.hpp>

namespace at {

/* Client for ShapeShift API.
 * API doumentation available: https://info.shapeshift.io/api
 * Method descriptions are kept from that page.
 *
 * Every method can throw a response_error or a server_error.
 * A response_error is when the API handles the request but for some reason
 * an error occuurs.
 *
 * A server_error is when the status code of the request is != 200. */
class Shapeshift : public Exchange, private Thrower {
private:
    const std::string _host = "https://shapeshift.io/";
    const std::string _affiliate_private_key;
    std::map<std::string, std::string> _shift_params(currency_pair_t pair,
                                                     hash_t return_addr,
                                                     hash_t withdrawal_addr);

public:
    Shapeshift() {}
    Shapeshift(std::string affiliate_private_key)
        : _affiliate_private_key(affiliate_private_key)
    {
    }
    ~Shapeshift() {}

    /* Gets the current rate offered by Shapeshift. This is an estimate because
     * the rate can occasionally change rapidly depending on the markets. The
     * rate is also a 'use-able' rate not a direct market rate. Meaning
     * multiplying your input coin amount times the rate should give you a close
     * approximation of what will be sent out. This rate does not include the
     * transaction (miner) fee taken off every transaction. */
    double rate(currency_pair_t) override;

    /* Gets the current deposit limit set by Shapeshift. Amounts deposited over
     * this limit will be sent to the return address if one was entered,
     * otherwise the user will need to contact ShapeShift support to retrieve
     * their coins. This is an estimate because a sudden market swing could move
     * the limit. */
    deposit_limit_t depositLimit(currency_pair_t) override;

    /* This gets the exchange info (pair, rate, limit, minimum limit, miner
     * fee).
     */
    std::vector<exchange_info_t> info() override;

    /* This gets the exchange info (pair, rate, limit, minimum limit, miner fee)
     * for the spcified pair. */
    exchange_info_t info(currency_pair_t) override;

    /* Get a list of the most recent transactions.
     * max is the maximum number of transactions to return.
     * Also, max must be a number between 1 and 50 (inclusive). */
    json recentTransaction(uint32_t) override;

    /* This returns the status of the most recent deposit transaction to the
     * address. */
    deposit_status_t depositStatus(hash_t) override;

    /* When a transaction is created with a fixed amount requested there is a 10
     * minute window for the deposit. After the 10 minute window if the deposit
     * has not been received the transaction expires and a new one must be
     * created. This api call returns how many seconds are left before the
     * transaction expires. Please note that if the address is a ripple address,
     * it will include the "?dt=destTagNUM" appended on the end, and you will
     * need to use the URIEncodeComponent() function on the address before
     * sending it in as a param, to get a successful response.
     *
     * hash_t is the deposit address to look up. */
    std::pair<deposit_status_t, uint32_t> timeRemeaningForTransaction(
        hash_t) override;

    /* Allows anyone to get a list of all the currencies that Shapeshift
     * currently supports at any given time. The list will include the name,
     * symbol, availability status, and an icon link for each. */
    std::map<std::string, coin_t> coins() override;

    /* Allows vendors to get a list of all transactions that have ever been done
     * using a specific API key. Transactions are created with an affilliate
     * PUBLIC KEY, but they are looked up using the linked PRIVATE KEY, to
     * protect the privacy of our affiliates' account details. */
    std::vector<shapeshift_tx_t> transactionsList();

    /* Allows vendors to get a list of all transactions that have ever been sent
     * to one of their addresses. The affilliate's PRIVATE KEY must be provided,
     * and will only return transactions that were sent to output address AND
     * were created using / linked to the affiliate's PUBLIC KEY. Please note
     * that if the address is a ripple address and it includes the
     * "?dt=destTagNUM" appended on the end, you will need to use the
     * URIEncodeComponent() function on the address before sending it in as a
     * param, to get a successful response.
     *
     * hash_t the address that output coin was sent to for the shift. */
    std::vector<shapeshift_tx_t> transactionsList(hash_t);

    /* This is the primary data input into ShapeShift.
     * Use only certain fields of the data required by the API (no optional
     * fields for XRP or optional fields for NXT).
     * If the object was instantiate with an API Key, the API key is sent in the
     * body request.
     * Returns the address in which deposit the [input_coin] amount to convert
     * into [output_coin]
     *
     * pair = what coins are being exchanged
     * returnAddress = [input_coin address] address to return deposit to if
     * anything goes wrong with exchange
     * withdrawal = [output_coin address] the address for resulting coin to be
     * sent to. */
    hash_t shift(currency_pair_t, hash_t return_addr, hash_t withdrawal_addr);

    /* This call allows you to request a fixed amount to be sent to the
     * withdrawal address. You provide a withdrawal address and the amount you
     * want sent to it. We return the amount to deposit and the address to
     * deposit to. This allows you to use shapeshift as a payment mechanism. */
    hash_t shift(currency_pair_t, hash_t return_addr, hash_t withdrawal_addr,
                 double amount);

    /* This call requests a receipt for a transaction. The email address will be
     * added to the conduit associated with that transaction as well. (Soon it
     * will also send receipts to subsequent transactions on that conduit). */
    bool sendReceipt(std::string, hash_t);

    /* This call also allows you to request a quoted price on the amount of a
     * transaction. */
    json quotedPrice(currency_pair_t, double amount);

    /* This call allows you to request for canceling a pending transaction by
     * the deposit address. If there is fund sent to the deposit address, this
     * pending transaction cannot be canceled.
     *
     * Throws a response_error if an error occur
     * */
    void cancel(hash_t);
};

}  // end namespace at

#endif  // AT_SHAPESHIFT_H_
