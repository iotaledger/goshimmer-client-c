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

/**
 * @brief creates a faucet request (0-value) message with the given destination address and broadcasts it to the node's
 * neighbors. It returns the message ID if successful.
 *
 * @param[in] conf A client instance
 * @param[in] addr A destination address
 * @param[out] res A response object
 * @return int 0 on success
 */
int get_funds(tangle_client_conf_t const *conf, byte_t const addr[], res_get_funds_t *res);

/**
 * @brief Response deserialization
 *
 * @param[in] j_str A string of JSON object
 * @param[out] res A response object
 * @return int 0 on success
 */
int deser_get_funds(char const *const j_str, res_get_funds_t *res);

#ifdef __cplusplus
}
#endif

#endif
