#ifndef __WALLET_OUTPUT_H__
#define __WALLET_OUTPUT_H__

#include <stdbool.h>

#include "core/address.h"
#include "core/balance.h"
#include "core/transation.h"

typedef struct {
  bool liked;
  bool confirmed;
  bool rejected;
  bool conflicting;
  bool spent;
} inclusion_state_t;

/**
 * @brief Wallet output is a wallet specific representation of an output in the IOTA network.
 *
 */
typedef struct {
  wallet_address_t address;
  byte_t tx_id[TRANSACTION_ID_BYTES];
  balance_h_t* balance;
  inclusion_state_t state;
} wallet_output_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif