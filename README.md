# Create Bridge Contract

This is an EOSIO account creator contract that allows apps to assume portions of the costs for account creation.

If you are an app you can send the `createbridge` contract funds with your `origin` as the memo to allocate some funds which
will be used to pay for 1/2 of the user's account's RAM.

Example:
- `cleos transfer YOUR_ACCOUNT createbridge "10.0000 EOS" "everipedia.org" -p YOUR_ACCOUNT`

Because of the way this contract works, anyone can help fund account creation for your app by simply sending amounts
to the contract with your app's origin inside of the memo.