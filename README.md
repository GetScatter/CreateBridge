# Create Bridge Contract

This is an EOSIO account creator contract that allows apps to assume portions of the costs for account creation.


If you are an app you can send the `createbridge` contract funds with your `origin` as the memo to allocate some funds which
will be used to pay for 1/2 of the user's account's RAM.


### Partial Costs

You can assume just 50% of the RAM costs for an account.
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "everipedia.org"`

### Full Costs

You can also opt to create entirely free accounts by adding a `::free` suffix to your `origin`
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "everipedia.org::free"`

### Globally Crowd-Free Accounts

Anyone can send EOS to this contract with the memo `free` to help fund completely free accounts for new EOS users.
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "free"`



## Everyone can help!

Because of the way this contract works, anyone can help fund account creation for your app by simply sending amounts
to the contract with your app's origin inside of the memo.
