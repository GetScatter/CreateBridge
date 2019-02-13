#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888

# This script is used to contribute towards account creation for a dapp.
# Arguments: 1. ORIGIN:         dapp name          
#            2. TOTAL_RAM:      % of ram you are willing to contribute per new user account for the dapp
#            3. TOTAL_ACCOUNTS: The max number of accounts you want to be created by your contribution    
#            4. CONTRIBUTOR:    your account name 

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

ORIGIN=${1:-mydapp.org}
AMOUNT=${2:-"10.0000 EOS"}
TOTAL_RAM=${2:-50}
TOTAL_ACCOUNTS=${3:-100}
CONTRIBUTOR=${4:-contributor1}

# contribute
$cleos transfer $CONTRIBUTOR createbridge "$AMOUNT" "$ORIGIN,$TOTAL_RAM,$TOTAL_ACCOUNTS" -p $CONTRIBUTOR
