#!/bin/bash

# Sometimes editing bash scripts on windows causes issues with interpretation of line endings (LF and CRLF discrepancies)
# dos2unix fixes the errors, and this script simply runs dos2unix on all scripts in the repo.

# Preparation:
# - sudo apt-get install dos2unix
# - dos2unix fix_shell_scripts.sh

cd scripts
dos2unix 0_initial_setup.sh 1_compile.sh 2_perms.sh 3_init.sh 4_define.sh 5_contribute.sh 6_createaccount.sh 7_reclaim.sh 8_reclaim_airdrop.sh log_tables.sh
cd ../tests
dos2unix autocreation.sh free.sh init.sh origin.sh reclaim.sh reclaim_airdrop_balance.sh
cd ../