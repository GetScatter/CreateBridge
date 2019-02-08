#pragma once

namespace balances {
    struct [[eosio::table, eosio::contract("createbridge")]] dappregistry {
        name owner;
        string dapp;
        asset ram; // ram in addition to the minimum required for account creation to put in the new user account created for the dapp
        asset net; // SYS/EOS amount to be staked/spent for net
        asset cpu; // SYS/EOS amount to be staked/spent for cpu
        bool buy;  // specify whether to stake or buy the net and cpu for the new user account for the dapp
        vector<name> whitelisted_accounts;
        uint64_t primary_key() const {return common::toUUID(dapp);}
    };

    typedef eosio::multi_index<"dappregistry"_n, dappregistry> Dappregistry;

    struct [[eosio::table, eosio::contract("createbridge")]] balances {
        uint64_t memo;
        asset balance;
        string origin;
        uint64_t timestamp;

        uint64_t primary_key() const { return memo; }
    };

    typedef eosio::multi_index<"balances"_n, balances> Balances;
}
