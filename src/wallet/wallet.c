#include <stdio.h>

#include "client/api/get_node_info.h"
#include "client/api/get_unspent_outputs.h"
#include "wallet/wallet.h"

wallet_t* wallet_init(char const url[], uint16_t port, byte_t const seed[]) {
  wallet_t* ctx = malloc(sizeof(wallet_t));
  if (ctx == NULL) {
    printf("[%s %d] OOM\n", __func__, __LINE__);
    return NULL;
  }

  // address manager, we should update address status later.
  ctx->addr_manager = am_new(seed, 0, NULL);
  if (ctx->addr_manager == NULL) {
    goto err;
  }

  // client endpoint
  strcpy(ctx->endpoint.url, url);
  ctx->endpoint.port = port;

  // unspent output manager
  ctx->unspent = unspent_outputs_init();

  // sync with node
  // if(wallet_refresh(ctx, true) == false){
  //   printf("[%s:%d] wallet status update failed\n", __func__, __LINE__);
  // }

  return ctx;

err:
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

// bool wallet_refresh(wallet_t* w, bool include_spent) {
//   bool ret = false;
//   addr_list_t* addrs = NULL;
//   bool has_ids = false;
//   if (include_spent) {
//     addrs = wallet_addresses(w);
//   } else {
//     addrs = wallet_unspent_addresses(w);
//   }
//
//   if (addrs == NULL || addr_list_len(addrs) == 0) {
//     printf("[%s:%d] empty address list\n", __func__, __LINE__);
//     return false;
//   }
//
//   res_unspent_outs_t* res = unspent_list_new();
//   if (res == NULL) {
//     ret = false;
//     goto end;
//   }
//
//   if (get_unspent_outputs(&w->endpoint, addrs, res) == 0) {
//     res_unspent_t* elm = NULL;
//     byte_t ed_addr[TANGLE_ADDRESS_BYTES] = {};
//     byte_t out_id[TX_OUTPUT_ID_BYTES] = {};
//     UNSPENT_OUTS_FOREACH(res, elm) {
//       has_ids = false;
//       // get address
//       address_from_base58(elm->address, ed_addr);
//
//       // parse output ids
//       if (output_id_list_len(elm->output_ids) != 0) {
//         has_ids = true;
//         output_id_t* id = NULL;
//         OUTPUT_ID_FOREACH(elm->output_ids, id) {
//           // trying to add output IDs
//           tx_output_id_from_base58(id->output_id, strlen(id->output_id), out_id);
//           outputs_manager_add(&w->outputs, out_id);
//           // balances
//
//         }
//       }
//
//       // check if address exist in address manager
//       byte_t* addr = NULL;
//       ADDR_LIST_FOREACH(addrs, addr) {
//         if (memcmp(addr, ed_addr, TANGLE_ADDRESS_BYTES) == 0) {
//           //
//         } else {
//           // adds address to am
//         }
//       }
//     }
//   }
//
// end:
//   addr_list_free(addrs);
//   unspent_list_free(res);
//
//   return ret;
// }

int wallet_unspent_outputs(wallet_t* w, send_funds_op_t* opt) { return -1; }