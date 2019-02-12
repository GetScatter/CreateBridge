#pragma once

namespace balances {
    
    struct [[eosio::table, eosio::contract("createbridge")]] contributors {
        name contributor;
        asset balance;
        int ram;   // percentage of ram cost the contributor wants to fund
        // add below flag later
        //bool useForRemainingBalance;
    };

    struct [[eosio::table, eosio::contract("createbridge")]] balances {
        uint64_t memo;
        vector<contributors> contributors;
        asset balance;
        string origin;
        uint64_t timestamp;

        uint64_t primary_key() const { return memo; }
    };

    typedef eosio::multi_index<"balances"_n, balances> Balances;
}
