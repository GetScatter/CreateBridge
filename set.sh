#!/bin/bash

eosio-cpp --abigen --contract=createbridge createbridge.cpp -o ~/contracts/createbridge.wasm
cleos set contract createbridge ~/contracts/ createbridge.wasm createbridge.abi -p createbridge@active
