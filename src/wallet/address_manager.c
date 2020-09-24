#include <inttypes.h>
#include <string.h>

#include "wallet/address_manager.h"

static void am_update_spent_address(wallet_am_t* const am, uint64_t index, bool new) {
  if (bitmask_op(am->spent_addr, index, new ? BITMASK_CLEAR : BITMASK_SET) != 0) {
    printf("[%s:%d] bitmask operating failed\n", __func__, __LINE__);
  }
}

// searches for the first unspent address and updates it.
static void am_update_first_unspent_index(wallet_am_t* const am) {
  for (uint64_t i = am->first_unspent_idx; i < am->last_addr_index; i++) {
    if (am_is_spent_address(am, i) == false) {
      am->first_unspent_idx = i;
      return;
    }
  }
  printf("[%s:%d] first unspent not found?\n", __func__, __LINE__);
}

// searches for the last unspent address and updates it.
static void am_update_last_unspent_index(wallet_am_t* const am) {
  for (uint64_t i = am->last_unspent_idx; i > 0; i--) {
    if (am_is_spent_address(am, i) == false) {
      am->last_unspent_idx = i;
      return;
    }
  }
  printf("[%s:%d] last unspent not found?\n", __func__, __LINE__);
}

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

void am_get_address(wallet_am_t* const am, uint64_t index, wallet_address_t* const out_addr) {
  if (index > am->last_addr_index) {
    // update spent address status
    am_update_spent_address(am, index, true);
    am->last_addr_index = index;
  }
  // update unspend address index?
  if (index > am->last_unspent_idx) {
    printf("update last unspend index %" PRIu64 " to %" PRIu64 "\n", am->last_unspent_idx, index);
    am->last_unspent_idx = index;
  }

  address_get(am->seed, index, ADDRESS_VER_ED25519, out_addr);
}

// generates and returns a new unused address.
void am_get_new_address(wallet_am_t* const am, wallet_address_t* const out_addr) {
  am_get_address(am, am->last_addr_index + 1, out_addr);
}

// returns true if the address given by the address index was spent already.
bool am_is_spent_address(wallet_am_t const* const am, uint64_t index) { return bitmask_get(am->spent_addr, index); }

void am_mark_spent_address(wallet_am_t* const am, uint64_t index) {
  bitmask_op(am->spent_addr, index, BITMASK_SET);
  // update spent address index
  if (index == am->first_unspent_idx) {
    // update first unspent index
    am_update_first_unspent_index(am);
  }

  if (index == am->last_unspent_idx) {
    // update last unspent index
    am_update_last_unspent_index(am);
  }
}

void am_get_first_unspent_address(wallet_am_t* const am, wallet_address_t* addr) {
  am_get_address(am, am->first_unspent_idx, addr);
}

void am_get_last_unspent_address(wallet_am_t* const am, wallet_address_t* addr) {
  am_get_address(am, am->last_unspent_idx, addr);
}
