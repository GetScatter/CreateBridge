#!/bin/bash

shopt -s expand_aliases
source ~/.bash_aliases

# Compiles the contract into ABI and WASM formats
# Arguments:
# - 1. CONTRACT_NAME: The name of the contract for table/struct linking
# - 2. OUTPUT_DIR: The output directory for the ABI and WASM files
# - 3. CONTRACT_ACCOUNT: The name of the contract account to set the contract to

CONTRACT_NAME=${1:-createbridge}
OUTPUT_DIR=${2:-~/contracts}
CONTRACT_ACCOUNT=${3:-createbridge}

echo ${CONTRACT_NAME}
echo ${OUTPUT_DIR}
echo ${CONTRACT_ACCOUNT}

eosio-cpp --abigen --contract=${CONTRACT_NAME} ../${CONTRACT_NAME}.cpp -o ${OUTPUT_DIR}/${CONTRACT_NAME}.wasm
eosio-wasm2wast ../${CONTRACT_NAME}.wasm -o ../${CONTRACT_NAME}.wast

# makes contract eosjs1 compatible
sed -i 's/1.1/1.0/g' ${OUTPUT_DIR}/${CONTRACT_NAME}.abi

cleos set contract ${CONTRACT_ACCOUNT} ${OUTPUT_DIR}/ ${CONTRACT_NAME}.wasm ${CONTRACT_NAME}.abi -p ${CONTRACT_ACCOUNT}@active
