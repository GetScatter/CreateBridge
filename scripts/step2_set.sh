# eos server node address
eosnode=http://127.0.0.1:8888

# This script calls the SET action of createbridge and specifies the following
# Arguments: 1. CHAIN_SYMBOL:           core symbol of the chain on which createbridge is deployed
#            2. SYMBOL_PRECISION:       precision of core symbol
#            3. NEWACCOUNT_CONTRACT:    name of the contract to call for "newaccount" action. Generally eosio 

#NOTE: This script assumes that you have the keys for createbridge account in your unlocked wallet 

#cleos
cleos="cleos -u $eosnode"

CHAIN_SYMBOL=${1:-EOS}
SYMBOL_PRECISION=${2:-4}
NEWACCOUNT_CONTRACT=${3:-eosio}


# specify the chain symbol and the contract name to call for new account action 
$cleos push action createbridge set '["'$SYMBOL_PRECISION','$CHAIN_SYMBOL'","'$NEWACCOUNT_CONTRACT'"]' -p createbridge

