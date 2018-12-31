#pragma once

namespace balances {

    struct [[eosio::table, eosio::contract("createbridge")]] balances {
        uint64_t memo;
        asset balance;
        string origin;
        uint64_t timestamp;

        uint64_t primary_key() const { return memo; }
    };

    typedef eosio::multi_index<"balances"_n, balances> Balances;
}
