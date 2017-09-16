# AT


## Build

```
mkdir build
cd build
cmake ..
CC=clang CXX=clang++ make
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
