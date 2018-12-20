#!/bin/bash

NAME=${1:-scatter11tst}
ORIGIN=${2:-everipedia.org}
MEMO=${3:-asdf}

#cleos transfer eosio createbridge "10.0000 SYS" "free" -p eosio@active
cleos push action createbridge create '["'$MEMO'", "'$NAME'", "EOS7w5aJCv5B7y3a6f4WCwPSvs6TpCAoRGnGpiLMsSWbmxaZdKigd", "'$ORIGIN'"]' -p createbridge@active
