#pragma once

namespace balances {

    struct [[eosio::table, eosio::contract("createbridge")]] registry {
        name owner;
        string dapp;
        asset ram; // ram in addition to the minimum required for account creation to put in the new user account created for the dapp
        asset net; // SYS/EOS amount to be staked/spent for net
        asset cpu; // SYS/EOS amount to be staked/spent for cpu
        vector<name> whitelisted_accounts;
        uint64_t primary_key() const {return common::toUUID(dapp);}
    };

    typedef eosio::multi_index<"registry"_n, registry> Registry;

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
