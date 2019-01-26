#!/bin/bash

PKEY=EOS8FRotV1qxh5245nzGYLAVp4S211MVPuuZw12tgmRHguua4fV81

cleoskylin set account permission oreorebridge active \
'{"threshold": 1,"keys": [{"key": "'$PKEY'","weight": 1}],"accounts": [{"permission":{"actor":"oreorebridge","permission":"eosio.code"},"weight":1}]}' owner
