
#include "lib/common.h"
#include "models/registry.h"

class airdrops {

public:

    name createbridge = common::createbridge;

    /*
    * Called when a new user account is created
    * Transfers the dapp tokens to the new account created
    */
    void airdrop(string dapp, name account){
        registry::Registry dapps(createbridge, createbridge.value);
        auto iterator = dapps.find(common::toUUID(dapp));
        if(iterator != dapps.end())dapps.modify(iterator, same_payer, [&](auto& row){
            // check if the dapp has opted for airdrop
            if(row.airdrop->contract != name("")){
                if(row.airdrop->tokens.amount > 0){
                    asset tokens = row.airdrop->limit;
                    auto memo = "airdrop " + tokens.to_string() + " to " + account.to_string();
                    action(
                        permission_level{ createbridge, "active"_n },
                        row.airdrop->contract,
                        name("transfer"),
                        make_tuple(createbridge, account, tokens, memo)
                    ).send();
                    row.airdrop->tokens -= tokens;
                } else {
                    eosio_assert(false,("Not enough " + row.airdrop->tokens.symbol.code().to_string() + " with createbridge to airdrop.").c_str());
                }
            }
        });   
    }
};