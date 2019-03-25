#!/bin/bash

shopt -s expand_aliases
source ~/.bash_aliases

# Creates the necessary accounts for the contract
# Arguments:
# - 1. PKEY: The public key used for the accounts, defaults to eosio test key

PKEY=${1:-EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV}
STANDARD_TOKEN_CONTRACT_DIR=${2:-"$HOME/eosio.contracts"}

# Creating required testing accounts
cleos create account eosio createbridge "$PKEY" "$PKEY"
cleos create account eosio appcustodian "$PKEY" "$PKEY"
cleos create account eosio contributor1 "$PKEY" "$PKEY"

# Creating `eosio.token` contract
cleos create account eosio eosio.token "$PKEY" "$PKEY"
cleos set contract eosio.token $STANDARD_TOKEN_CONTRACT_DIR/eosio.token --abi eosio.token.abi -p eosio.token
cleos push action eosio.token create '[ "eosio", "1000000000.0000 EOS"]' -p eosio.token
cleos push action eosio.token issue '[ "eosio", "1000000000.0000 EOS", "init" ]' -p eosio
cleos transfer eosio contributor1 "1000.0000 EOS" -p eosio
cleos transfer eosio appcustodian "1000.0000 EOS" -p eosio

# Creating `exampletoken` contract
cleos create account eosio exampletoken "$PKEY" "$PKEY"
cleos set contract exampletoken $STANDARD_TOKEN_CONTRACT_DIR/eosio.token --abi eosio.token.abi -p exampletoken
cleos push action exampletoken create '[ "eosio", "1000000000.0000 EX"]' -p exampletoken
cleos push action exampletoken issue '[ "eosio", "1000000000.0000 EX", "init" ]' -p eosio