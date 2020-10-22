#ifndef __CORE_UNSPENT_OUTPUTS_H__
#define __CORE_UNSPENT_OUTPUTS_H__

#include <stdbool.h>
#include <stdint.h>

#include "core/address.h"
#include "core/output_ids.h"
#include "uthash.h"

typedef struct {
  uint64_t addr_index;
  byte_t addr[TANGLE_ADDRESS_BYTES];
  bool spent;
  output_ids_t *ids;
  UT_hash_handle hh;
} unspent_outputs_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes an unspent output hash table.
 *
 * @return unspent_outputs_t* a NULL pointer
 */
static unspent_outputs_t *unspent_outputs_init() { return NULL; }

/**
 * @brief Adds an element to the table
 *
 * @param[in] t An unspent output table
 * @param[in] addr An address
 * @param[in] addr_index The index of the address
 * @param[in] ids An output id table
 * @return int 0 on success
 */
int unspent_outputs_add(unspent_outputs_t **t, byte_t const addr[], uint64_t addr_index, output_ids_t *ids);

/**
 * @brief Updates/Replaces an element in the table
 *
 * @param[in] t An unspent output table
 * @param[in] addr An address
 * @param[in] ids An output id table
 * @return int 0 on success
 */
int unspent_outputs_update(unspent_outputs_t **t, byte_t const addr[], output_ids_t *ids);

/**
 * @brief Appends/Creates an element in the table
 *
 * @param[in] t An unspent output table
 * @param[in] addr An address
 * @param[in] ids An output id table
 * @return int 0 on success
 */
int unspent_outputs_append_id(unspent_outputs_t **t, byte_t const addr[], output_ids_t *ids);

/**
 * @brief Duplicates an unspent output hash table
 *
 * @param[in] t An unspent output hash table
 * @return unspent_outputs_t* A pointer to a new unspent output table
 */
unspent_outputs_t *unspent_outputs_clone(unspent_outputs_t **t);

/**
 * @brief Checks if a given address exists in the table
 *
 * @param[in] t An unspent output hash table
 * @param[in] addr The address to find
 * @return unspent_outputs_t* A point to an element
 */
static unspent_outputs_t *unspent_outputs_find(unspent_outputs_t **t, byte_t const addr[]) {
  unspent_outputs_t *b = NULL;
  HASH_FIND(hh, *t, addr, TANGLE_ADDRESS_BYTES, b);
  return b;
};

/**
 * @brief Removes and frees an element in the table
 *
 * @param[in] t An unspent output hash table
 * @param[in] addr The address to remove
 */
static void unspent_outputs_remove(unspent_outputs_t **t, byte_t const addr[]) {
  unspent_outputs_t *elm = unspent_outputs_find(t, addr);
  if (elm) {
    output_ids_free(&elm->ids);
    HASH_DEL(*t, elm);
    free(elm);
  }
}

/**
 * @brief The size of the hash table
 *
 * @param[in] t An unspent output hash table
 * @return size_t The size of the table
 */
static size_t unspent_outputs_count(unspent_outputs_t **t) { return HASH_COUNT(*t); }

/**
 * @brief frees the unspent output hash table
 *
 * @param[in] t An unspent output hash table
 */
static void unspent_outputs_free(unspent_outputs_t **t) {
  unspent_outputs_t *curr_elm, *tmp;
  HASH_ITER(hh, *t, curr_elm, tmp) {
    output_ids_free(&curr_elm->ids);
    HASH_DEL(*t, curr_elm);
    free(curr_elm);
  }
}

/**
 * @brief Sets the local status of this unspent outputs
 *
 *
 * @param[in] t An unspent output hash table
 * @param[in] addr An address
 * @param[in] spent True as spent, False as unspent
 */
void unspent_outputs_set_spent(unspent_outputs_t **t, byte_t addr[], bool spent);

/**
 * @brief Gets the local status of this unspent output
 *
 * @param[in] t An unspent output hash table
 * @param[in] addr An address
 * @return true Spent address
 * @return false Unspent address
 */
bool unspent_outputs_get_spent(unspent_outputs_t **t, byte_t addr[]);

/**
 * @brief Calculates confirmed balances
 *
 * @param[in] t An unspent output hash table
 * @return uint64_t The sum of confirmed balances
 */
uint64_t unspent_outputs_balance(unspent_outputs_t **t);

/**
 * @brief Calculates confirmed balances from given color
 *
 * @param[in] t An unspent output hash table
 * @param[]in color A specific color
 * @return uint64_t The sum of confirmed balances
 */
uint64_t unspent_outputs_balance_with_color(unspent_outputs_t **t, byte_t color[]);

/**
 * @brief Gets an outputs table that sufficient to the given balance
 *
 * @param[in] t An unspent output hash table
 * @param[in] required_balance A required balance
 * @param[in] color A required color
 * @return unspent_outputs_t*
 */
unspent_outputs_t *unspent_outputs_required_outputs(unspent_outputs_t **t, uint64_t required_balance, byte_t color[]);

/**
 * @brief print out an unspent output hash table
 *
 * @param[in] t An unspent output hash table
 */
void unspent_outputs_print(unspent_outputs_t **t);

#ifdef __cplusplus
}
#endif

#endif
