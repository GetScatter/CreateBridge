# Create Bridge Contract

This is an EOSIO account creator contract that allows apps to assume portions of the costs for account creation.


If you are an app you can send the `createbridge` contract funds with your `origin` as the memo to allocate some funds which will be used to pay for 1/2 of the user's account's RAM.

### Define an account name for your dapp

If you are an app, you can define a unique identifier for the dapp and register the `owner` account name for the dapp. Also, you can specify the amount of EOS for ram, net and cpu (in order) to give to the new user accounts. Only the owner account/ whitelisted accounts would be able to create new user accounts for your dapp. 
- `cleos push action createbridge define '[YOUR_ACCOUNT,app_name,ram,net,cpu]'`

### Whitelist other accounts

You can whitelist other accounts, to create account on behalf of your dapp.
-  `cleos push action createbridge whitelist '[YOUR_ACCOUNT,ACCOUNT_NAME,app_name]'`

### Partial Costs

You can specify the app you want to contribute to along with the percentage of RAM cost you want to contribute, separated by comma in the memo field for transfer action.
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "everipedia.org,50"`

### Full Costs

You can also opt to create entirely free accounts by adding a `::free` suffix to your `origin`
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "everipedia.org::free"`

### Globally Crowd-Free Accounts

Anyone can send EOS to this contract with the memo `free` to help fund completely free accounts for new EOS users.
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "free"`



## Everyone can help!

Because of the way this contract works, anyone can help fund account creation for your app by simply sending amounts
to the contract with your app's origin inside of the memo.
