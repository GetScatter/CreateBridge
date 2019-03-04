#!/bin/bash

shopt -s expand_aliases
source ~/.bash_aliases

# This script calls the RECLAIM action of createbridge to get back the remaining token balance for a contributor
# Arguments: 1. ORIGIN:         dapp name
#            2. CONTRIBUTOR:    the name of the contributor  
#            3. CHAIN_SYMBOL:   core symbol of the chain   

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

CHAIN_SYMBOL=${1:-EOS}
ORIGIN=${2:-test.com}
CONTRIBUTOR=${3:-contributor1}

# reclaim
cleos push action createbridge reclaim '["'$CONTRIBUTOR'","'$ORIGIN'","'$CHAIN_SYMBOL'"]' -p $CONTRIBUTOR