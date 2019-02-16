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

    struct [[eosio::table, eosio::contract("createbridge")]] token {
        symbol S_SYS;
        uint64_t primary_key()const { return S_SYS.raw(); }
    };

    typedef eosio::multi_index<"token"_n, token> Token;
};