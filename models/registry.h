#pragma once

namespace registry{

    struct [[eosio::table, eosio::contract("createbridge")]] registry {
        name owner;
        string dapp;
        asset ram;           
        asset net;           
        asset cpu;           
        name airdropcontract;
        asset airdroptokens;
        asset airdroplimit; 
        vector<name> whitelisted_accounts;
        uint64_t primary_key() const {return common::toUUID(dapp);}
    };

    typedef eosio::multi_index<"registry"_n, registry> Registry;
}

