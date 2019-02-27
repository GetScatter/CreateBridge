#!/bin/bash

# eos server node address
eosnode=http://127.0.0.1:8888

# This script calls the DEFINE action of createbridge to register an dapp with createbridge
# Arguments: 1. DAPP_OWNER: account name to be registered as the owner of the dapp
#            2. ORIGIN:                     a unique string to register the dapp
#            3. CORE_SYMBOL:                core symbol of the chain 
#            4. RAM:                        specify the amount of RAM to put in the new user accounts created for the dapp
#            5. NET:                        amount of core tokens to be staked for net bandwidth for the new user accounts
#            6. CPU:                        amount of core tokens to be staked for cpu for the new user accounts
#            7. AIRDROP_TOKEN_CONTRACT:     the contract name under which the dapp token is deployed 
#            8. AIRDROP_SYMBOL:             symbol of dapp tokens
#            9. AIRDROP_TOKEN_TOTAL:        the total number of dapp tokens to be airdropped
#            10.AIRDROP_TOKEN_LIMIT:        the number of dapp tokens to be airdropped to every new account created
#            11.WHITELISTED_ACCOUNT:        the account to whitelist to create new user accounts for the dapp on behalf of the owner

#NOTE: This script assumes that you have the keys for the DAPP_OWNER in your unlocked wallet

#cleos
cleos="cleos -u $eosnode"

DAPP_OWNER=${1:-mydappowner1}
ORIGIN=${2:-mydapp.org}
CORE_SYMBOL=${3:-EOS}
RAM=${4:-2.000}
NET=${5:-1.000}
CPU=${6:-1.000}
AIRDROP_TOKEN_CONTRACT=${7:-mydapptoken1}
AIRDROP_SYMBOL=${8:-'DP'}
AIRDROP_TOKEN_TOTAL=${9:-1000.0000}
AIRDROP_TOKEN_LIMIT=${10:-10.0000}
WHITELISTED_ACCOUNT=${11:-whitelist111}

# app registration
$cleos push action createbridge define '["'$DAPP_OWNER'","'$ORIGIN'","'$RAM' '$CORE_SYMBOL'","'$NET' '$CORE_SYMBOL'","'$CPU' '$CORE_SYMBOL'","'$AIRDROP_TOKEN_CONTRACT'","'$AIRDROP_TOKEN_TOTAL' '$AIRDROP_SYMBOL'","'$AIRDROP_TOKEN_LIMIT' '$AIRDROP_SYMBOL'"]' -p $DAPP_OWNER

# whitelist other accounts
$cleos push action createbridge whitelist '["'$DAPP_OWNER'","'$WHITELISTED_ACCOUNT'","'$ORIGIN'"]' -p $DAPP_OWNER
