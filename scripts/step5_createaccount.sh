#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888
# eosnode=https://ore-staging.openrights.exchange:443
# eosnode=https://ore.openrights.exchange

#NOTE: This script assumes that you have the keys for the DAPP_OWNER in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

NAME=${1:-sampleuser11}
ORIGIN=${2:-sampleapp}
DAPP_OWNER=${3:-app.oreid}

$cleos push action createbridge create '["'$DAPP_OWNER'","'$NAME'","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","'$ORIGIN'"]' -p $DAPP_OWNER
