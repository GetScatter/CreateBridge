#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888
# eosnode=https://ore-staging.openrights.exchange:443
# eosnode=https://ore.openrights.exchange

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

NAME=${1:-eporeiduser2}
ORIGIN=${2:-everipedia.org}
DAPP_OWNER=${3:-everipediaiq}
MEMO=${3:-app.oreid}

# app registration
$cleos push action createbridge define '["'$DAPP_OWNER'","'$ORIGIN'","2.0000 SYS","1.0000 SYS","1.0000 SYS","everipediaiq","1000.0000 IQ","10.0000 IQ"]' -p $DAPP_OWNER

# whitelisting
$cleos push action createbridge whitelist '["'$DAPP_OWNER'","'$MEMO'","'$ORIGIN'"]' -p $DAPP_OWNER

# transfer airdrop amount
$cleos push action everipediaiq transfer '["'$DAPP_OWNER'","createbridge","1000.0000 IQ","transfer"]' -p $DAPP_OWNER

# contributions
$cleos transfer $DAPP_OWNER createbridge "100.0000 SYS" "$ORIGIN,50,100" -p $DAPP_OWNER
$cleos transfer $MEMO createbridge "100.0000 SYS" "$ORIGIN,50,100" -p $MEMO

# create account
$cleos push action createbridge create '["'$MEMO'","'$NAME'","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","'$ORIGIN'"]' -p $MEMO

# get newly created account
$cleos get account $NAME
