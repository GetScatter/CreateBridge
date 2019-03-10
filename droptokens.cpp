
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
                sendTokens(createbridge, account, iter->per_account, ("CreateBridge: Airdropped " + iter->per_account.to_string() + " from " + dapp), iter->contract);

                airdrops.modify(iter, same_payer, [&](auto& row){
                    row.balance -= iter->per_account;
                });
            } else {
                // TODO: Notify token account of lack of balances.
                // Might even want to de-register this airdrop token and send remaining fractional balances
                // back to owner to clear up RAM automatically.
            }
        }
    }
};