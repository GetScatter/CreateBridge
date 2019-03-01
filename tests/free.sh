#!/bin/bash

# eos server node address
# point to the current chain 
eosnode=http://127.0.0.1:8888

#NOTE: This script assumes you have the keys for createbridge in your unlocked wallet
#NOTE: This script also assumes that the accounts mydappowner1 and contributor1 exists on the chain and their keys in an unlocked wallet

#cleos
cleos="cleos -u $eosnode"

NAME=${1:-mydappuser12}
ORIGIN=${2:-free}
MEMO=${3:-asdf}
CONTRIBUTOR=${4:-contributor1}
CREATOR=${5:-mydappowner1}

$cleos push action createbridge define '["","free","2.0000 SYS","1.0000 SYS","1.0000 SYS","","0.0000 SYS","0.0000 SYS"]' -p createbridge@active

$cleos transfer $CONTRIBUTOR createbridge "10.0000 SYS" "free" -p $CONTRIBUTOR

$cleos push action createbridge create '["'$MEMO'", "'$NAME'", "EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L", "'$ORIGIN'"]' -p $CREATOR

$cleos get account $NAME