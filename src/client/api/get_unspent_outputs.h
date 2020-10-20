#ifndef __CLIENT_API_UNSPENT_OUTPUTS_H__
#define __CLIENT_API_UNSPENT_OUTPUTS_H__

#include <stdbool.h>

#include "client/client_service.h"
#include "core/transaction.h"
#include "core/unspent_outputs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The unspent output API
 *
 * @param[in] conf The client endpoint configuration
 * @param[in] addr A list of addresses
 * @param[out] unspent An unspent outputs table
 * @return int 0 on success
 */
int get_unspent_outputs(tangle_client_conf_t const *conf, addr_list_t *addrs, unspent_outputs_t **unspent);

/**
 * @brief Unspent output deserialization
 *
 * @param[in] j_str A string of json object
 * @param[out] unspent An unspent outputs table
 * @return int 0 on success
 */
int deser_unspent_outputs(char const *const j_str, unspent_outputs_t **unspent);

#ifdef __cplusplus
}
#endif

#endif
