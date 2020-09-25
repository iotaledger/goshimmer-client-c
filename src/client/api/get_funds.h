#ifndef __CLIENT_API_FUNDS_H__
#define __CLIENT_API_FUNDS_H__

#include <stdbool.h>
#include <stdint.h>

#include "client/api/response_error.h"
#include "client/client_service.h"
#include "core/address.h"

#define TANGLE_MGS_ID_BYTES 64
#define TANGLE_MGS_ID_BASE58_BUF 96

typedef struct {
  char msg_id[TANGLE_MGS_ID_BASE58_BUF];
} res_get_funds_t;

#ifdef __cplusplus
extern "C" {
#endif

int get_funds(iota_client_conf_t const *conf, char const addr[], res_get_funds_t *res);

int deser_get_funds(char const *const j_str, res_get_funds_t *res);

#ifdef __cplusplus
}
#endif

#endif
