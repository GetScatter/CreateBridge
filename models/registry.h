#pragma once

namespace registry{
    
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
}

