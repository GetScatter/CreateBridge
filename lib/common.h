#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

using namespace eosio;
using std::string;
using std::vector;

namespace common {
    static const symbol S_SYS = symbol("EOS", 4);
    static const symbol S_RAM = symbol("RAMCORE", 4);
    static asset CPU(0'4000, S_SYS);
    static asset NET(0'1000, S_SYS);

    inline static uint64_t toUUID(string username){
        return std::hash<string>{}(username);
    }



    struct rammarket {
        asset    supply;

        struct connector {
            asset balance;
            double weight = .5;
        };

        connector base;
        connector quote;

        uint64_t primary_key()const { return supply.symbol.raw(); }
    };


    typedef eosio::multi_index<"rammarket"_n, rammarket> RamInfo;
};