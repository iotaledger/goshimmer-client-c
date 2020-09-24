#ifndef __WALLET_AM_H__
#define __WALLET_AM_H__

#include <stdbool.h>

#include "core/address.h"
#include "utils/bitmask.h"

typedef struct {
  byte_t seed[IOTA_SEED_BYTES];
  uint64_t last_addr_index;
  bitmask_t* spent_addr;
  uint64_t first_unspent_idx;
  uint64_t last_unspent_idx;
} wallet_am_t;

#ifdef __cplusplus
extern "C" {
#endif

wallet_am_t* am_new();

// get address from the given address
void am_get_address(wallet_am_t* const am, uint64_t index, byte_t out_addr[]);

static void am_get_new_address(wallet_am_t* const am, byte_t out_addr[]) {
  am_get_address(am, am->last_addr_index + 1, out_addr);
}

bool am_is_spent_address(wallet_am_t* const am, uint64_t index);

#ifdef __cplusplus
}
#endif

#endif