#ifndef __WALLET_ASSET_REGISTRY_H__
#define __WALLET_ASSET_REGISTRY_H__

#include "core/address.h"
#include "core/balance.h"
#include "utils/iota_str.h"

/**
 * @brief Wallet asset represents a container for all the information regarding a colored coin.
 *
 */
typedef struct {
  byte_t color[BALANCE_COLOR_BYTES]; /**< Color contains the identifier of this asset */
  iota_str_t* name;                  /**< Name of the asset */
  iota_str_t* symbol;                /**< currency symbol of the asset (optional) */
  int precision; /**< Precision defines how many decimal places are shown when showing this asset in wallets */
  wallet_address_t address; /**< defines the target address where the asset is supposed to be created */
  uint64_t amount;          /**< the amount of tokens that we want to create */
} wallet_asset_t;

// hashtable of assets

#ifdef __cplusplus
extern "C" {
#endif

int wallet_ar_new();

// registers an asset in the registry, so we can look up names and symbol of colored coins.
int wallet_ar_register_asset(byte_t const color[], wallet_asset_t* asset);

// returns the name of the given asset.
int wallet_ar_name(byte_t const color[]);

// returns the symbol of the token.
int wallet_ar_symbol(byte_t const color[]);

int wallet_ar_precision(byte_t const color[]);

#ifdef __cplusplus
}
#endif

#endif