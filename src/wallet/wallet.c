#include <stdio.h>

#include "client/api/get_node_info.h"
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
