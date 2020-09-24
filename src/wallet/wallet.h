#ifndef __WALLET_WALLET_H__
#define __WALLET_WALLET_H__

#include <stdbool.h>

#include "client/client_service.h"

typedef struct {
  iota_client_conf_t endpoint;
  // wallet_am_t addr_manager;
  // wallet_ar_t asset_reg;
  // wallet_om_t unspent_manager;
} wallet_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif