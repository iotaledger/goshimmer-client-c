#ifndef __WALLET_API_H__
#define __WALLET_API_H__

#include <stdbool.h>

#include "client/client_service.h"
#include "wallet/address_manager.h"
#include "wallet/asset_registry.h"
#include "wallet/output.h"

typedef struct {
  byte_t address[TANGLE_ADDRESS_BYTES];
  byte_t tx_output_id[TX_OUTPUT_ID_BYTES];
} unspent_output_t;

typedef struct {
  tangle_client_conf_t endpoint;
  wallet_am_t* addr_manager;
  // wallet_ar_t asset_reg;
  // wallet_om_t unspent_manager;
} wallet_t;

// a struct that is used to aggregate the optional parameters provided in the send founds call
typedef struct {
  int64_t amount;
  byte_t address[TANGLE_ADDRESS_BYTES];
  byte_t color[BALANCE_COLOR_BYTES];
  byte_t remainder[TANGLE_ADDRESS_BYTES];
} send_funds_op_t;

#ifdef __cplusplus
extern "C" {
#endif

// creates or init a wallet instance

/**
 * @brief Creates or initializes a wallet instance
 *
 * @param[in] url The URL of an endpoint
 * @param[in] port The port number, 0 for default port (8443 or 443)
 * @param[in] seed The seed, NULL for random seed
 * @return wallet_t* A wallet instance
 */
wallet_t* wallet_init(char const url[], uint16_t port, byte_t const seed[]);

/**
 * @brief Frees a wallet instance
 *
 * @param[in] w A wallet instance
 */
void wallet_free(wallet_t* w);

/**
 * @brief Checks if connected endpoint is synced.
 *
 * @param[in] w A wallet instance
 * @return true The connected node is synced
 * @return false The connected node is unsynced
 */
bool wallet_is_node_synced(wallet_t* w);

/**
 * @brief Gets a receive address. aka the last unspent address
 *
 * @param[in] w A wallet instance
 * @param[out] addr The receive address
 */
void wallet_receive_address(wallet_t* w, byte_t addr[]);

/**
 * @brief Generates and returns a new unused receive address.
 *
 * @param[in] w A wallet instance
 * @param[out] addr The new unused address
 */
void wallet_new_receive_address(wallet_t* w, byte_t addr[]);

/**
 * @brief Returns the address that is used for the remainder of funds, aka first unspent address
 *
 * @param[in] w A wallet instance
 * @param[out] addr The remainder address
 */
void wallet_remainder_address(wallet_t* w, byte_t addr[]);

/**
 * @brief Gets a list of all addresses of the wallet.
 *
 * @param[in] w A wallet instance
 * @return addr_list_t*
 */
addr_list_t* wallet_addresses(wallet_t* const w);

/**
 * @brief Gets a list of all unspent addresses of the wallet.
 *
 * @param[in] w A wallet instance
 * @return addr_list_t*
 */
addr_list_t* wallet_unspent_addresses(wallet_t* const w);

/**
 * @brief Gets a list of all spent addresses of the wallet.
 *
 * @param[in] w A wallet instance
 * @return addr_list_t*
 */
addr_list_t* wallet_spent_addresses(wallet_t* const w);

// returns the unspent outputs that are available for spending.
int wallet_unspent_outputs(wallet_t* w, send_funds_op_t* opt);

// issues a payment of the given amount to the given address.
int wallet_send_funds();
int wallet_send_funds_opt();

// creates a new colored token with the given details.
int wallet_create_asset();
int wallet_update_asset();
int wallet_delete_asset();

// requests some funds from the faucet for testing purposes.
int wallet_req_funds();

// the confirmed and pending balance of the funds managed by this wallet.
int wallet_balance();

// from all the inputs determine which ones we need to consume.
int wallet_determine_outputs();

// build input for the transfer
int wallet_build_inputs();

// build outputs for the transfer
int wallet_build_outputs();

#ifdef __cplusplus
}
#endif

#endif