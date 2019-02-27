#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888

# This script calls the RECLAIM action of createbridge to get back the remaining token balance for a contributor
# Arguments: 1. ORIGIN:         dapp name
#            2. CONTRIBUTOR:    the name of the contributor  
#            3. CHAIN_SYMBOL:   core symbol of the chain   

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

CHAIN_SYMBOL=${1:-SYS}
ORIGIN=${2:-mydapp.org}
CONTRIBUTOR=${3:-contributor1}

# reclaim
$cleos push action createbridge reclaim '["'$CONTRIBUTOR'","'$ORIGIN'","'$CHAIN_SYMBOL'"]' -p $CONTRIBUTOR