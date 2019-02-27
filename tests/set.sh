#!/bin/bash

# eos server node address
# point to the current chain 
eosnode=http://127.0.0.1:8888

#NOTE: This script assumes that you have the keys for createbridge account in your unlocked wallet 

#cleos
cleos="cleos -u $eosnode"

CHAIN_SYMBOL=${1:-SYS}
SYMBOL_PRECISION=${2:-4}
TOKEN_CONTRACT=${3:-eosio}

# specify the chain symbol and the contract name to call for new account action 
$cleos push action createbridge set '["'$SYMBOL_PRECISION', '$CHAIN_SYMBOL'","'$TOKEN_CONTRACT'"]' -p createbridge

symbol=$($cleos get table createbridge createbridge token | jq '.rows[].S_SYS')
echo "$symbol set successfully"
