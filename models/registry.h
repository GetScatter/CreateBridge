#pragma once

namespace registry{

    struct airdropdata {
        name        contract;
        asset       tokens;
        asset       limit;
    };

    struct [[eosio::table, eosio::contract("createbridge")]] registry {
        name owner;
        string dapp;
        uint64_t ram_bytes;           
        asset net;           
        asset cpu;
        vector<name> custodians;

        std::optional<airdropdata> airdrop;

        uint64_t primary_key() const {return common::toUUID(dapp);}

        EOSLIB_SERIALIZE(registry, (owner)(dapp)(ram_bytes)(net)(cpu)(custodians)(airdrop))
    };

    typedef eosio::multi_index<"registry"_n, registry> Registry;
}

