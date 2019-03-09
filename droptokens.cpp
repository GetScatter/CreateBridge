
#include "lib/common.h"
#include "models/airdrops.h"
#include "models/registry.h"

class droptokens {

public:

    name createbridge = common::createbridge;

    /*
    * Called when a new user account is created
    * Transfers the dapp tokens to the new account created
    */
    void airdrop(string dapp, name account){
        //TODO: Fix me
        registry::Registry dapps(createbridge, createbridge.value);
        auto existingDapp = dapps.find(common::toUUID(dapp));
        if(existingDapp == dapps.end()) return;

        airdrops::Airdrops airdrops(createbridge, existingDapp->id);
        auto iter = airdrops.begin();
        while(iter != airdrops.end()){
            if(iter->balance.amount >= iter->per_account.amount){
                action(
                    permission_level{ createbridge, "active"_n },
                    iter->contract,
                    name("transfer"),
                    make_tuple(createbridge, account, iter->per_account, ("CreateBridge: Airdropped " + iter->per_account.to_string() + " from " + dapp))
                ).send();

                airdrops.modify(iter, same_payer, [&](auto& row){
                    row.balance -= iter->per_account;
                });
            }
        }


//        if(iterator != dapps.end()) dapps.modify(iterator, same_payer, [&](auto& row){
//            // check if the dapp has opted for airdrop
//            if(row.airdrop->contract != name("")){
//                if(row.airdrop->tokens.amount > 0){
//                    asset tokens = row.airdrop->limit;
//                    auto memo = "airdrop " + tokens.to_string() + " to " + account.to_string();
//                    action(
//                        permission_level{ createbridge, "active"_n },
//                        row.airdrop->contract,
//                        name("transfer"),
//                        make_tuple(createbridge, account, tokens, memo)
//                    ).send();
//                    row.airdrop->tokens -= tokens;
//                } else {
//                    eosio_assert(false,("Not enough " + row.airdrop->tokens.symbol.code().to_string() + " with createbridge to airdrop.").c_str());
//                }
//            }
//        });
    }
};