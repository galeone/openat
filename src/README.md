## Source code organization

`at` folder contains the `openat` library source code.

Files in the current directory, instead, use the `openat` library to create a configurable crypto-currency monitor and automatic trader.

## Auto Trader: configuration

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

<!--
## Auto Trader: strategies
TODO
-->
