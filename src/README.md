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

## Installation

The `openatd` service collects and store markets information about the specified currencies in the configuration file. Data is stored in the SQLite database at `~/.config/openat/db.db3`.

```bash
# Install openatd under /usr/local/bin
sudo cp build/src/openatd /usr/local/bin/openatd
# Move the previous configuration file under ~/.config/openat/
mkdir -p ~/.config/openat/
cp config.json ~/.config/openat/
# Move the service file in the correct location
sudo cp misc/systemd/openatd@.service  /etc/systemd/system/openatd@.service
```

## Start

Enable the daemon on boot and start it with:

```bash
sudo systemctl enable openatd@$USER.service
sudo systemctl start openatd@$USER.service
```

<!--
## Auto Trader: strategies
TODO
-->
