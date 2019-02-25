#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888
# eosnode=https://ore-staging.openrights.exchange:443
# eosnode=https://ore.openrights.exchange

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

NAME=${1:-eporeiduser1}
ORIGIN=${2:-free}
MEMO=${3:-asdf}
CONTRIBUTOR=${4:-eosio}
CREATOR=${5:-everipediaiq}

$cleos push action createbridge define '["","free","2.0000 SYS","1.0000 SYS","1.0000 SYS","","0.0000 SYS","0.0000 SYS"]' -p createbridge@active

$cleos transfer eosio createbridge "10.0000 SYS" "free" -p $CONTRIBUTOR

$cleos push action createbridge create '["'$MEMO'", "'$NAME'", "EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L", "'$ORIGIN'"]' -p $CREATOR

$cleos get account $NAME