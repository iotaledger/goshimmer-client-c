#ifndef __CORE_TRANSATION_H__
#define __CORE_TRANSATION_H__

#include <stdint.h>

#include "core/address.h"
#include "core/balance.h"
#include "core/types.h"
#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRANSACTION_ID_BYTES 32
#define TRANSACTION_ID_BASE58_BUF 48
// OutputID is the data type that represents the identifier for an Output.
#define OUTPUT_ID_BYTES (TANGLE_ADDRESS_BYTES + TRANSACTION_ID_BYTES)
#define OUTPUT_ID_BASE58_LEN 96

/**
 * @brief A transaction output object
 *
 */
typedef struct {
  byte_t address[TANGLE_ADDRESS_BYTES];
  balance_t balance;
} tx_output_t;

// Inputs represents a list of referenced Outputs that are used as Inputs in a transaction
typedef UT_array tx_inputs_t;
// Outputs represents a list of Outputs that are part of a transaction.
typedef UT_array tx_outputs_t;

typedef struct {
  tx_inputs_t *inputs;
  tx_outputs_t *outputs;
  // TODO: signatures
  // TODO: payload
} transaction_t;

/**
 * @brief Gets a random transaction id.
 *
 * @param[out] id A transaction id
 */
void tx_id_random(byte_t id[]);

/**
 * @brief Gets a human readable version of the id (base58 encoded).
 *
 * @param[out] str_buf The id string
 * @param[in, out] buf_len The length of string
 * @param[in] id The id in bytes
 * @return true
 * @return false
 */
bool tx_id_2_base58(char str_buf[], size_t *buf_len, byte_t id[]);

/**
 * @brief Gets a random output id from a given address
 *
 * @param[out] output_id A buffer holds output id
 * @param[in] addr An address in bytes
 */
void tx_output_id_random(byte_t output_id[], byte_t addr[]);

/**
 * @brief Gets an output id from given address and id.
 *
 * @param[out] output_id A buffer holds output id
 * @param[in] addr An address in bytes
 * @param[in] id An id in bytes
 */
void tx_output_id(byte_t output_id[], byte_t addr[], byte_t id[]);

/**
 * @brief Gets a human readable output id (base58 encoded).
 *
 * @param[out] str_buf A buffer holds output id string
 * @param[in, out] buf_len The length of buffer
 * @param[in] output_id An output id
 * @return true
 * @return false
 */
bool tx_output_id_2_base58(char str_buf[], size_t *buf_len, byte_t output_id[]);

/**
 * @brief Allocates a transaction input list object.
 *
 * @return tx_inputs_t* a pointer to tx_inputs_t object
 */
tx_inputs_t *tx_inputs_new();

/**
 * @brief Appends an output id to the list.
 *
 * @param[in] tx_in The list object
 * @param[in] output_id An output id to be appended to the list.
 */
static void tx_inputs_push(tx_inputs_t *tx_in, byte_t output_id[]) { utarray_push_back(tx_in, output_id); }

/**
 * @brief Gets input list size
 *
 * @param[in] tx_in An input list object
 * @return size_t
 */
static size_t tx_inputs_len(tx_inputs_t *tx_in) { return utarray_len(tx_in); }

/**
 * @brief Frees an input list.
 *
 * @param[in] tx_in An input list object.
 */
static void tx_inputs_free(tx_inputs_t *tx_in) { utarray_free(tx_in); }

/**
 * @brief Prints an input list.
 *
 * @param[in] tx_in An input list object.
 */
void tx_inputs_print(tx_inputs_t *tx_in);

/**
 * @brief Gets output id by index from an input list.
 *
 * @param[in] tx_in The input list object
 * @param[in] index The index of output id
 * @return byte_t* A pointer to the output id, NULL if not found.
 */
static byte_t *tx_inputs_at(tx_inputs_t *tx_in, size_t index) {
  // return NULL if not found.
  return (byte_t *)utarray_eltptr(tx_in, index);
}

/**
 * @brief loops transaction input list
 *
 */
#define TX_INPUTS_FOREACH(tx_in, output_id)                           \
  for (output_id = (byte_t *)utarray_front(tx_in); output_id != NULL; \
       output_id = (byte_t *)utarray_next(tx_in, output_id))

/**
 * @brief Allocates transaction outputs object
 *
 * @return tx_outputs_t* A pointer to the object.
 */
tx_outputs_t *tx_outputs_new();

/**
 * @brief Appends an output object to list.
 *
 * @param[in] tx_out The output list object.
 * @param[in] output An output object.
 */
static void tx_outputs_push(tx_outputs_t *tx_out, tx_output_t const *const output) {
  utarray_push_back(tx_out, output);
}

/**
 * @brief Gets length of transaction output list.
 *
 * @param[in] tx_out A transaction output list object.
 * @return size_t
 */
static size_t tx_outputs_len(tx_outputs_t *tx_out) { return utarray_len(tx_out); }

/**
 * @brief Frees a transaction output list
 *
 * @param[in] tx_out
 */
static void tx_outputs_free(tx_outputs_t *tx_out) { utarray_free(tx_out); }

/**
 * @brief Gets an output object from list by given index.
 *
 * @param[in] tx_out A transaction output list object
 * @param[in] index The index of an output object
 * @return tx_output_t*
 */
static tx_output_t *tx_outputs_at(tx_outputs_t *tx_out, size_t index) {
  // return NULL if not found.
  return (tx_output_t *)utarray_eltptr(tx_out, index);
}

/**
 * @brief Prints an output list object.
 *
 * @param[in] tx_out An output list.
 */
void tx_outputs_print(tx_outputs_t *tx_out);

/**
 * @brief loops a transaction output list
 *
 */
#define TX_OUTPUTS_FOREACH(tx_out, o) \
  for (o = (tx_output_t *)utarray_front(tx_out); o != NULL; o = (tx_output_t *)utarray_next(tx_out, o))

#ifdef __cplusplus
}
#endif

#endif