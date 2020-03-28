# OpenAT: Open Source Algorithmic Trading Library
[![Build Status](https://travis-ci.org/galeone/openat.svg?branch=master)](https://travis-ci.org/galeone/openat)

OpenAT provides an easy to use C++ interface for working with (crypto-)currencies markets and exchanges.

The aim is to give to the user the possibility to build it's own generic (crypto-)currency trading bot or daemon (check out an example of crypto trading daemon and currency price monitor (WIP!): [Openatd: OpenAT Daemon](https://github.com/galeone/openatd/).

OpenAT is divided in 3 different parts:

1. **Market**: place/cancel orders, monitor order book, access history of placed/cancelled orders, global balance, balance per coin, ...
2. **Global market data monitoring**: crypto currency price, volume, markets, fiat value, ...
3. **Exchange**: change a currency for another currency without defining the price (shapeshift like)

Everything using a strong typing system: every request returns a well defined type, the request to the markets are created using C++ structure: you will never fill a JSON field manually.

Have a look at [`types.hpp`](https://github.com/galeone/openat/blob/master/include/at/types.hpp).

[This project is in early stage and needs your help!](https://github.com/galeone/openat#contributing)

## Examples

The best way to understand how OpenAT works is looking at the examples.

### Market

Every implemented market satisfies the contract with the [Market](https://github.com/galeone/openat/blob/master/include/at/market.hpp) interface: you can write code that works with a general `Market` and just use any available implementation.

#### Market: available coins

```cpp
// A call to `coins()` returns `std::map<std::string, coin_t>` where the key is the name of the coin
// and the value is a `coin_t` type, which contains basic informations about the coin
// like t's name, the symbol and it's status

auto coins = market->coins();
for(const auto& pair : coins) {
    auto coin = pair.second;
    std::cout << "name: " << coin.name << " symbol: " << coin.symbol << " status: "
              << coin.status << "\n";
}
```

#### Market: available pairs and pair info

```cpp
// Given the market variable `market`
// market->info() returns a std::vector<market_info_t> where each entry of the
// vector contains the information about any available pair in the market

auto pair_info = market->info();
for(const auto& info : pair_info) {
    std::cout << "pair: " << info.pair << " " << "limits: " << info.limit << " fees: "
              << "maker: " << info.maker_fee << " taker: " << info.taker_fee << "\n";
}
```

You can specify the pair you're interested in, hence:

```cpp
// information about the ETH/EUR pair
auto info = market->info(currency_pair_t("eth", "eur");
std::cout << "pair: " << info.pair << " " << "limits: " << info.limit << " fees: "
          << "maker: " << info.maker_fee << " taker: " << info.taker_fee << "\n";
```

#### Market: ticker per pair

```cpp
// given a certain pair, obtain the ticker information
auto ticker = market->ticker(currency_pair_t("eth", "eur"));

// ticker is a struct with 2 `quotation_t` fileds: bid and ask

std::cout << "bid:\n\price: " << ticker.bid.price << " amount: " << ticker.bid.amount << " time: " << ticker.bid.time << "\n";

std::cout << "ask:\n\price: " << ticker.ask.price << " amount: " << ticker.ask.amount << " time: " << ticker.ask.time << "\n";

```

##### Market: order book per pair

```cpp
// a call to the `orderBook(pair)` returns a std::vector<ticker_t>
auto order_book = market->orderBook(currency_pair_t("eth", "eur"));
for(const auto& order : order_book) {
    // handle the `ticker_t` fields, see previous example
}
```

##### Market: balance per currency and global balance

```cpp
// The method balance has 2 versions:
// 1. `std::map<std::string, double> balance()` which returns the pair `symbol`,`balance`
auto balances = market->balance();
for(const auto& pair : balances) {
    std::cout << pair.first << ": " << pair.second << "\n";
}
// 2. `double balance(std::string symbol)` which returns the balance for the specified symbol

auto btc = market->balance("BTC");
std::cout << "BTC: " << btc << "\n";
```

##### Market: list of open and closed orders

```cpp
// We can get the list of closed orders (a std::vector<order_t>) calling
auto closed_orders = market->closedOrders();
for(const auto order : closed_orders) {
    std::cout << "txid: " << order.txid
              << " status: " << order.status
              << " type: " << order.type
              << " action: " << order.action
              << " pair: " << order.pair
              << " open time: " << order.open
              << " close time: " << order.close
              << " volume: " << order.volume
              << " cost: " << order.cost
              << " fee: " << order.fee
              << " price: " << order.price;
}

// We can get the list of open orders calling
auto open_orders = market->openOrders();
for(const auto order : open_orders) {
    std::cout << "txid: " << order.txid
              << " status: " << order.status
              << " type: " << order.type
              << " action: " << order.action
              << " pair: " << order.pair
              << " open time: " << order.open
              << " close time: " << order.close
              << " volume: " << order.volume
              << " cost: " << order.cost
              << " fee: " << order.fee
              << " price: " << order.price;
}

```

##### Market: place and cancel order

```cpp
// Given an open order, close it (use the txid to identify the order on the market)
auto close_me = open_orders[0];
market->close(close_me);

// Place a limit order for a certain pair
// let's buy a litecoin with EUR
order_t limit;
limit.pair = currency_pair_t("LTC", "EUR");
limit.volume = 1;
limit.action = order_action_t::buy; // BUY
limit.type = order_type_t::limit; // limit order
// place
try {
    market->place(limit);
}
catch(...) {
    // handle the exception in case of error
}

// Place a market order (no specify the price)
// half ltc per eur
order_t market;
market.pair = currency_pair_t("LTC", "EUR");
market.volume = 0.5;
market.action = order_action_t::buy;
market.type = order_type_t::market;
// place
try {
    market->place(market);
} catch(...) {
    // handle errors
}

// Sell LTC for eur, market order
order_t market;
market.pair = currency_pair_t("LTC", "EUR");
market.volume = 0.5;
market.action = order_action_t::sell; // SELL order
market.type = order_type_t::market;
// place
try {
    market->place(market);
} catch(...) {
    // handle errors
}

```

At the moment of writing the only implementation of the Market interface is for https://kraken.com/. But pull requests for any other market are more than welcome!

## Global market data monitor

OpenAT contains a client for the [coinmarketcap.com](https://coinmarketcap.com) API and also it's able to parse the webpage of the currencies in order to extract information about certain currencies that are available only in the website and not in the API.

The interface is easy and intuitive:

```cpp
// ticker returns a vector of `cm_ticker_t`, where `cm` stands for cumulative
// The struct it's easy, so no further explaination are required:
/*
typedef struct {
    std::string id, name, symbol;
    int rank;
    double price_usd, price_btc;
    long long int day_volume_usd, market_cap_usd, available_supply,
        total_supply;
    float percent_change_1h, percent_change_24h, percent_change_7d;
    std::time_t last_updated;
} cm_ticker_t;
*/
std::vector<cm_ticker_t> ticker();

// The call to ticer(uint32_t limit) it's the same of limit() but returns only
// the first `limit` currencies
std::vector<cm_ticker_t> ticker(uint32_t limit);

// ticker(std::string currency_symbol) returns a single `cm_ticker_t`
// for the specified currency
cm_ticker_t ticker(std::string currency_symbol);

// markets(std::string currency_symbol) returns the information parsed from the website
// about the markets where the specified symbol is traded on.
// The cm_market_t struct is:
/*
typedef struct {
    std::string name;
    currency_pair_t pair;
    long long int day_volume_usd;
    double day_volume_btc;
    double price_usd, price_btc;
    float percent_volume;
    std::time_t last_updated;
} cm_market_t;
*/
std::vector<cm_market_t> markets(std::string currency_symbol);

// A call to global() returns the overall information about the cryptomarket
// gm_data_t is:
/*
typedef struct {
    long long int total_market_cap_usd, total_24h_volume_usd;
    float bitcoin_percentage_of_market_cap;
    int active_currencies, active_assets, active_markets;
} gm_data_t;
*/
gm_data_t global();
```

## Exchange

OpenAT contains also a client for https://shapeshift.com/. The [`shapeshift.hpp`](https://github.com/galeone/openat/blob/master/include/at/shapeshift.hpp) file is documented (and it's nothing more than the shapeshift API documentation), you can use it as documentation.


## Build

Clone the repository and make sure to clone the submodules too:

```
git clone --recursive https://github.com/galeone/openat
```

### Building on (Arch)linux

```
# Install the required dependencies

sudo pacman -S spdlog nlohmann-json gumbo-parser sqlite
# install gumbo query to your system
cd libs/gumbo/query/build
cmake ..
make
sudo make install
# if there are problem with the static library, remove the last line
# `libfind_process(Gumbo)`
# from libs/gumbo/query/cmake/FindGumbo.cmake
cd -
# Install curlpp, or with yay -S curlpp
# or using the submodule
cd libs/curlpp
mkdir build
cd build
cmake ..
make
sudo make install
cd -
# build openat
mkdir build
cd build
cmake ..
CC=clang CXX=clang++ make
```

### Building on macOS

Install the needed dependencies and remember to link them:
```
brew install gcc
brew install openssl sqlite
brew link sqlite --force
```

For security reasons, HomeBrew doesn't allow to symlink OpenSSL to /usr/local.
You will need to manually tell cmake where to find these libraries. Make sure
to provide cmake with the correct version of the OpenSSL (1.1).

You can now proceed and build `at`:
```
mkdir build && cd build
CC=gcc-7 CXX=g++-7 cmake \
    -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl@1.1/ \
    -DOPENSSL_INCLUDE_DIR=/usr/local/opt/openssl@1.1/include \
    -DOPENSSL_CRYPTO_LIBRARY=/usr/local/opt/openssl@1.1/lib/libcrypto.dylib ..
make
```
## Test

```
cd build
# ID is a test defined by gumbo-query that somehow appears into
# the tests of the current project. Exclue it using cmame -E (exclude) ID
ctest -E ID
```

## Embed it as a submodule using CMake

Copy or clone the project into the `libs` folder, than add to your `CMakeLists.txt`:

```cmake
# Build OpenAT
set(OPENAT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/libs/openat")
set(OPENAT_INCLUDE_DIR "${OPENAT_SOURCE_DIR}/include")
add_subdirectory(${OPENAT_SOURCE_DIR})

# Add the OPENAT_INCLUDE_DIR and the exported variable
# OPENAT_REQUIRED_INCLUDES to the `include_directories` section
include_directories(
# ... other includes
${OPENAT_INCLUDE_DIR}
${OPENAT_REQUIRED_INCLUDES}
)

# Add the `openat` project to the `target_link_libraries`:
target_link_libraries(project_name LINK_PUBLIC
    openat
    # other linked libraries...
)    
```

# Contributing

The best way to contribute to OpenAT is via pull request and issues, here on GitHub.

There are a lot of things to do to improve OpenAT (and [openatd: OpenAT Daemon](https://github.com/galeone/openatd/) too!):

1. **Add implementations of the Market interface**: this is the most important part. More market are implemented and more OpenAT can be useful.
With more implementation with can easily write trading bot for arbitrage in the easiest way ever.
2. **Add data sources**: coinmarketcap is a good data source and it works well. But we can try to make OpenAT smarter, collecting any other data that talks about crypto currencies (just think, train a ML model with the stream of collected tweets and news feed... we can do sentiment analysis and many other cool things: a lot of (high quality) data is everything.
3. **Improve the documentation**: at the time of writing, the only documentation is the README and the comments in the header files. We can do better.
4. **Unit test**: test the server response it's something hard (especially when you work with idiotic APIs like the shapeshift ones, where a field change it's type from request to request): we have to create a mock server and test everything.
5. **OMG you're using double and not integers everywhere!**: yes you're right. But since OpenAT basically collects data and send request to API that accepts JSON, using doubles and integer changes nothing (you have to convert the data to a string in every case). But if you want to change OpenAT making it use integer and the information about the number of meaningful digits you're welcome.

Also, if you want to donate instead of contributing with code, feel free do donate ETH at this address: `0xa1d283e77f8308559f62909526ccb2d9444d96fc`


