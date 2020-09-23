#ifndef __WALLET_OUTPUT_H__
#define __WALLET_OUTPUT_H__

#include <stdbool.h>

#include "address.h"
#include "transation.h"

typedef struct{
  bool liked;
  bool confirmed;
  bool rejected;
  bool conflicting;
  bool spent;
}inclusion_state_t;

/**
 * @brief Wallet output is a wallet specific representation of an output in the IOTA network.
 * 
 */
typedef struct{
  wallet_address_t address;
  byte_t tx_id[TRANSACTION_ID_BYTES];
  // TODO
  void* balance;
  inclusion_state_t state;
}wallet_output_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif