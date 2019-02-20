#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/action.hpp>

#include "lib/common.h"
#include "models/accounts.h"
#include "models/balances.h"
#include "models/registry.h"
    
class contributor_balances {

public:

    name createbridge = name("createbridge");

    /*
     * Returns the total balance contributed for a dapp
    */
    asset balanceFor(string& memo){
        balances::Balances balances(createbridge, createbridge.value);
        uint64_t payerId = common::toUUID(memo);
        auto payer = balances.find(payerId);
        if(payer == balances.end()) return asset(0'0000, common::getCoreSymbol(name("createbridge")));
        return payer->balance;
    }

    /*
     * Checks whether the balance for an account is greater than the required balance
    */
    bool hasBalance(string memo, const asset& quantity){
        return balanceFor(memo).amount > quantity.amount;
    }

    /*
     * Adds the amount contributed by the contributor for an app to the balances table
     * Called by the internal transfer function 
    */
    void addBalance(const name& from, const asset& quantity, string& memo){
        int ram;
        int totalaccounts;
        vector<string> stats = common::split(memo, ',');
        uint64_t id = common::toUUID(stats[0]);

        if(stats[0] != "free"){
            ram = stoi(stats[1]);
        } else {
            ram = 100;
        }

        if(stats.size() == 3){
            totalaccounts = stoi(stats[2]);
        } else {
            totalaccounts = -1;
        }

        balances::Balances balances(createbridge, createbridge.value);
        auto iterator = balances.find(id);
        if(iterator == balances.end()) balances.emplace(createbridge, [&](auto& row){
            row.memo = id;
            row.contributors.push_back({from, quantity, ram, totalaccounts, 0});
            row.balance = quantity;
            row.origin = stats[0];
            row.timestamp = now();

        });
        else balances.modify(iterator, same_payer, [&](auto& row){
            auto pred = [from](const balances::contributors & item) {
                return item.contributor == from;
            };
            std::vector<balances::contributors>::iterator itr = std::find_if(std::begin(row.contributors), std::end(row.contributors), pred);    
            if(itr != std::end(row.contributors)){
                itr->balance += quantity;
                itr->ram = ram;
                itr->totalaccounts = totalaccounts;
            } else {
                row.contributors.push_back({from, quantity, ram, totalaccounts, 0});
                row.timestamp = now();
            }
            row.balance += quantity;
        });
    }

    /*
     * Subtracts the amount used to create an account from the total amount contributed by a contributor for an app
     * Called by the create action
    */
    void subBalance(string memo, string& origin, const asset& quantity){
        uint64_t id = common::toUUID(origin);

        balances::Balances balances(createbridge, createbridge.value);
        auto iterator = balances.find(id);

        eosio_assert(iterator != balances.end(), "No balance object");
        eosio_assert(iterator->balance.amount >= quantity.amount, "overdrawn balance" );

        if(iterator->balance.amount - quantity.amount <= 0){
            balances.erase(iterator);
        }

        else balances.modify(iterator, same_payer, [&](auto& row){
            auto pred = [memo](const balances::contributors & item) {
                return item.contributor == name(memo);
            };
            auto itr = std::find_if(std::begin(row.contributors), std::end(row.contributors), pred);    
            if(itr != std::end(row.contributors)){
                row.balance -= quantity;
                itr->balance -= quantity;
                itr->createdaccounts += 1;
            } else {
                auto msg = "The account " + memo + "not found as one of the contributors for " + origin; 
                eosio_assert(false, msg.c_str());
            }
        });
    }
};