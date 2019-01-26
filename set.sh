#!/bin/bash

eosio-cpp --abigen --contract=createbridge createbridge.cpp -o ~/contracts/createbridge.wasm
cleoskylin set contract oreorebridge ~/contracts/ createbridge.wasm createbridge.abi -p oreorebridge@active
