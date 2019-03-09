#pragma once

namespace registry{

    struct airdropdata {
        name        contract;
        asset       tokens;
        asset       limit;

        bool isEmpty(){
            return contract == name("");
        }

        bool validate(){
            if(isEmpty()) return true;
            eosio_assert(tokens.amount > 0, "Airdrop tokens must have an amount greater than 0");
            eosio_assert(limit.amount > 0, "Airdrop limit must have an amount greater than 0");
            return true;
        }
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

