#!/bin/bash

# This script adds the code level permission to the active permission of createbridge 
# Arguments: 1. PKEY: active key of createbridge

PKEY=${1:-EOS4xJvy2tYU21reKbbq4RPLxgzxNmrLtidVWpio5Ggwisfkgzg2L}

cleos set account permission createbridge active \
'{"threshold": 1,"keys": [{"key": "'$PKEY'","weight": 1}],"accounts": [{"permission":{"actor":"createbridge","permission":"eosio.code"},"weight":1}]}' owner
