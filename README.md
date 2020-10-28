# IOTA Pollen library  

This is a pollen wallet library for Linux and POSIX systems. 

# Disclaimer  

This library is is under developing and tested with [GoShimmer-0.3.0](https://github.com/iotaledger/goshimmer/releases/tag/v0.3.0). It's not guarantee to work with other GoShimmer release, please use with caution!  

# requirements  

* libcurl
* cmake 3.15 above

# Applications  

[pollen_qianbao_cli](https://github.com/oopsmonk/pollen_qianbao_cli): CLI wallet for Linux and Raspberry Pi.  

[pollen_qianbao_esp32](https://github.com/oopsmonk/iota_c_platformIO/tree/dev_pollen_esp32) (WIP): CLI wallet for ESP32 

# Wallet APIs  

## Wallet status  

* `wallet_init`: Creates a wallet instance
* `wallet_refresh`: Update ledger status with the connected node
* `wallet_is_node_synced`: Checks if the connected node is synced
* `wallet_request_funds`: Requests some funds from the faucet for testing purpose
* `wallet_balance`: Gets the confirmed balance
* `wallet_status_print`: Shows the status of this wallet
* `wallet_send_funds`: Issues a payment

## Addresses  

* `wallet_receive_address`: Gets a receive address
* `wallet_new_receive_address`: Generates a new address
* `wallet_remainder_address`: Returns the address that is used for the remainder of funds
* `wallet_addresses`: Gets all addresses
* `wallet_unspent_addresses`: Gets all unspent addresses
* `wallet_spent_addresses`: Gets all spent addresses

**Visit [wallet.h](https://github.com/oopsmonk/pollen_qianbao/blob/master/src/wallet/wallet.h) for more details.**

