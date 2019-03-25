#!/bin/bash

shopt -s expand_aliases
source ~/.bash_aliases

echo "-------------------"
echo "Balances"
cleos get table createbridge createbridge balances
echo ""
echo ""

echo "Contributors"
cleos get table createbridge createbridge contributors
echo ""
echo ""

echo "Registry"
cleos get table createbridge createbridge registry
echo ""
echo ""

echo "Token"
cleos get table createbridge createbridge token
echo ""
echo ""
echo "-------------------"