#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888

# This script calls the CREATE action of createbridge to create a new user account
# Arguments: 1. NAME:        name of the new account        
#            2. ORIGIN:      dapp name
#            3. DAPP_OWNER:  the owner/whitelisted account for the dapp

#NOTE: This script assumes that you have the keys for the DAPP_OWNER in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

NAME=${1:-mydappuser11}
ORIGIN=${2:-mydapp.org}
DAPP_OWNER=${3:-mydapptoken1}

$cleos push action createbridge create '["'$DAPP_OWNER'","'$NAME'","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L","'$ORIGIN'"]' -p $DAPP_OWNER