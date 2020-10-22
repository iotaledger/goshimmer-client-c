#ifndef __CORE_OUTPUT_IDS_H__
#define __CORE_OUTPUT_IDS_H__

#include "core/balance.h"
#include "core/transaction.h"
#include "core/types.h"
#include "uthash.h"

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

/**
 * @brief Output IDs object
 *
 * A hash table uses the transaction id as the key
 *
 */
typedef struct {
  byte_t id[TX_ID_BYTES];
  balance_ht_t *balances;
  inclusion_state_t st;
  UT_hash_handle hh;
} output_ids_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes an output id hash table.
 *
 * @return output_ids_t* a NULL pointer
 */
static output_ids_t *output_ids_init() { return NULL; }

/**
 * @brief Adds an element to the table
 *
 * @param[in] t An output id table
 * @param[in] id A transaction id
 * @param[in] balances A balance table
 * @param[in] st The inclusion status
 * @return int 0 on success
 */
int output_ids_add(output_ids_t **t, byte_t const id[], balance_ht_t *balances, inclusion_state_t *st);

/**
 * @brief Updates/Replances an element in the table
 *
 * @param[in] t An output id table
 * @param[in] id A transaction id
 * @param[in] balances A balance table
 * @param[in] st The inclusion status
 * @return int 0 on success
 */
int output_ids_update(output_ids_t **t, byte_t const id[], balance_ht_t *balances, inclusion_state_t *st);

/**
 * @brief Duplicates an output id hash table
 *
 * @param[in] t An output id hash table
 * @return output_ids_t* A pointer to a new output id table
 */
output_ids_t *output_ids_clone(output_ids_t **t);

/**
 * @brief Checks if a given transaction id exists in the table
 *
 * @param[in] t An output id hash table
 * @param[in] id The tx id to find
 * @return output_ids_t* A point to an element
 */
static output_ids_t *output_ids_find(output_ids_t **t, byte_t const id[]) {
  output_ids_t *b = NULL;
  HASH_FIND(hh, *t, id, TX_ID_BYTES, b);
  return b;
};

/**
 * @brief Removes and frees an element in the table
 *
 * @param[in] t An output id hash table
 * @param[in] id The tx id to remove
 */
static void output_ids_remove(output_ids_t **t, byte_t const id[]) {
  output_ids_t *elm = output_ids_find(t, id);
  if (elm) {
    balance_ht_free(&elm->balances);
    HASH_DEL(*t, elm);
    free(elm);
  }
}

/**
 * @brief The size of the hash table
 *
 * @param[in] t An output id hash table
 * @return size_t The size of the table
 */
static size_t output_ids_count(output_ids_t **t) { return HASH_COUNT(*t); }

/**
 * @brief frees the output id hash table
 *
 * @param[in] t An output id hash table
 */
static void output_ids_free(output_ids_t **t) {
  output_ids_t *curr_elm, *tmp;
  HASH_ITER(hh, *t, curr_elm, tmp) {
    balance_ht_free(&curr_elm->balances);
    HASH_DEL(*t, curr_elm);
    free(curr_elm);
  }
}

/**
 * @brief Calculates confirmed balances from outputs
 *
 * @param[in] t An output id hash table
 * @return uint64_t The sum of confirmed balances.
 */
uint64_t output_ids_balance(output_ids_t **t);

/**
 * @brief Calculates confirmed balances with the given color
 *
 * @param[in] t An output id hash table
 * @param[in] color A specific color
 * @return uint64_t The sum of confirmed balances.
 */
uint64_t output_ids_balance_with_color(output_ids_t **t, byte_t color[]);

// uint64_t output_ids_balance(output_ids_t **t);

/**
 * @brief print out an output id hash table
 *
 * @param[in] t An output id hash table
 */
void output_ids_print(output_ids_t **t);

/**
 * @brief Prints out inclustion state object
 *
 * @param[in] st An inclustion state object
 */
void inclustion_state_print(inclusion_state_t *st);

#ifdef __cplusplus
}
#endif

#endif
