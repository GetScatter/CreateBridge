#!/bin/bash

# eos server node address
# point to the current chain 
eosnode=http://127.0.0.1:8888

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet.
#NOTE: Also assumes that the accounts mydappuser11, mydappowner1 and contributor1 exists on the chain and their keys in an unlocked wallet
#NOTE: For airdrops, the script assumes that the dapp token contract is deployed under mydapptoken1, "DP" symbol created under it and issued to mydappowner1

#cleos
cleos="cleos -u $eosnode"

CHAIN_SYMBOL=${1:-EOS}
NAME=${2:-mydappuser11}
ORIGIN=${3:-mydapp.org}
TOKEN_CONTRACT=${4:-mydapptoken1}
TOKEN_SYMBOL=${5:-DP}
DAPP_OWNER=${6:-mydappowner1}
MEMO=${7:-contributor1}

# app registration
AIRDROP_JSON='{"contract":"", "tokens":"0 EOS", "limit":"0 EOS"}'
PARAMS_JSON='{"owner":"'$DAPP_OWNER'", "dapp":"'$ORIGIN'", "ram_bytes":"4096", "net":"1.0000 EOS", "cpu":"1.0000 EOS", "airdrop":'$AIRDROP_JSON'}'

$cleos push action createbridge define "$PARAMS_JSON" -p $DAPP_OWNER

# whitelisting
$cleos push action createbridge whitelist '["'$DAPP_OWNER'","'$MEMO'","'$ORIGIN'"]' -p $DAPP_OWNER

# contribute and create account simultaneously
$cleos transfer $MEMO createbridge "10.0000 EOS" "$ORIGIN,100,1,$NAME,EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L,EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L" -p $MEMO

$cleos get account $NAME