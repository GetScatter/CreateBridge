#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888

# This script calls the DEFINE action of createbridge to register an dapp with createbridge
# Arguments: 1. DAPP_OWNER: account name to be registered as the owner of the dapp
#            2. ORIGIN:                     a unique string to register the dapp
#            3. RAM:                        specify the amount of RAM to put in the new user accounts created for the dapp
#            4. NET:                        amount of core tokens to be staked for net bandwidth for the new user accounts
#            5. CPU:                        amount of core tokens to be staked for cpu for the new user accounts
#            6. AIRDROP_TOKEN_CONTRACT:     the contract name under which the dapp token is deployed 
#            7. AIRDROP_TOKEN_TOTAL:        the total number of dapp tokens to be airdropped
#            8. AIRDROP_TOKEN_LIMIT:        the number of dapp tokens to be airdropped to every new account created

#NOTE: This script assumes that you have the keys for the DAPP_OWNER in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

DAPP_OWNER=${1:-mydappowner1}
ORIGIN=${2:-mydapp.org}
RAM=${3:-'2.000 SYS'}
NET=${4:-'1.000 SYS'}
CPU=${5:-'1.000 SYS'}
AIRDROP_TOKEN_CONTRACT=${6:-mydapptoken1}
AIRDROP_TOKEN_TOTAL=${7:-1000.0000 DP}
AIRDROP_TOKEN_LIMIT=${8:-10.0000 DP}

# app registration
$cleos push action createbridge define '["'$DAPP_OWNER'","'$ORIGIN'",'$RAM','$NET','$CPU',"'$AIRDROP_TOKEN_CONTRACT'","'$AIRDROP_TOKEN_TOTAL'","'$AIRDROP_TOKEN_LIMIT'"]' -p $DAPP_OWNER
