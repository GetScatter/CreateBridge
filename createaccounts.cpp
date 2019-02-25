#include <eosiolib/eosio.hpp>

#include "lib/common.h"

#include "models/accounts.h"
#include "models/balances.h"
#include "models/registry.h"

#include "contributions.cpp"
#include "airdrops.cpp"

using namespace eosio;
using namespace std;

class createaccounts : public contributions, public airdrops{

public:
    name createbridge = common::createbridge;

    /***
     * Checks if an account is whitelisted for a dapp by the owner of the dapp
     * @return
     */
    void createJointAccount(string& memo, name& account, string& origin, accounts::authority& ownerAuth, accounts::authority& activeAuth){
        // memo is the account that pays the remaining balance i.e
        // balance needed for new account creation - (balance contributed by the contributors)
        vector<balances::chosen_contributors> contributors;
        name freeContributor;

        asset balance;
        asset requiredBalance;

        symbol coreSymbol = common::getCoreSymbol();
        asset ramFromDapp = asset(0'0000, coreSymbol);
        asset ramFromGlobalFund = asset(0'0000, coreSymbol);

        string freeId = "free";
        
        balances::Balances balances(createbridge, createbridge.value);
        registry::Registry dapps(createbridge, createbridge.value);
        
        // gets the ram, net and cpu requirements for the new user accounts from the dapp registry
        auto iterator = dapps.find(common::toUUID(origin));
        asset ram = iterator->ram;
        asset ramFromPayer = ram;
        asset net = iterator->net;
        asset cpu = iterator->cpu;

        if(memo != origin && contributions::hasBalance(origin, ram)){
            uint64_t originId = common::toUUID(origin);
            auto dapp = balances.find(originId);

            if(dapp != balances.end()){
                uint64_t seed = account.value;
                uint64_t value = name(memo).value;        
                contributors = getContributors(origin, seed, value, ram);
                
                for(std::vector<balances::chosen_contributors>::iterator itr = contributors.begin(); itr != contributors.end(); ++itr){
                    ramFromDapp += itr->rampay;
                }

                ramFromPayer -= ramFromDapp;
            }
        }

        // find the balance of the "memo" account for the origin and check if it has balance >= total balance for RAM, CPU and net - (balance payed by the contributors)
        if(ramFromPayer > asset(0'0000, coreSymbol)){
            asset balance = contributions::findContribution(origin, name(memo));
            requiredBalance = ramFromPayer + cpu + net;
            // if the "memo" account doesn't have enough fund, check globally available "free" pool
            if(balance < requiredBalance){
                auto msg = "Not enough balance in " + memo + " or donated by the contributors for " + origin + " to pay for account creation.";
                eosio_assert(false, msg.c_str());
            }
        }

        createAccount(account, ownerAuth, activeAuth, ram, net, cpu);

        // subtract the used balance 
        if(ramFromPayer.amount > 0)
        {
            subBalance(memo, origin, requiredBalance);
        }

        if(ramFromDapp.amount > 0){
            for(std::vector<balances::chosen_contributors>::iterator itr = contributors.begin(); itr != contributors.end(); ++itr){

                // check if the memo account and the dapp contributor is the same. If yes, only increament accounts created by 1
                if(itr->contributor == name{memo} && ramFromPayer.amount > 0){
                    subBalance(itr->contributor.to_string(), origin, itr->rampay,true);
                } else {
                    subBalance(itr->contributor.to_string(), origin, itr->rampay);
                }
            }
        }

        if(ramFromGlobalFund.amount > 0){
            subBalance(freeContributor.to_string(), freeId, requiredBalance);
        }
        
        // airdrop dapp tokens if requested
        airdrop(origin, account);
    }

    /***
     * Checks if an account is whitelisted for a dapp by the owner of the dapp
     * @return
     */
    bool checkIfWhitelisted(name account, string dapp){
        registry::Registry dapps(createbridge, createbridge.value);
        auto iterator = dapps.find(common::toUUID(dapp));
        auto position_in_whitelist = std::find(iterator->whitelisted_accounts.begin(), iterator->whitelisted_accounts.end(), account); 
        if(position_in_whitelist != iterator->whitelisted_accounts.end()){
            return true;
        }
        return false;
    }

    /***
     * Calls the chain to create a new account
     */ 
    void createAccount(name& account, accounts::authority& ownerauth, accounts::authority& activeauth, asset& ram, asset& net, asset& cpu){
        // FIX it to delegate from createbridge, instead of buying and staking from the new account created
        accounts::newaccount new_account = accounts::newaccount{
            .creator = createbridge,
            .name = account,
            .owner = ownerauth,
            .active = activeauth
        };

        name newAccountContract = common::getNewAccountContract();

        action(
            permission_level{ createbridge, "active"_n },
            newAccountContract,
            name("newaccount"),
            new_account
        ).send();

        action(
            permission_level{ createbridge, "active"_n },
            newAccountContract,
            name("buyram"),
            make_tuple(createbridge, account, ram)
        ).send();

        action(
            permission_level{ createbridge, "active"_n },
            newAccountContract,
            name("delegatebw"),
            make_tuple(createbridge, account, net, cpu, true)
        ).send();
    };

};