# Create Bridge Contract

This is an EOSIO account creator contract that allows apps to assume portions of the costs for account creation.

If you are an app you can send the `createbridge` contract funds with your `origin` as the memo to allocate some funds which will be used to pay for 1/2 of the user's account's RAM.

### Define an account name your dapp

If you are an app, you can define a unique identifier for the dapp and register the `owner` account name for the dapp.
Only the owner account/ whitelisted accounts would be able to create new user accounts for your dapp. 
You can also specify the amount of EOS for ram, net and cpu (in order) to give to the new user accounts. 
Also, if you wish to airdrop dapp tokens to the newly created account specify the airdrop contract, total airdrop supply
and the amount to airdrop to every new user account created. 
- `cleos push action createbridge define '[YOUR_ACCOUNT,app_name,ram,net,cpu,airdrop_contract,airdrop_tokens,airdrop_limit]'`
- `cleos push action createbridge define '["everipediabp","everipedia.org","1.0000 SYS","1.0000 SYS","1.0000 SYS","everipediaiq","1000000.0000 IQ","10.0000 IQ"]' -p eosio@active`

### Whitelist other accounts

You can whitelist other accounts, to create account on behalf of your dapp.
-  `cleos push action createbridge whitelist '[YOUR_ACCOUNT,account_name,app_name]'`

### Partial Costs

You can specify the app you want to contribute to along with the percentage of RAM cost you want to contribute, separated by comma in the memo field for transfer action.
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "everipedia.org,50"`
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "everipedia.org,100"`


### Globally Crowd-Free Accounts

Anyone can send EOS to this contract with the memo `free` to help fund completely free accounts for new EOS users.
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "free"`


### Reclaim balance
- As a contributor, you can reclaim your balance at any time.
- You can specify the token symbol of the balance you want to reclaim:
    - Native token to reclaim chain symbol balance
    - Dapp token symbol to reclaim the remaining airdrop balance. Only to be reclaimed by the dapp owner
- `cleos push action createbridge reclaim '["YOUR_ACCOUNT","app_name","token_symbol"]' -p $CONTRIBUTOR

### Everyone can help!

Because of the way this contract works, anyone can help fund account creation for your app by simply sending amounts
to the contract with the app origin inside of the memo.
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "everipedia.org,50"`

### To get started with createbridge, run the following scripts present inside /scripts:
- Attach the code level permission to active permission of createbridge
    ./perms.sh

- Specify the token symbol for the chain and also the contract to call for newaccount action
    ./set.sh

### To test the code for a sampleapp
- Define a sampledapp and contributor for it
    ./define.sh
    ./contribute.sh

- Create a new user account for the sampleapp
    ./createaccount.sh

- Reclaim the contributed balance for the sampleapp
    ./reclaim.sh
