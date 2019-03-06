#!/bin/bash

# eos server node address
# point to the current chain 
eosnode=http://127.0.0.1:8888

#NOTE: This script assumes that you have the keys for createbridge account in your unlocked wallet 

#cleos
cleos="cleos -u $eosnode"

CHAIN_SYMBOL=${1:-EOS}
SYMBOL_PRECISION=${2:-4}
TOKEN_CONTRACT=${3:-eosio}
MINIMUM_RAM=${4:-4096}

# specify the chain symbol and the contract name to call for new account action 
$cleos push action createbridge init '["'$SYMBOL_PRECISION','$CHAIN_SYMBOL'","'$TOKEN_CONTRACT'","'$MINIMUM_RAM'"]' -p createbridge

symbol=$($cleos get table createbridge createbridge token | jq '.rows[].S_SYS')
echo "$symbol set successfully"
