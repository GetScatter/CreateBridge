#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/action.hpp>

#include "lib/common.h"
#include "lib/publickey.h"

#include "models/accounts.h"
#include "models/balances.h"
#include "models/registry.h"
#include "models/airdrops.h"

#include "createaccounts.cpp"

using namespace eosio;
using namespace common;
using namespace accounts;
using namespace registry;
using namespace balances;
using namespace airdrops;
using namespace std;

CONTRACT createbridge : contract, public createaccounts {
private:
    Registry dapps;
    Balances balances;
    Token token;

public:
    using contract::contract;
    static name external_code;
    createbridge(name receiver, name code,  datastream<const char*> ds):contract(receiver, code, ds),
        dapps(_self, _self.value),
        balances(_self, _self.value),
        token(_self, _self.value){}

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
     * Called to specify the following details:
     * symbol:              the core token of the chain or the token used to pay for new user accounts of the chain  
     * newAccountContract:  the contract to call for new account action 
     * minimumram:           minimum bytes of RAM to put in a new account created on the chain 
    */
    ACTION init(const symbol& symbol, name newaccountcontract, uint64_t minimumram){
        require_auth(_self);

        auto iterator = token.find(symbol.raw());

        if(iterator == token.end())token.emplace(_self, [&](auto& row){
            row.S_SYS = symbol;
            row.newaccountcontract = newaccountcontract;
            row.min_ram = minimumram;
        }); else token.modify(iterator, same_payer, [&](auto& row){
            row.S_SYS = symbol;
            row.newaccountcontract = newaccountcontract;
            row.min_ram = minimumram;
        });    
    }

    /***
     * Called to define an account name as the owner of a dapp along with the following details:
     * owner:           account name to be registered as the owner of the dapp 
     * dapp:            the string/account name representing the dapp
     * ram_bytes:       bytes of ram to put in the new user account created for the dapp
     * net:             EOS amount to be staked for net
     * cpu:             EOS amount to be staked for cpu
     * airdropcontract: airdropdata struct/json or null
     * Only the owner account/whitelisted account will be able to create new user account for the dapp
     */ 

    ACTION define(name& owner, string dapp, uint64_t ram_bytes, asset net, asset cpu) {
        require_auth(dapp != "free" ? owner : _self);

        auto iterator = dapps.find(toUUID(dapp));

        eosio_assert(iterator == dapps.end() || (iterator != dapps.end() && iterator->owner == owner),
                ("the dapp " + dapp + " is already registered by another account").c_str());

        uint64_t min_ram = getMinimumRAM();

        eosio_assert(ram_bytes >= min_ram, ("ram for new accounts must be equal to or greater than " + to_string(min_ram) + " bytes.").c_str());


        // Creating a new dapp reference
        if(iterator == dapps.end()) dapps.emplace(_self, [&](auto& row){
            row.id = dapps.available_primary_key();
            row.owner = owner;
            row.dapp = dapp;
            row.ram_bytes = ram_bytes;
            row.net = net;
            row.cpu = cpu;
        });

        // Updating an existing dapp reference's configurations
        else dapps.modify(iterator, same_payer, [&](auto& row){
            row.ram_bytes = ram_bytes;
            row.net = net;
            row.cpu = cpu;
        });
    }


    /***
     * Registers an airdrop with a specific dapp.
     * @param dapp - The name of the dapp
     * @param contract - The airdropped token contract
     * @param per_account - How many tokens to drop per new account.
     * @return
     */
    ACTION regdrop(string dapp, name& contract, asset& per_account){
        auto existingDapp = dapps.find(toUUID(dapp));
        eosio_assert(existingDapp != dapps.end(), (dapp + " is not registered.").c_str());
        require_auth(existingDapp->owner);

        Airdrops airdrops(_self, existingDapp->id);
        auto size = std::distance(airdrops.cbegin(),airdrops.cend());
        eosio_assert(size < 5, "You can only airdrop 5 tokens at once.");

        airdrops.emplace(existingDapp->owner, [&](auto& row){
            row.key = airdrop::keyFrom(contract, per_account);
            row.contract = contract;
            row.per_account = per_account;
            row.balance = asset(0'0000, per_account.symbol);
        });
    }

    /***
     * Removes all airdrops registered on a dapp, or a specified one at an ID.
     * @param dapp
     * @param id (optional)
     * @return
     */
    ACTION remdrop(string dapp, std::optional<uint64_t> id){
        auto existingDapp = dapps.find(toUUID(dapp));
        eosio_assert(existingDapp != dapps.end(), (dapp + " is not registered.").c_str());
        require_auth(existingDapp->owner);

        Airdrops airdrops(_self, existingDapp->id);

        if(id){
            auto drop = airdrops.find(*id);
            eosio_assert(drop != airdrops.end(), "There was no such ID in this dapp's airdrops");
            airdrops.erase(drop);
        } else {
            while(airdrops.begin() != airdrops.end()){
                auto iter = --airdrops.end();

                action(
                        permission_level{ _self, "active"_n },
                        iter->contract,
                        name("transfer"),
                        make_tuple(_self, existingDapp->owner, iter->balance, "Reclaimed airdrop tokens.")
                ).send();

                airdrops.erase(iter);
            }
        }

    }

    /***
     * Lets the owner account of the dapp to whitelist other accounts. 
     */ 
    ACTION whitelist(name owner, name account, string dapp){
        require_auth(owner);

        auto iterator = dapps.find(toUUID(dapp));

        if(iterator != dapps.end() && owner == iterator->owner) dapps.modify(iterator, same_payer, [&](auto& row){
            if (std::find(row.custodians.begin(), row.custodians.end(), account) == row.custodians.end()){
                row.custodians.push_back(account);
            }
        });

        else eosio_assert(false, ("the dapp " + dapp + " is not owned by account " + owner.to_string()).c_str());
    }

    /***
     * Creates a new user account. 
     * It also airdrops custom dapp tokens to the new user account if a dapp owner has opted for airdrops
     * memo:                name of the account paying for the balance left after getting the donation from the dapp contributors 
     * account:             name of the account to be created
     * ownerkey,activekey:  key pair for the new account  
     * origin:              the string representing the dapp to create the new user account for. For ex- everipedia.org, lumeos
     * For new user accounts, it follows the following steps:
     * 1. Choose a contributor, if any, for the dapp to fund the cost for new account creation
     * 2. Check if the contributor is funding 100 %. If not, check if the "memo" account has enough to fund the remaining cost of account creation
     * 3. If not, then check the globally available free fund for the remaining cost of an account creation
    */
    ACTION create(string& memo, name& account, public_key& ownerkey, public_key& activekey, string& origin){
        auto iterator = dapps.find(toUUID(origin));

        // Only owner/whitelisted account for the dapp can create accounts
        if(iterator != dapps.end())
        {
            if(name(memo)== iterator->owner)                    require_auth(iterator->owner);
            else if(checkIfWhitelisted(name(memo), origin))     require_auth(name(memo));
            else if(origin == "free")                           print("using globally available free funds to create account");
            else                                                eosio_assert(false, ("only owner or whitelisted accounts can create new user accounts for " + origin).c_str());
        }
        else {
            eosio_assert(false, ("no owner account found for " + origin).c_str());
        }

        authority owner{ .threshold = 1, .keys = {key_weight{ownerkey, 1}}, .accounts = {}, .waits = {} };
        authority active{ .threshold = 1, .keys = {key_weight{activekey, 1}}, .accounts = {}, .waits = {} };

        createJointAccount(memo, account, origin, owner, active);
    }

    /***
     * Transfers the remaining balance of a contributor from createbridge back to the contributor
     * reclaimer: account trying to reclaim the balance
     * dapp:      the dapp name for which the account is trying to reclaim the balance
     * sym:       symbol of the tokens to be reclaimed. It can have value based on the following scenarios:
     *            - reclaim the "native" token balance used to create accounts. For ex - EOS/SYS
     *            - reclaim the remaining airdrop token balance used to airdrop dapp tokens to new user accounts. For ex- IQ/LUM
     */ 
    ACTION reclaim(name reclaimer, string dapp){
        require_auth(reclaimer);

        asset reclaimer_balance;
        bool nocontributor;

        auto iterator = balances.find(common::toUUID(dapp));

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
                    eosio_assert(false, ("no remaining contribution for " + dapp + " by " + reclaimer.to_string()).c_str());
                }

            nocontributor = row.contributors.empty();
        });

        // delete the entire balance object if no contributors are there for the dapp
        if(nocontributor && iterator->balance == asset(0'0000, getCoreSymbol())){
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
            eosio_assert(false, ("no funds given by " + reclaimer.to_string() +  " for " + dapp).c_str());
        }
    }

    /**********************************************/
    /***                                        ***/
    /***               Transfers                ***/
    /***                                        ***/
    /**********************************************/

    /***
     * Only accepts EOS tokens from the eosio.token contract. Will never be used for alt-tokens.
     * @param from
     * @param to
     * @param quantity
     * @param memo
     */
    void transfer(const name& from, const name& to, const asset& quantity, string& memo){
        if(to != _self) return;
        if(from == name("eosio.stake")) return;
        if(quantity.symbol != getCoreSymbol()) return;
        addBalance(from, quantity, memo);
    }

    /***
     * Only accepts transferring alt-tokens. Will never be used for EOS.
     * @param from
     * @param to
     * @param quantity
     * @param dapp
     */
    void transferAirdrop(const name& from, const name& to, const asset& quantity, string& dapp){
        if(to != _self) return;
        auto existingDapp = dapps.find(toUUID(dapp));
        eosio_assert(existingDapp != dapps.end(), (dapp + " is not registered.").c_str());

        // The row is created by the app creator when registering the token so that there is true RAM owner.
        // Sadly this means that airdrop tokens can never be given back to community supporters,
        // as that would open up a RAM vulnerability due to having to use _self as payer in transfer
        // catches, however they can at least send airdrop tokens to help fund new dapp users as well.
        Airdrops airdrops(_self, existingDapp->id);
        auto airdrop = airdrops.find(airdrop::keyFrom(external_code, quantity));
        eosio_assert(airdrop != airdrops.end(), "Could not find airdrop row");
        // Murmur collisions possible, so checking symbol and contract.
        eosio_assert(airdrop->per_account.symbol == quantity.symbol, "Invalid symbol.");
        eosio_assert(airdrop->contract == external_code, "Invalid contract.");

        airdrops.modify(airdrop, same_payer, [&](auto& row){
            row.balance += quantity;
        });
    }
};

extern "C" {
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    if( code == receiver ) switch(action) {
        EOSIO_DISPATCH_HELPER( createbridge, (init)(clean)(create)(define)(regdrop)(remdrop)(whitelist)(reclaim))
    }

    else {
        if(code == name("eosio.token").value && action == name("transfer").value){
            execute_action(name(receiver), name(code), &createbridge::transfer);
        }
        else if (action == name("transfer").value){
            createbridge::external_code = name{code};
            execute_action(name(receiver), name(code), &createbridge::transferAirdrop);
        }
    }
}
};