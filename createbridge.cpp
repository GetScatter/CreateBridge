#include <eosiolib/eosio.hpp>

#include "lib/common.h"
#include "models/accounts.h"
#include "models/balances.h"

using namespace eosio;
using namespace common;
using namespace accounts;
using namespace balances;

using namespace std;

CONTRACT createbridge : contract {
public:
    using contract::contract;
    createbridge(name receiver, name code,  datastream<const char*> ds):contract(receiver, code, ds) {}



    /**********************************************/
    /***                                        ***/
    /***                Actions                 ***/
    /***                                        ***/
    /**********************************************/

    ACTION create(string& memo, name& account, public_key& key, string& origin){
        require_auth(_self);



        RamInfo ramInfo(name("eosio"), name("eosio").value);
        auto ramData = ramInfo.find(S_RAM.raw());
        eosio_assert(ramData != ramInfo.end(), "Could not get RAM info");

        uint64_t base = ramData->base.balance.amount;
        uint64_t quote = ramData->quote.balance.amount;
        asset baseRamCost = asset((((double)quote / base))*4096, S_EOS);

        key_weight k = key_weight{key, 1};

        authority auth{
            .threshold = 1,
            .keys = {k},
            .accounts = {},
            .waits = {}
        };

        Balances balances(_self, _self.value);

        uint64_t payerId = toUUID(memo);
        auto payer = balances.find(payerId);
        eosio_assert(payer != balances.end(), "Could not find balance");
        asset fromPayer = payer->balance;

        asset ramFromPayer = baseRamCost;
        asset ramFromDapp = asset(0'0000, S_EOS);
        uint64_t originId = toUUID(origin);
        auto dapp = balances.find(originId);

        if(dapp != balances.end()){
            ramFromDapp.amount = baseRamCost.amount/2;
            ramFromPayer -= ramFromDapp;
        }

        asset leftover = fromPayer - ramFromPayer;
        asset cpu(((float)leftover.amount * 0.8), S_EOS);
        asset net(((float)leftover.amount * 0.2), S_EOS);

        newaccount new_account = newaccount{
                .creator = _self,
                .name = account,
                .owner = auth,
                .active = auth
        };

        action(
                permission_level{ _self, "active"_n },
                name("eosio"),
                name("newaccount"),
                new_account
        ).send();

        action(
                permission_level{ _self, "active"_n },
                name("eosio"),
                name("buyram"),
                make_tuple(_self, account, baseRamCost)
        ).send();

        action(
                permission_level{ _self, "active"_n },
                name("eosio"),
                name("delegatebw"),
                make_tuple(_self, account, net, cpu, true)
        ).send();

        subBalance(memo, fromPayer);
        if(ramFromDapp.amount > 0){
            subBalance(origin, ramFromDapp);
        }

    }

    /**********************************************/
    /***                                        ***/
    /***                Balances                ***/
    /***                                        ***/
    /**********************************************/

    void hasBalance(string& memo, const asset& quantity){
        uint64_t id = toUUID(memo);

        Balances balances(_self, _self.value);
        auto iterator = balances.find(id);
        eosio_assert(iterator != balances.end(), "Could not find balance");
        eosio_assert(iterator->balance.amount >= quantity.amount, "Not enough tokens.");
    }

    void addBalance(const asset& quantity, string& memo){
        uint64_t id = toUUID(memo);

        Balances balances(_self, _self.value);
        auto iterator = balances.find(id);
        if(iterator == balances.end()) balances.emplace(_self, [&](auto& row){
            row.memo = id;
            row.balance = quantity;
        });
        else balances.modify(iterator, same_payer, [&](auto& row){
            row.balance += quantity;
        });
    }

    void subBalance(string& memo, const asset& quantity){
        uint64_t id = toUUID(memo);

        Balances balances(_self, _self.value);
        auto iterator = balances.find(id);

        eosio_assert(iterator != balances.end(), "No balance object");
        eosio_assert(iterator->balance.amount >= quantity.amount, "overdrawn balance" );

        if(iterator->balance.amount - quantity.amount <= 0){
            balances.erase(iterator);
        }

        else balances.modify(iterator, same_payer, [&](auto& row){
            row.balance -= quantity;
        });
    }

    /**********************************************/
    /***                                        ***/
    /***               Transfers                ***/
    /***                                        ***/
    /**********************************************/

    void transfer(const name& from, const name& to, const asset& quantity, string& memo){
        if(to != _self) return;
        if(from == name("eosio.stake")) return;
        if(quantity.symbol != S_EOS) return;
        addBalance(quantity, memo);
    }

};

extern "C" {
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    auto self = receiver;

    if( code == self ) switch(action) {
        EOSIO_DISPATCH_HELPER( createbridge, (create) )
    }

    else {
        if(code == name("eosio.token").value && action == name("transfer").value){
            execute_action(name(receiver), name(code), &createbridge::transfer);
        }
    }


}
};