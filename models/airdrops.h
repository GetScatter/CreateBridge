#pragma once

namespace airdrops {
    struct [[eosio::table, eosio::contract("createbridge")]] airdrop {
	        string dapp;
            asset tokens; // the token amount transferred to createbridge to airdrop tokens 
            name airdropcontract; //it is very likely that the token contract for the dapp token is different then eosio.token contract
            asset limit;
    };
}
