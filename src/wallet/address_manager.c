#include "wallet/address_manager.h"

wallet_am_t* am_new(byte_t const seed[], uint64_t last_addr_index, bitmask_t* spent_addr) {
  wallet_am_t* am = malloc(sizeof(wallet_am_t));
  if (!am) {
    return NULL;
  }
  memcpy(am->seed, seed, IOTA_SEED_BYTES);
  am->spent_addr = bitmask_clone(spent_addr);
  am->last_addr_index = last_addr_index;
  return am;
}
