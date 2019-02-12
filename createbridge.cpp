#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/action.hpp>

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

    template <typename T>
    void cleanTable(){
        T db(_self, _self.value);
        while(db.begin() != db.end()){
            auto itr = --db.end();
            db.erase(itr);
        }
    }

    ACTION clean(){
        require_auth(_self);
        cleanTable<Balances>();
    }


    /**********************************************/
    /***                                        ***/
    /***                Actions                 ***/
    /***                                        ***/
    /**********************************************/


    // Called to claim an account name as the owner of a dapp. 
    // Only the owner account/whitelisted account will be able to create new user account for the dapp 
    ACTION define(name& owner, string dapp, asset ram, asset net, asset cpu, bool buy) {
        require_auth(owner);
        Registry dapps(_self, _self.value);
        auto iterator = dapps.find(toUUID(dapp));
        if(iterator == dapps.end())dapps.emplace(_self, [&](auto& row){
            row.owner = owner;
            row.dapp = dapp;
            row.ram = ram;
            row.net = net;
            row.cpu = cpu;
        }); else {
            auto msg = "the dapp " + dapp + " is already registered by another account";
            eosio_assert(false, msg.c_str());
        }
    }

    // lets the owner account of the dapp to whitelist other accounts. 
    ACTION whitelist(name owner, name account, string dapp){
        require_auth(owner);

        Registry dapps(_self, _self.value);
        auto iterator = dapps.find(toUUID(dapp));

        if(iterator != dapps.end() && owner == iterator->owner)dapps.modify(iterator, same_payer, [&](auto& row){
            row.whitelisted_accounts.push_back(account);
        }); else {
            auto msg = "the dapp " + dapp + " is not owned by account " + owner.to_string();
            eosio_assert(false, msg.c_str());
        }
    }

    ACTION create(string& memo, name& account, public_key& ownerkey, public_key& activekey, string& origin){
        Registry dapps(_self, _self.value);
        
        auto iterator = dapps.find(toUUID(origin));
        if(iterator != dapps.end())
        {
            if(name(memo)== iterator->owner){
                require_auth(iterator->owner);
            }else if(checkIfWhitelisted(name(memo), origin)){
                require_auth(name(memo));
            } else {
                auto msg = "only owner or whitelisted accounts can create new user accounts for " + origin;
                eosio_assert(false, msg.c_str());
            }
        }else {
            auto msg = "no owner account found for " + memo;
            eosio_assert(false, msg.c_str());
        }

        key_weight ok = key_weight{ownerkey, 1};
        authority ownerAuth{
                .threshold = 1,
                .keys = {ok},
                .accounts = {},
                .waits = {}
        };

        key_weight ak = key_weight{activekey, 1};
        authority activeAuth{
                .threshold = 1,
                .keys = {ak},
                .accounts = {},
                .waits = {}
        };

        asset ram = getRamCost();
        asset minimumCost = ram + asset(0'5000, S_SYS);
        
        string freeId = "free";
        string originFreeId = origin+"::free";

        if(hasBalance(originFreeId, minimumCost)) createFreeAccount(memo, account, ownerAuth, activeAuth, ram, originFreeId);
        else if(hasBalance(freeId, minimumCost))  createFreeAccount(memo, account, ownerAuth, activeAuth, ram, freeId);
        else {
            createJointAccount(memo, account, origin, ownerAuth, activeAuth, ram);        
        }                                    
    }



    /**********************************************/
    /***                                        ***/
    /***                Helpers                 ***/
    /***                                        ***/
    /**********************************************/

    void createFreeAccount(string& memo, name& account, authority& ownerAuth, authority& activeAuth, asset& ram, string& id){
        createAccount(account, ownerAuth, activeAuth, ram, NET, CPU);

        //subBalance(id, ram + asset(0'5000, S_SYS));

        if(hasBalance(memo, asset(0'0000, S_SYS))) {
            asset remainder = balanceFor(memo);
            action(
                    permission_level{ _self, "active"_n },
                    name("eosio.token"),
                    name("transfer"),
                    make_tuple(_self, account, remainder, memo)
            ).send();
          //  subBalance(memo, remainder);
        }
    }

    void createJointAccount(string& memo, name& account, string& origin, authority& ownerAuth, authority& activeAuth, asset& ram){
        // memo is the account that pays the remaining balance i.e
        // balance needed for new account creation - (balance contributed by the contributors)
        name contributor;

        asset balance;
        asset requiredBalance;

        asset ramFromPayer = ram;
        asset ramFromDapp = asset(0'0000, S_SYS);

        Balances balances(_self, _self.value);

        if(memo != origin && hasBalance(origin, ram)){
            uint64_t originId = toUUID(origin);
            auto dapp = balances.find(originId);

            if(dapp != balances.end()){
                // TODO: add the "find the contributor" logic here
                // TODO: fix this, get ram requirement from the registry table for the dapp
                contributor = dapp->contributors[1].contributor;
                ramFromDapp =  asset(0'5000, S_SYS);
                ramFromPayer -= ramFromDapp;
            }
        }

        if(ramFromPayer > asset(0'0000, S_SYS)){
            // find the balance of the "memo" account for the origin and check if it has balance >= total balance for RAM, CPU and net - (balance payed by the contributors)
            asset balance = findContribution(origin, name(memo));
            requiredBalance = ramFromPayer + CPU + NET;
            if(balance < requiredBalance){
            auto msg = "Not enough balance in "+ memo + " to pay for account creation.";
            eosio_assert(false, msg.c_str());
            }
        }

        // TODO: get net and cpu requirement from the registry table for the dapp
        createAccount(account, ownerAuth, activeAuth, ram, NET, CPU);

        subBalance(memo, origin, requiredBalance);

        if(ramFromDapp.amount > 0){
            subBalance(contributor.to_string(), origin, ramFromDapp);
        }
    }

    bool checkIfWhitelisted(name account, string dapp){
        Registry dapps(_self, _self.value);
        auto iterator = dapps.find(toUUID(dapp));
        auto position_in_whitelist = std::find(iterator->whitelisted_accounts.begin(), iterator->whitelisted_accounts.end(), account); 
        if(position_in_whitelist != iterator->whitelisted_accounts.end()){
            return true;
        }
        return false;
    }

    asset findContribution(string dapp, name contributor){
        Balances balances(_self, _self.value);
        uint64_t id = toUUID(dapp);
        auto iterator = balances.find(id);
        auto pred = [contributor](const contributors & item) {
                return item.contributor == contributor;
        };
        auto itr = std::find_if(std::begin(iterator->contributors), std::end(iterator->contributors), pred);    
        if(itr != std::end(iterator->contributors)){
            return itr->balance;
        } else {
            auto msg = "No contribution found for " + dapp + "by " + contributor.to_string();
            eosio_assert(false, msg.c_str());
        }
    }

    
    /***
     * Gets the current RAM cost in EOS for 4096 bytes of RAM.
     * @return
     */
    asset getRamCost(){
        RamInfo ramInfo(name("eosio"), name("eosio").value);
        auto ramData = ramInfo.find(S_RAM.raw());
        eosio_assert(ramData != ramInfo.end(), "Could not get RAM info");

        uint64_t base = ramData->base.balance.amount;
        uint64_t quote = ramData->quote.balance.amount;
        return asset((((double)quote / base))*4096, S_SYS);
    }

    /***
     * Creates an account based on passed in values
     */
    void createAccount(name& account, authority& ownerauth, authority& activeauth, asset& ram, asset& net, asset& cpu){
        newaccount new_account = newaccount{
            .creator = _self,
            .name = account,
            .owner = ownerauth,
            .active = activeauth
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
            make_tuple(_self, account, ram)
        ).send();

        action(
            permission_level{ _self, "active"_n },
            name("eosio"),
            name("delegatebw"),
            make_tuple(_self, account, net, cpu, true)
        ).send();
    }








    /**********************************************/
    /***                                        ***/
    /***                Balances                ***/
    /***                                        ***/
    /**********************************************/

    asset balanceFor(string& memo){
        Balances balances(_self, _self.value);
        uint64_t payerId = toUUID(memo);
        auto payer = balances.find(payerId);
        if(payer == balances.end()) return asset(0'0000, S_SYS);
        return payer->balance;
    }

    bool hasBalance(string memo, const asset& quantity){
        return balanceFor(memo).amount > quantity.amount;
    }

    void addBalance(const name& from, const asset& quantity, string& memo){
        vector<string> stats = common::split(memo, ',');
        uint64_t id = toUUID(stats[0]);
        int ram = stoi(stats[1]);

        Balances balances(_self, _self.value);
        auto iterator = balances.find(id);

        if(iterator == balances.end()) balances.emplace(_self, [&](auto& row){
            row.memo = id;
            row.contributors.push_back({from, quantity, ram});
            row.balance = quantity;
            row.origin = stats[0];
            row.timestamp = now();
        });
        else balances.modify(iterator, same_payer, [&](auto& row){
            auto pred = [from](const contributors & item) {
                return item.contributor == from;
            };
            std::vector<contributors>::iterator itr = std::find_if(std::begin(row.contributors), std::end(row.contributors), pred);    
            if(itr != std::end(row.contributors)){
                itr->balance += quantity;
                itr->ram = ram;
            } else {
                row.contributors.push_back({from, quantity, ram});
                row.timestamp = now();
            }
            row.balance += quantity;
        });
    }

    void subBalance(string memo, string&origin, const asset& quantity){
        uint64_t id = toUUID(origin);

        Balances balances(_self, _self.value);
        auto iterator = balances.find(id);

        eosio_assert(iterator != balances.end(), "No balance object");
        eosio_assert(iterator->balance.amount >= quantity.amount, "overdrawn balance" );

        if(iterator->balance.amount - quantity.amount <= 0){
            balances.erase(iterator);
        }

        else balances.modify(iterator, same_payer, [&](auto& row){
            auto pred = [memo](const contributors & item) {
                return item.contributor == name(memo);
            };
            auto itr = std::find_if(std::begin(row.contributors), std::end(row.contributors), pred);    
            if(itr != std::end(row.contributors)){
                row.balance -= quantity;
                itr->balance -= quantity;
            } else {
                auto msg = "The account " + memo + "not found as one of the contributors for " + origin; 
                eosio_assert(false, msg.c_str());
            }
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
        if(quantity.symbol != S_SYS) return;
        if(memo.length() > 64) return;
        addBalance(from, quantity, memo);
    }

};

extern "C" {
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    auto self = receiver;

    if( code == self ) switch(action) {
        EOSIO_DISPATCH_HELPER( createbridge, (clean)(create)(define)(whitelist) )
    }

    else {
        if(code == name("eosio.token").value && action == name("transfer").value){
            execute_action(name(receiver), name(code), &createbridge::transfer);
        }
    }
}
};