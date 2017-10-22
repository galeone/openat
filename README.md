# AT
[![Build Status](https://travis-ci.org/galeone/at.svg?branch=master)](https://travis-ci.org/galeone/at)


## Build

Clone the repository and make sure to clone the submodules too:

```
git clone --recursive https://github.com/galeone/at
```

### Building on UNIX

```
mkdir build
cd build
cmake ..
CC=clang CXX=clang++ make
```

### Building on macOS

As of october 2017, the version of clang shipped with XCode 9 does not fully
support C++17. You will need to install gcc from HomeBrew; it will be available
as `gcc-7` and `g++-7`. Note that the gcc included in XCode is just a gcc 
frontend with an LLVM (clang) backend; it will not be able to build `at`.
```
brew install gcc
```

Install the needed dependencies and remember to link them:
```
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

## Configuration

Create the file `config.json` and put it in the same folder of the `at` executable.
`config.json` should look like:

```json
{
    "monitor": {
        "currencies": ["btc", "xrp", "eth", "ltc", "xmr"],
        "pairs": [
            ["btc", "usd"],
            ["xrp", "usd"],
            ["eth", "btc"],
            ["ltc", "btc"],
            ["xmr", "usd"]
        ]
    },
    "markets": {
        "kraken": {
            "apiKey": "",
            "apiSecret": "",
            "otp": ""
        }
    }
}
```
