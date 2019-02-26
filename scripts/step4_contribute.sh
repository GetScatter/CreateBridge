#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888
# eosnode=https://ore-staging.openrights.exchange:443
# eosnode=https://ore.openrights.exchange

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

ORIGIN=${2:-sampleapp}
TOTAL_RAM=${3:-50}
TOTAL_ACCOUNTS=${4:-100}
CONTRIBUTOR=${5:-app.oreid}

# contribute
$cleos transfer $CONTRIBUTOR createbridge "100.0000 SYS" "$ORIGIN,$TOTAL_RAM,$TOTAL_ACCOUNTS" -p $CONTRIBUTOR
