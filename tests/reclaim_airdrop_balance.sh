#!/bin/bash

# eos server node address
# point to the current chain 
eosnode=http://127.0.0.1:8888

#NOTE: This script assumes that you have the keys for the dapp_owner in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

ORIGIN=${1:-mydapp.org}
DAPP_OWNER=${2:-mydappowner1}
TOKEN=${3:-DP}

# reclaim dapp tokens
$cleos push action createbridge reclaim '["'$DAPP_OWNER'","'$ORIGIN'","'$TOKEN'"]' -p $DAPP_OWNER