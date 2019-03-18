#pragma once

namespace registry {

    struct [[eosio::table, eosio::contract("createbridge")]] registry {
        uint64_t        id;
        name            owner;
        string          dapp;
        uint64_t        ram_bytes;
        asset           net;
        asset           cpu;
        vector<name>    custodians;

        uint64_t primary_key() const {return common::toUUID(dapp); }
    };

    typedef eosio::multi_index<"registry"_n, registry> Registry;
}

