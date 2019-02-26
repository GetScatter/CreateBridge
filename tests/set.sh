# eos server node address
eosnode=http://127.0.0.1:8888
# eosnode=https://ore-staging.openrights.exchange:443
# eosnode=https://ore.openrights.exchange

#NOTE: This script assumes that you have the keys for createbridge account in your unlocked wallet 

#cleos
cleos="cleos -u $eosnode"

CHAIN_SYMBOL=${1:-SYS}

# specify the chain symbol and the contract name to call for new account action 
$cleos push action createbridge set '["4, '$CHAIN_SYMBOL'","eosio"]' -p createbridge

symbol=$($cleos get table createbridge createbridge token | jq '.rows[].S_SYS')
echo "$symbol set successfully"
