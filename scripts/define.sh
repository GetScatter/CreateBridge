#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888
# eosnode=https://ore-staging.openrights.exchange:443
# eosnode=https://ore.openrights.exchange

#NOTE: This script assumes that you have the keys for the DAPP_OWNER in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

ORIGIN=${2:-sampleapp}
TOTAL_RAM=${3:-50}
TOTAL_ACCOUNTS=${4:-100}
DAPP_OWNER=${5:-app.oreid}

# app registration
$cleos push action createbridge define '["'$DAPP_OWNER'","'$ORIGIN'","2.0000 SYS","1.0000 SYS","1.0000 SYS","","0.0000 SYS","0.0000 SYS"]' -p $DAPP_OWNER

