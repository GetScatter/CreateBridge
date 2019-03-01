#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888

# This script calls the RECLAIM action of createbridge to get back the remaining airdrop token balance for a dapp
# Arguments: 1. ORIGIN:         dapp name
#            2. DAPP_OWNER:     account name registered as the owner of the dapp
#            3. TOKEN:          symbol of the dapp token

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

ORIGIN=${1:-mydapp.org}
DAPP_OWNER=${2:-mydappowner1}
TOKEN=${3:-DP}

# reclaim dapp tokens
$cleos push action createbridge reclaim '["'$DAPP_OWNER'","'$ORIGIN'","'$TOKEN'"]' -p $DAPP_OWNER