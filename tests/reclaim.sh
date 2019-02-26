#!/bin/bash

# eos server node address
# point to the current chain 
eosnode=http://127.0.0.1:8888

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

ORIGIN=${2:-mydapp.org}
CONTRIBUTOR=${3:-contirbutor1}
CHAIN_SYMBOL=${4:-SYS}

# contributions
$cleos transfer $CONTRIBUTOR createbridge "5.0000 $CHAIN_SYMBOL" "$ORIGIN,50,100" -p $CONTRIBUTOR

# reclaim
$cleos push action createbridge reclaim '["'$CONTRIBUTOR'","'$ORIGIN'","'$CHAIN_SYMBOL'"]' -p $CONTRIBUTOR