#pragma once

#include <sstream>
#include <string>

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

   std::vector<std::string> split(const std::string &s, char delim) {
        std::stringstream ss(s);
        std::string item;
        std::vector<std::string> elems;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
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

    struct [[eosio::table, eosio::contract("createbridge")]] token {
        symbol S_SYS;
        name   newaccountcontract;
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
};