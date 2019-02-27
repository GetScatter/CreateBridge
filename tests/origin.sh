#!/bin/bash

# eos server node address
# point to the current chain 
eosnode=http://127.0.0.1:8888

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet.
#NOTE: Also assumes that the accounts mydappuser11, mydappowner1 and contributor1 exists on the chain and their keys in an unlocked wallet
#NOTE: For airdrops, the script assumes that the dapp token contract is deployed under mydapptoken1, "DP" symbol created under it and issued to mydappowner1

#cleos
cleos="cleos -u $eosnode"

CHAIN_SYMBOL=${1:-SYS}
NAME=${2:-mydappuser11}
ORIGIN=${3:-mydapp.org}
TOKEN_CONTRACT=${4:-mydapptoken1}
TOKEN_SYMBOL=${5:-DP}
DAPP_OWNER=${6:-mydappowner1}
MEMO=${7:-contributor1}

# app registration
$cleos push action createbridge define '["'$DAPP_OWNER'","'$ORIGIN'","2.0000 '$CHAIN_SYMBOL'","1.0000 '$CHAIN_SYMBOL'","1.0000 '$CHAIN_SYMBOL'","'$TOKEN_CONTRACT'","1000.0000 '$TOKEN_SYMBOL'","10.0000 '$TOKEN_SYMBOL'"]' -p $DAPP_OWNER

# whitelisting
$cleos push action createbridge whitelist '["'$DAPP_OWNER'","'$MEMO'","'$ORIGIN'"]' -p $DAPP_OWNER

# transfer airdrop amount
$cleos push action $TOKEN_CONTRACT transfer '["'$DAPP_OWNER'","createbridge","1000.0000 '$TOKEN_SYMBOL'","transfer"]' -p $DAPP_OWNER

# contributions
$cleos transfer $DAPP_OWNER createbridge "100.0000 $CHAIN_SYMBOL" "$ORIGIN,50,100" -p $DAPP_OWNER
$cleos transfer $MEMO createbridge "100.0000 $CHAIN_SYMBOL" "$ORIGIN,50,100" -p $MEMO

# create account
$cleos push action createbridge create '["'$MEMO'","'$NAME'","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","'$ORIGIN'"]' -p $MEMO

# get newly created account
$cleos get account $NAME
