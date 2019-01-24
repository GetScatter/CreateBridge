#!/bin/bash

NAME=${1:-scatter11tst}
KEY=${3:-EOS7w5aJCv5B7y3a6f4WCwPSvs6TpCAoRGnGpiLMsSWbmxaZdKigd}

#cleos transfer eosio createbridge "10.0000 SYS" "$MEMO" -p eosio@active
#cleos transfer eosio createbridge "10.0000 SYS" "$ORIGIN" -p eosio@active
cleos push action createbridge create '["'$KEY'", "'$NAME'", "'$KEY'", "'$KEY'"]' -p createbridge@active
