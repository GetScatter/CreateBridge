#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888
# eosnode=https://ore-staging.openrights.exchange:443
# eosnode=https://ore.openrights.exchange

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

ORIGIN=${2:-sampleapp}
CONTRIBUTOR=${3:-app.oreid}

# reclaim
$cleos push action createbridge reclaim '["'$CONTRIBUTOR'","'$ORIGIN'","SYS"]' -p $CONTRIBUTOR