#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/action.hpp>

#include "lib/common.h"
#include "models/accounts.h"
#include "models/balances.h"
#include "models/registry.h"

using namespace eosio;
using namespace common;
using namespace accounts;
using namespace registry;
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


    /***
     * Called to define an account name as the owner of a dapp along with the following details:
     * owner:           account name to be registered as the owner of the dapp 
     * dapp:            the string/account name representing the dapp
     * ram:             ram to put in the new user account created for the dapp
     * net:             EOS amount to be staked for net
     * cpu:             EOS amount to be staked for cpu
     * airdropcontract: dapp token contract
     * airdroptoken:    the total supply of dapp tokens to be airdropped
     * airdroplimit:    token amount to be airdropped to new users for the dapp
     * Only the owner account/whitelisted account will be able to create new user account for the dapp
     */ 
    ACTION define(name& owner, string dapp, asset ram, asset net, asset cpu, name airdropcontract, asset airdroptokens, asset airdroplimit) {
        require_auth(owner);
        Registry dapps(_self, _self.value);
        auto iterator = dapps.find(toUUID(dapp));
        if(iterator == dapps.end())dapps.emplace(_self, [&](auto& row){
            row.owner = owner;
            row.dapp = dapp;
            row.ram = ram;
            row.net = net;
            row.cpu = cpu;
            row.airdropcontract = airdropcontract;
            row.airdroptokens = airdroptokens;
            row.airdroplimit = airdroplimit;
        }); else {
            auto msg = "the dapp " + dapp + " is already registered by another account";
            eosio_assert(false, msg.c_str());
        }
    }

    /***
     * Lets the owner account of the dapp to whitelist other accounts. 
     */ 
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

    /***
     * Creates a new user account
     */ 
    ACTION create(string& memo, name& account, public_key& ownerkey, public_key& activekey, string& origin){
        Registry dapps(_self, _self.value);
        
        auto iterator = dapps.find(toUUID(origin));

        // Only owner/whitelisted account for the dapp can create accounts
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

    /***
     * Transfers the remaining balance of a contributor from createbridge back to the contributor
     */ 
    ACTION reclaim(name reclaimer, string dapp){
        require_auth(reclaimer);

        asset reclaimer_balance;
        bool nocontributor;

        Balances balances(_self, _self.value);

        auto iterator = balances.find(common::toUUID(dapp));
        auto msg = "no funds given by " + reclaimer.to_string() +  " for " + dapp;

        if(iterator != balances.end()){

            balances.modify(iterator, same_payer, [&](auto& row){
                auto pred = [reclaimer](const contributors & item) {
                    return item.contributor == reclaimer;
                };
                auto reclaimer_record = remove_if(std::begin(row.contributors), std::end(row.contributors), pred);
                if(reclaimer_record != row.contributors.end()){
                    reclaimer_balance = reclaimer_record->balance;
                    row.contributors.erase(reclaimer_record, row.contributors.end());
                    row.balance -= reclaimer_balance;
                } else {
                    eosio_assert(false, msg.c_str());
                }

                nocontributor = row.contributors.empty();
            });

            // delete the entire balance object if no contributors are there for the dapp
            if(nocontributor && iterator->balance == asset(0'0000, S_SYS)){
                    balances.erase(iterator);
            }

            // transfer the remaining balance for the contributor from the createbridge account to contributor's account
            auto memo = "reimburse the remaining balance to " + reclaimer.to_string();
            action(
                permission_level{ _self, "active"_n },
                name("eosio.token"),
                name("transfer"),
                make_tuple(_self, reclaimer, reclaimer_balance, memo)
            ).send();

        } else {
            eosio_assert(false, msg.c_str());
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
            //subBalance(memo, remainder);
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

        // gets the ram, net and cpu requirements for the new user accounts from the dapp registry
        Registry dapps(_self, _self.value);
        auto iterator = dapps.find(toUUID(origin));
        asset ramAmount = iterator->ram;
        asset net = iterator->net;
        asset cpu = iterator->cpu;

        if(memo != origin && hasBalance(origin, ram)){
            uint64_t originId = toUUID(origin);
            auto dapp = balances.find(originId);

            if(dapp != balances.end()){
                // TODO: add the "find the contributor" logic here
                contributor = dapp->contributors[1].contributor;
                ramFromDapp = (dapp->contributors[1].ram * ramAmount)/100;
                ramFromPayer -= ramFromDapp;
            }
        }

        // find the balance of the "memo" account for the origin and check if it has balance >= total balance for RAM, CPU and net - (balance payed by the contributors)
        if(ramFromPayer > asset(0'0000, S_SYS)){
            asset balance = findContribution(origin, name(memo));
            requiredBalance = ramFromPayer + cpu + net;
            if(balance < requiredBalance){
            auto msg = "Not enough balance in "+ memo + " to pay for account creation.";
            eosio_assert(false, msg.c_str());
            }
        }

        createAccount(account, ownerAuth, activeAuth, ram, NET, CPU);

        // subtract the used balance 
        subBalance(memo, origin, requiredBalance);
        if(ramFromDapp.amount > 0){
            subBalance(contributor.to_string(), origin, ramFromDapp);
        }

        // airdrop dapp tokens if requested
        airdrop(origin, account);
    }

    /***
     * Checks if an account is whitelisted for a dapp by the owner of the dapp
     * @return
     */
    bool checkIfWhitelisted(name account, string dapp){
        Registry dapps(_self, _self.value);
        auto iterator = dapps.find(toUUID(dapp));
        auto position_in_whitelist = std::find(iterator->whitelisted_accounts.begin(), iterator->whitelisted_accounts.end(), account); 
        if(position_in_whitelist != iterator->whitelisted_accounts.end()){
            return true;
        }
        return false;
    }

    /***
     * Gets the balance of a contributor for a dapp
     * @return
     */
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
        // FIX it to delegate from createbridge, instead of buying and staking from the new account created
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

    /**********************************************/
    /***                                        ***/
    /***               Airdrops                 ***/
    /***                                        ***/
    /**********************************************/

    void airdrop(string dapp, name account){
        Registry dapps(_self, _self.value);
        auto iterator = dapps.find(toUUID(dapp));
        if(iterator != dapps.end())dapps.modify(iterator, same_payer, [&](auto& row){
            if(row.airdropcontract != name("")){
                asset tokens = row.airdroplimit;
                auto memo = "airdrop " + tokens.to_string() + " to " + account.to_string();
                action(
                    permission_level{ _self, "active"_n },
                    row.airdropcontract,
                    name("transfer"),
                    make_tuple(_self, account, tokens, memo)
                ).send();
                row.airdroptokens -= tokens;
            }
        });   
    }
};

extern "C" {
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    auto self = receiver;

    if( code == self ) switch(action) {
        EOSIO_DISPATCH_HELPER( createbridge, (clean)(create)(define)(whitelist)(reclaim))
    }

    else {
        if(code == name("eosio.token").value && action == name("transfer").value){
            execute_action(name(receiver), name(code), &createbridge::transfer);
        }
    }
}
};