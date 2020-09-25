#ifndef __CLIENT_API_INFO_H__
#define __CLIENT_API_INFO_H__

#include <stdbool.h>
#include <stdint.h>

#include "client/api/response_error.h"
#include "client/client_service.h"

typedef struct {
  char version[32];
  char id[64];
  bool is_synced;
} res_node_info_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets info API
 *
 * @param[in] conf The client endpoint configuration
 * @param[out] res A response object of node info
 * @return int 0 on success
 */
int get_node_info(tangle_client_conf_t const *conf, res_node_info_t *res);

/**
 * @brief node info JSON deserialization
 *
 * @param[in] j_str A string of json object
 * @param[out] res A response object of node info
 * @return int 0 on success
 */
int deser_node_info(char const *const j_str, res_node_info_t *res);

#ifdef __cplusplus
}
#endif

#endif
