#!/bin/bash

shopt -s expand_aliases
source ~/.bash_aliases

# This script calls the CREATE action of createbridge to create a new user account
# Arguments: 1. NAME:        name of the new account        
#            2. DAPP:      dapp name
#            3. DAPP_OWNER:  the owner account for the dapp
#            4. CUSTODIAN_ACCOUNT: the whitelisted account for the dapp
#NOTE: This script assumes that you have the keys for the DAPP_OWNER in your unlocked wallet

TYPE=${1:-1}
NAME=${2:-testuser1111}
DAPP=${3:-test.com}
DAPP_OWNER=${4:-eosio}
CUSTODIAN_ACCOUNT=${5:-appcustodian}

PUBLIC_KEY="EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV"

if [ $TYPE -eq 1 ]; then
    # Uncomment the below command if you want the dapp owner to create the new account
    cleos push action createbridge create '["'$DAPP_OWNER'","'$NAME'","'$PUBLIC_KEY'","'$PUBLIC_KEY'","'$DAPP'"]' -p $DAPP_OWNER
fi

if [ $TYPE -eq 2 ]; then
     #Uncomment below if you want to contribute and create the new account at the same time
     cleos transfer $CUSTODIAN_ACCOUNT createbridge "0.1000 EOS" "$PUBLIC_KEY,$NAME" -p $CUSTODIAN_ACCOUNT
     cleos get account $NAME
fi

if [ $TYPE -eq 3 ]; then
    # Custodian account creates the new account
    cleos push action createbridge create '["'$CUSTODIAN_ACCOUNT'","'$NAME'","EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV","EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV","'$DAPP'"]' -p $CUSTODIAN_ACCOUNT
fi





