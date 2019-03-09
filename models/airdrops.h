#pragma once

#include "../lib/common.h"

using namespace common;

namespace airdrops {
    struct [[eosio::table, eosio::contract("createbridge")]] airdrop {
        uint64_t        key;
        name            contract;
        asset           per_account;
        asset           balance;

        uint64_t primary_key() const {return key;}

        static uint64_t keyFrom(name contract, asset quantity){
            return toUUID(contract.to_string() + quantity.symbol.code().to_string());
        }
    };

    typedef eosio::multi_index<"airdrops"_n, airdrop> Airdrops;
}

