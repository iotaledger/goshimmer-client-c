#ifndef __CLIENT_API_SEND_TX_H__
#define __CLIENT_API_SEND_TX_H__

#include "client/api/response_error.h"
#include "client/client_service.h"
#include "core/transaction.h"

typedef struct {
  char tx_id[TRANSACTION_ID_BASE58_BUF];
} res_send_tx_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sends a new transaction to the tangle.
 *
 * @param[in] conf A client instance
 * @param[in] tx_bytes A transaction in bytes
 * @param[out] res A transaction id from response
 * @return int 0 on success
 */
int send_tx_bytes(tangle_client_conf_t const *conf, byte_t const tx_bytes[], res_send_tx_t *res);

/**
 * @brief Response deserialization
 *
 * @param[in] j_str A string of JSON object
 * @param[out] res A response object
 * @return int 0 on success
 */
int deser_send_tx(char const *const j_str, res_send_tx_t *res);

#ifdef __cplusplus
}
#endif

#endif
