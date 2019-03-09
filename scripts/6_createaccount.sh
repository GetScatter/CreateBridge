#!/bin/bash

shopt -s expand_aliases
source ~/.bash_aliases

# This script calls the CREATE action of createbridge to create a new user account
# Arguments: 1. NAME:        name of the new account        
#            2. ORIGIN:      dapp name
#            3. DAPP_OWNER:  the owner account for the dapp
#            4. CUSTODIAN_ACCOUNT: the whitelisted account for the dapp
#NOTE: This script assumes that you have the keys for the DAPP_OWNER in your unlocked wallet

NAME=${1:-testuser1111}
ORIGIN=${2:-test.com}
DAPP_OWNER=${3:-eosio}
CUSTODIAN_ACCOUNT=${4:-appcustodian}

# Custodian account creates the new account
cleos push action createbridge create '["'$CUSTODIAN_ACCOUNT'","'$NAME'","EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV","EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV","'$ORIGIN'"]' -p $CUSTODIAN_ACCOUNT

# Uncomment the below command if you want the dapp owner to create the new account
# cleos push action createbridge create '["'$DAPP_OWNER'","'$NAME'","EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV","EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV","'$ORIGIN'"]' -p $DAPP_OWNER

# Uncomment below if you want to contribute and create the new account at the same time
# cleos transfer $CUSTODIAN_ACCOUNT createbridge "10.0000 EOS" "$ORIGIN,100,1,$NAME,EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV,EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV" -p $CUSTODIAN_ACCOUNT