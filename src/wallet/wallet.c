#include <stdio.h>

#include "client/api/get_funds.h"
#include "client/api/get_node_info.h"
#include "client/api/get_unspent_outputs.h"
#include "wallet/wallet.h"

wallet_t* wallet_init(char const url[], uint16_t port, byte_t const seed[], uint64_t last_addr, uint64_t first_unspent,
                      uint64_t last_unspent) {
  wallet_t* ctx = malloc(sizeof(wallet_t));
  if (ctx == NULL) {
    printf("[%s %d] OOM\n", __func__, __LINE__);
    return NULL;
  }

  // address manager, we should update address status later.
  // TODO: init local unspent/spent addresses
  bitmask_t* addr_mask = bitmask_new();
  if (addr_mask == NULL) {
    goto err;
  }

  // mask spent address
  for (uint64_t i = 0; i < first_unspent; i++) {
    bitmask_op(addr_mask, i, BITMASK_SET);
  }
  bitmask_show(addr_mask);

  ctx->addr_manager = am_new(seed, last_addr, addr_mask);
  if (ctx->addr_manager == NULL) {
    goto err;
  }
  ctx->addr_manager->first_unspent_idx = first_unspent;
  ctx->addr_manager->last_unspent_idx = last_unspent;
  ctx->addr_manager->last_addr_index = last_addr;

  // client endpoint
  strcpy(ctx->endpoint.url, url);
  ctx->endpoint.port = port;

  // unspent output manager
  ctx->unspent = unspent_outputs_init();

  // fetch remote status, sync with node
  if (wallet_refresh(ctx, true) == false) {
    printf("[%s:%d] wallet status update failed\n", __func__, __LINE__);
  }

  if (addr_mask) {
    bitmask_free(addr_mask);
  }
  return ctx;

err:
  if (addr_mask) {
    bitmask_free(addr_mask);
  }

  wallet_free(ctx);
  return NULL;
}

void wallet_free(wallet_t* w) {
  if (w) {
    if (w->addr_manager) {
      am_free(w->addr_manager);
    }
    if (w->unspent) {
      unspent_outputs_free(&w->unspent);
    }
  }
  free(w);
}

bool wallet_is_node_synced(wallet_t* w) {
  if (!w) {
    printf("[%s:%d] wallet instance is uninitialized\n", __func__, __LINE__);
    return false;
  }

  res_node_info_t info;
  if (get_node_info(&w->endpoint, &info) != 0) {
    printf("[%s:%d] get node info failed\n", __func__, __LINE__);
    return false;
  }
  return info.is_synced;
}

void wallet_receive_address(wallet_t* w, byte_t addr[]) { am_get_last_unspent_address(w->addr_manager, addr); }

void wallet_new_receive_address(wallet_t* w, byte_t addr[]) { am_get_new_address(w->addr_manager, addr); }

void wallet_remainder_address(wallet_t* w, byte_t addr[]) { am_get_first_unspent_address(w->addr_manager, addr); }

addr_list_t* wallet_addresses(wallet_t* w) { return am_addresses(w->addr_manager); }

addr_list_t* wallet_unspent_addresses(wallet_t* w) { return am_unspent_addresses(w->addr_manager); }

addr_list_t* wallet_spent_addresses(wallet_t* const w) { return am_spent_addresses(w->addr_manager); }

bool wallet_refresh(wallet_t* w, bool include_spent) {
  bool ret = false;
  addr_list_t* addrs = NULL;
  bool has_ids = false;
  if (include_spent) {
    addrs = wallet_addresses(w);
  } else {
    addrs = wallet_unspent_addresses(w);
  }

  if (addrs == NULL || addr_list_len(addrs) == 0) {
    printf("[%s:%d] empty address list\n", __func__, __LINE__);
    return false;
  }

  unspent_outputs_t* res = unspent_outputs_init();

  if (get_unspent_outputs(&w->endpoint, addrs, &res) == 0) {
    unspent_outputs_t *unspent, *tmp;
    HASH_ITER(hh, res, unspent, tmp) {
      // get the local status of this address
      unspent_outputs_t* elm = unspent_outputs_find(&w->unspent, unspent->addr);
      bool is_spent = false;
      if (elm) {
        is_spent = elm->spent;
      }
      unspent_outputs_update(&w->unspent, unspent->addr, unspent->ids);
      // mark the output as spent if we already marked it as spent locally
      unspent_outputs_set_spent(&w->unspent, unspent->addr, is_spent);
    }
  }
  ret = true;

end:
  addr_list_free(addrs);
  unspent_outputs_free(&res);

  return ret;
}

uint64_t wallet_balance(wallet_t* w) {
  wallet_refresh(w, false);
  return unspent_outputs_balance(&w->unspent);
}

int wallet_request_funds(wallet_t* w) {
  int ret = -1;
  byte_t receiver[TANGLE_ADDRESS_BYTES];
  res_get_funds_t res = {};
  wallet_receive_address(w, receiver);
  ret = get_funds(&w->endpoint, receiver, &res);
  printf("[%s:%d] message ID: %s\n", __func__, __LINE__, res.msg_id);
  return ret;
}

int wallet_unspent_outputs(wallet_t* w, send_funds_op_t* opt) { return -1; }