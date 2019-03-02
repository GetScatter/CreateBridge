#!/bin/bash

shopt -s expand_aliases
source ~/.bash_aliases

# This script calls the CREATE action of createbridge to create a new user account
# Arguments: 1. NAME:        name of the new account        
#            2. ORIGIN:      dapp name
#            3. DAPP_OWNER:  the owner/whitelisted account for the dapp

#NOTE: This script assumes that you have the keys for the DAPP_OWNER in your unlocked wallet

NAME=${1:-mydappuser11}
ORIGIN=${2:-mydapp.org}
DAPP_OWNER=${3:-mydapptoken1}
WHITELISTED_ACCOUNT=${5:-whitelist111}

# Whitelisted account creates the new account
# cleos push action createbridge create '["'$WHITELISTED_ACCOUNT'","'$NAME'","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","'$ORIGIN'"]' -p $WHITELISTED_ACCOUNT

# Uncomment the below command if you want the dapp owner to create the new account
# cleos push action createbridge create '["'$DAPP_OWNER'","'$NAME'","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","'$ORIGIN'"]' -p $DAPP_OWNER
