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
    printf("[%s %d] OOM\n", __func__, __LINE__);
    return NULL;
  }

  // init seed
  if (seed) {
    memcpy(am->seed, seed, TANGLE_SEED_BYTES);
  } else {
    randombytes_buf((void* const)am->seed, TANGLE_SEED_BYTES);
  }

  // init spent address bitmask
  if (spent_addr) {
    am->spent_addr = bitmask_clone(spent_addr);
  } else {
    am->spent_addr = bitmask_new();
  }

  am->last_addr_index = last_addr_index;
  am->first_unspent_idx = 0;
  am->last_unspent_idx = 0;
  // TODO update address status from the Tangle
  return am;
}

void am_free(wallet_am_t* am) {
  if (am) {
    if (am->spent_addr) {
      bitmask_free(am->spent_addr);
    }
    free(am);
  }
}

void am_get_address(wallet_am_t* const am, uint64_t index, byte_t out_addr[]) {
  if (index > am->last_addr_index) {
    // update spent address status
    am_update_spent_address(am, index, true);
    am->last_addr_index = index;
  }
  // update unspent address index?
  if (index > am->last_unspent_idx) {
    printf("update last unspent index %" PRIu64 " to %" PRIu64 "\n", am->last_unspent_idx, index);
    am->last_unspent_idx = index;
  }

  address_get(am->seed, index, ADDRESS_VER_ED25519, out_addr);
}

// generates and returns a new unused address.
void am_get_new_address(wallet_am_t* const am, byte_t out_addr[]) {
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

void am_get_first_unspent_address(wallet_am_t* const am, byte_t addr[]) {
  am_get_address(am, am->first_unspent_idx, addr);
}

void am_get_last_unspent_address(wallet_am_t* const am, byte_t addr[]) {
  am_get_address(am, am->last_unspent_idx, addr);
}

addr_list_t* am_addresses(wallet_am_t* const am) {
  address_t tmp_addr = {};
  addr_list_t* list = addr_list_new();
  if (list == NULL) {
    return NULL;
  }

  for (uint64_t i = 0; i <= am->last_addr_index; i++) {
    address_get(am->seed, i, ADDRESS_VER_ED25519, tmp_addr.addr);
    tmp_addr.index = i;
    addr_list_push(list, &tmp_addr);
  }
  return list;
}

addr_list_t* am_unspent_addresses(wallet_am_t* const am) {
  address_t tmp_addr = {};
  addr_list_t* list = addr_list_new();
  if (list == NULL) {
    return NULL;
  }

  for (uint64_t i = am->first_unspent_idx; i <= am->last_addr_index; i++) {
    if (!am_is_spent_address(am, i)) {
      address_get(am->seed, i, ADDRESS_VER_ED25519, tmp_addr.addr);
      tmp_addr.index = i;
      addr_list_push(list, &tmp_addr);
    }
  }
  return list;
}

addr_list_t* am_spent_addresses(wallet_am_t* const am) {
  address_t tmp_addr = {};
  addr_list_t* list = addr_list_new();
  if (list == NULL) {
    return NULL;
  }

  for (uint64_t i = 0; i <= am->last_addr_index; i++) {
    if (am_is_spent_address(am, i)) {
      address_get(am->seed, i, ADDRESS_VER_ED25519, tmp_addr.addr);
      tmp_addr.index = i;
      addr_list_push(list, &tmp_addr);
    }
  }
  return list;
}

void am_print(wallet_am_t* am) {
  bitmask_show(am->spent_addr);
  printf("last address index: %" PRIu64 "\n", am->last_addr_index);
  printf("first unspent index: %" PRIu64 "\n", am->first_unspent_idx);
  printf("last unspent index: %" PRIu64 "\n", am->last_unspent_idx);
}
