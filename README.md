# GoShimmer Client library

This is a client library that help you to develop IoT applications with a GoShimmer node in C language.

It aims to run on devices such as, Raspberry(Banana/Orange) Pi, ESP32, and microcontrollers with network stack.

# Disclaimer  

This library is is under developing and tested with [GoShimmer-0.3.0](https://github.com/iotaledger/goshimmer/releases/tag/v0.3.0). It's not guarantee to work with other GoShimmer release, please use with caution!  

# Requirements

Requirements are vary, it depends on your target.

For Raspberry Pi or Linux-like:

* libcurl
* cmake 3.15 above

For ESP32:

* [esp-idf](https://docs.espressif.com/projects/esp-idf/en/v4.1/index.html)  


# Applications

Use this client library you are able to write applications like:  
* [pollen_qianbao_cli](https://github.com/oopsmonk/pollen_qianbao_cli) (WIP): CLI wallet for Linux and Raspberry Pi.  
* [pollen_qianbao_esp32](https://github.com/oopsmonk/iota_c_platformIO/tree/dev_pollen_esp32) (WIP): CLI wallet on ESP32 platform  

# Support APIs

There are two API sets in this repository
* GoShimmer APIs  
* Wallet APIs  

## GoShimmer APIs

These APIs are provided by GoShimmer for interact with it directly, see [goshimmer/Client Lib](https://github.com/iotaledger/goshimmer/wiki/Client-Lib:-Interaction-with-layers) for more details.

* `faucet`: Obtains tokens from the faucet
* `info`: Gets node information
* `value/unspentOutputs`: Gets unspent outputs from addresses
* `value/sendTransaction`: Issues a transaction

**Visit [client/api](https://github.com/oopsmonk/goshimmer-client-c/tree/master/src/client/api) for more details.**

## Wallet APIs

Wallet APIs are higher layer APIs for wallet functionalities, it provides an easy way to build a customized wallet application as you need.

**Wallet functions**

* `wallet_init`: Creates a wallet instance
* `wallet_refresh`: Update ledger status with the connected node
* `wallet_is_node_synced`: Checks if the connected node is synced
* `wallet_request_funds`: Requests some funds from the faucet for testing purpose
* `wallet_balance`: Gets the confirmed balance
* `wallet_status_print`: Shows the status of this wallet
* `wallet_send_funds`: Issues a payment

**Address functions**

* `wallet_receive_address`: Gets a receive address
* `wallet_new_receive_address`: Generates a new address
* `wallet_remainder_address`: Returns the address that is used for the remainder of funds
* `wallet_addresses`: Gets all addresses
* `wallet_unspent_addresses`: Gets all unspent addresses
* `wallet_spent_addresses`: Gets all spent addresses

**Visit [wallet.h](https://github.com/oopsmonk/goshimmer-client-c/blob/master/src/wallet/wallet.h) for more details.**
