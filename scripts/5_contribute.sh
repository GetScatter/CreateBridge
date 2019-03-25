#!/bin/bash

shopt -s expand_aliases
source ~/.bash_aliases

# This script is used to contribute towards account creation for a dapp.
# Arguments: 1. ORIGIN:         dapp name          
#            2. TOTAL_RAM:      % of ram you are willing to contribute per new user account for the dapp
#            3. TOTAL_ACCOUNTS: The max number of accounts you want to be created by your contribution    
#            4. CONTRIBUTOR:    your account name 

#NOTE: This script assumes that you have the keys for the contributor in your unlocked wallet

ORIGIN=${1:-test.com}
AMOUNT=${2:-"10.0000 EOS"}
TOTAL_RAM=${2:-50}
TOTAL_ACCOUNTS=${3:-100}
CONTRIBUTOR=${4:-contributor1}
CUSTODIAN_ACCOUNT=${5:-appcustodian}

# contribute
cleos transfer $CONTRIBUTOR createbridge "$AMOUNT" "$ORIGIN,$TOTAL_RAM,$TOTAL_ACCOUNTS" -p $CONTRIBUTOR
cleos transfer $CUSTODIAN_ACCOUNT createbridge "$AMOUNT" "$ORIGIN,$TOTAL_RAM,$TOTAL_ACCOUNTS" -p $CUSTODIAN_ACCOUNT
