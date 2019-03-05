#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

using namespace eosio;
using std::string;
using std::vector;

namespace common {
    static const symbol S_RAM = symbol("RAMCORE", 4);
    static const name createbridge = name("createbridge");

    inline static uint64_t toUUID(string username){
        return std::hash<string>{}(username);
    }

    std::vector<std::string> split(const string& str, const string& delim) {
        vector<string> parts;
        if(str.size() == 0) return parts;
        size_t prev = 0, pos = 0;
        do
        {
            pos = str.find(delim, prev);
            if (pos == string::npos) pos = str.length();
            string token = str.substr(prev, pos-prev);
            if (!token.empty()) parts.push_back(token);
            prev = pos + delim.length();
        }
        while (pos < str.length() && prev < str.length());
        return parts;
    }

    uint64_t generate_random(uint64_t seed, uint64_t val){
        const uint64_t a = 1103515245;
        const uint64_t c = 12345;

        // using LCG alogrithm : https://en.wikipedia.org/wiki/Linear_congruential_generator
        // used by standard c++ rand function : http://pubs.opengroup.org/onlinepubs/009695399/functions/rand.html

        uint64_t seed2 = (uint32_t)((a * seed + c) % 0x7fffffff);
        uint64_t value = ((uint64_t)seed2 * val) >> 31;

        return value;
    }

    /**********************************************/
    /***                                        ***/
    /***            Chain constants             ***/
    /***                                        ***/
    /**********************************************/

    struct [[eosio::table, eosio::contract("createbridge")]] token {
        symbol S_SYS;
        name   newaccountcontract;
        uint64_t min_ram;
        uint64_t primary_key()const { return S_SYS.raw(); }
    };

    typedef eosio::multi_index<"token"_n, token> Token;

    /***
     * Returns the symbol of the core token of the chain or the token used to pay for new account creation
     * @return
     */
    symbol getCoreSymbol(){
        Token token(createbridge, createbridge.value);
        return token.begin()->S_SYS;
    }

    /***
     * Returns the contract name for new account action
     */
    name getNewAccountContract(){
        Token token(createbridge, createbridge.value);
        return token.begin()->newaccountcontract;
    }

    /**
     * Returns the minimum bytes of RAM for new account creation
     */
    uint64_t getMinimumRAM(){
        Token token(createbridge, createbridge.value);
        return token.begin()->min_ram;
    }

    /**********************************************/
    /***                                        ***/
    /***            RAM calculations            ***/
    /***                                        ***/
    /**********************************************/

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

    /***
     * Returns the price of ram for given bytes
     */

    asset getRamCost(uint64_t ram_bytes){
       RamInfo ramInfo(name("eosio"), name("eosio").value);
       auto ramData = ramInfo.find(S_RAM.raw());
       symbol coreSymbol = getCoreSymbol();
       eosio_assert(ramData != ramInfo.end(), "Could not get RAM info");

       uint64_t base = ramData->base.balance.amount;
       print("\nbase\n");
       print(std::to_string(base));
       uint64_t quote = ramData->quote.balance.amount;
       return asset((((double)quote / base))*ram_bytes, coreSymbol);
    }
};