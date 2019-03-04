#!/bin/bash

shopt -s expand_aliases
source ~/.bash_aliases

# This script calls the RECLAIM action of createbridge to get back the remaining airdrop token balance for a dapp
# Arguments: 1. ORIGIN:         dapp name
#            2. DAPP_OWNER:     account name registered as the owner of the dapp
#            3. TOKEN:          symbol of the dapp token

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

ORIGIN=${1:-test.com}
DAPP_OWNER=${2:-eosio}
TOKEN=${3:-EX}

# reclaim dapp tokens
cleos push action createbridge reclaim '["'$DAPP_OWNER'","'$ORIGIN'","'$TOKEN'"]' -p $DAPP_OWNER