#ifndef __WALLET_ASSET_H__
#define __WALLET_ASSET_H__

#include "address.h"
#include "balance.h"
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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif