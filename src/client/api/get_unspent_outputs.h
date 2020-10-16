#ifndef __CLIENT_API_UNSPENT_OUTPUTS_H__
#define __CLIENT_API_UNSPENT_OUTPUTS_H__

#include <stdbool.h>

#include "client/client_service.h"
#include "core/transaction.h"

typedef addr_list_t req_unspent_outs_t;

// represents the different states of an OutputID
typedef struct {
  bool solid;
  bool confirmed;
  bool rejected;
  bool liked;
  bool conflicting;
  bool finalized;
  bool preferred;
} inclusion_state_t;

typedef struct {
  byte_t output_id[TX_OUTPUT_ID_BASE58_BUF];
  balance_list_t *balances;
  inclusion_state_t st;
} output_id_t;

typedef UT_array output_id_list_t;

#define OUTPUT_ID_FOREACH(list, id) \
  for (id = (output_id_t *)utarray_front(list); id != NULL; id = (output_id_t *)utarray_next(list, id))

typedef struct {
  byte_t address[TANGLE_ADDRESS_BASE58_BUF];
  output_id_list_t *output_ids;
} res_unspent_t;

typedef UT_array res_unspent_outs_t;

#define UNSPENT_OUTS_FOREACH(list, out) \
  for (out = (res_unspent_t *)utarray_front(list); out != NULL; out = (res_unspent_t *)utarray_next(list, out))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The unspent output API
 *
 * @param[in] conf The client endpoint configuration
 * @param[in] req A list of addresses
 * @param[out] res A list of unspent outputs
 * @return int 0 on success
 */
int get_unspent_outputs(tangle_client_conf_t const *conf, req_unspent_outs_t *req, res_unspent_outs_t *res);

/**
 * @brief Unspent output deserialization
 *
 * @param[in] j_str A string of json object
 * @param[out] res A response of the unspent output API
 * @return int 0 on success
 */
int deser_unspent_outputs(char const *const j_str, res_unspent_outs_t *res);

/**
 * @brief Allocates output id list object
 *
 * @return output_id_list_t* A pointer to the object.
 */
output_id_list_t *output_id_list_new();

/**
 * @brief Duplicates an output id list
 *
 * @param[in] src An output id list for clone
 * @return output_id_list_t* A new output id list object
 */
output_id_list_t *output_id_list_clone(output_id_list_t *src);

/**
 * @brief Appends an id to list.
 *
 * @param[in] list The output id list object.
 * @param[in] id An id object.
 */
static void output_id_list_push(output_id_list_t *list, output_id_t const *const id) { utarray_push_back(list, id); }

/**
 * @brief Gets length of output id list.
 *
 * @param[in] list An output id list object.
 * @return size_t
 */
static size_t output_id_list_len(output_id_list_t *list) { return utarray_len(list); }

/**
 * @brief Frees an output id list
 *
 * @param[in] list
 */
static void output_id_list_free(output_id_list_t *list) { utarray_free(list); }

/**
 * @brief Allocates unspent list object
 *
 * @return res_unspent_outs_t* A pointer to the object.
 */
res_unspent_outs_t *unspent_list_new();

/**
 * @brief Duplicates an unspent output list
 *
 * @param[in] src An unspent list for clone
 * @return res_unspent_outs_t* A new unspent list object
 */
// res_unspent_outs_t *unspent_list_clone(res_unspent_outs_t *src);

/**
 * @brief Appends an unspent output to list.
 *
 * @param[in] list The unspent output list object.
 * @param[in] unspent An id object.
 */
static void unspent_list_push(res_unspent_outs_t *list, res_unspent_t const *const unspent) {
  utarray_push_back(list, unspent);
}

/**
 * @brief Gets length of unspent output list.
 *
 * @param[in] list An unspent output list object.
 * @return size_t
 */
static size_t unspent_list_len(res_unspent_outs_t *list) { return utarray_len(list); }

/**
 * @brief Frees an unspent output list
 *
 * @param[in] list
 */
static void unspent_list_free(res_unspent_outs_t *list) { utarray_free(list); }

/**
 * @brief Shows unspent outputs
 *
 * @param[in] list A unspent output list
 */
void unspent_list_print(res_unspent_outs_t *list);

#ifdef __cplusplus
}
#endif

#endif
