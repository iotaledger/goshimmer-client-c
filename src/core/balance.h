#ifndef __CORE_BALANCE_H__
#define __CORE_BALANCE_H__

#include <stdbool.h>
#include <stdint.h>

#include "core/types.h"
#include "utarray.h"
#include "uthash.h"

// Color represents a marker that is associated to a token balance and that gives it a certain "meaning". The zero value
// represents "vanilla" IOTA tokens but it is also possible to define tokens that represent i.e. real world assets.

#define BALANCE_COLOR_BYTES 32
#define BALANCE_COLOR_BASE58_BUF 48

// Balance represents a balance in the IOTA ledger. It consists out of a numeric value and a color.
typedef struct {
  int64_t value;
  byte_t color[BALANCE_COLOR_BYTES];
} balance_t;

// balance list object
typedef UT_array balance_list_t;

// balance hash table object
typedef struct {
  byte_t color[BALANCE_COLOR_BYTES];
  int64_t value;
  UT_hash_handle hh;  // hash table handler
} balance_ht_t;

/**
 * @brief loops balance list
 *
 */
#define BALANCE_LIST_FOREACH(list, elm) \
  for (elm = (balance_t *)utarray_front(list); elm != NULL; elm = (balance_t *)utarray_next(list, elm))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inits a balance object by given value and color.
 *
 * @param[in] color The color is optional, NULL for a default value.
 * @param[in] value The value
 * @param[out] balance The balance object
 */
void balance_init(byte_t const color[], int64_t const value, balance_t *balance);

/**
 * @brief Inits a balance object from the given bytes.
 *
 * @param[in] balance_bytes The balance bytes for serializing to the balance object
 * @param[out] balance The balance object
 */
void balance_from_bytes(byte_t balance_bytes[], balance_t *balance);

/**
 * @brief Serialize balance object to bytes
 *
 * @param[in] balance The balance object to be serialized
 * @param[out] balance_bytes A buffer holds bytes data
 */
void balance_2_bytes(balance_t *balance, byte_t balance_bytes[]);

/**
 * @brief Sets color to a balance object.
 *
 * @param[in] color The color bytes to be set
 * @param[out] balance the balance object
 */
void balance_set_color(byte_t const color[], balance_t *balance);

/**
 * @brief Gets a human readable color string (base58 encoded).
 *
 * @param[in] color A color object in bytes
 * @param[out] color_str A buffer holds color string
 * @return true On success
 * @return false On failed
 */
bool balance_color_2_base58(byte_t color[], char color_str[]);

/**
 * @brief Gets a binary color from base58 encoded string.
 *
 * @param[in] color_str A based58 string
 * @param[out] color A buffer holds color binary
 * @return true On success
 * @return false On failed
 */
bool balance_color_from_base58(char color_str[], byte_t color[]);

/**
 * @brief Gets a random color
 *
 * @param[out] color A buffer holds binary color
 */
void balance_color_random(byte_t color[]);

/**
 * @brief print out a balance object
 *
 * @param[in] balance A balance object.
 */
void balance_print(balance_t *balance);

/**
 * @brief Duplicates a balance object
 *
 * @param[in] src A balance object
 * @return balance_t*
 */
balance_t *balance_clone(balance_t *src);

/**
 * @brief Allocates a balance list object.
 *
 * @return balance_list_t* a pointer to a balance list object
 */
balance_list_t *balance_list_new();

/**
 * @brief Appends an balance element to the list.
 *
 * @param[in] list The balance list
 * @param[in] balance A balance element.
 */
static void balance_list_push(balance_list_t *list, balance_t const *const balance) {
  utarray_push_back(list, balance);
}

/**
 * @brief Removes a balance element from tail.
 *
 * @param[in] list The balance list
 */
static void balance_list_pop(balance_list_t *list) { utarray_pop_back(list); }

/**
 * @brief Gets balance list size
 *
 * @param[in] list A balance_list_t object
 * @return size_t the length of list
 */
static size_t balance_list_len(balance_list_t *list) { return utarray_len(list); }

/**
 * @brief Gets a balance element from list by given index.
 *
 * @param[in] list An address list object
 * @param[in] index The index of the address
 * @return balance_list_t*
 */
static balance_t *balance_list_at(balance_list_t *list, size_t index) {
  // return NULL if not found.
  return (balance_t *)utarray_eltptr(list, index);
}

/**
 * @brief Frees a balance list.
 *
 * @param[in] list A balance list object.
 */
static void balance_list_free(balance_list_t *list) { utarray_free(list); }

/**
 * @brief print out a colored balance list
 *
 * @param[in] list A balance list
 */
void balance_list_print(balance_list_t *list);

/**
 * @brief Duplicates a balance list
 *
 * @param[in] src A balance list object
 * @return balance_list_t*
 */
balance_list_t *balance_list_clone(balance_list_t *src);

/**
 * @brief Initializes a balance hash table.
 *
 * @return balance_ht_t* a NULL pointer
 */
static balance_ht_t *balance_ht_init() { return NULL; }

/**
 * @brief Adds a colored balance to the table
 *
 * @param[in] t A colored balance hash table
 * @param[in] color The color in bytes
 * @param[in] value The value of the balance
 * @return int o on success
 */
int balance_ht_add(balance_ht_t **t, byte_t const color[], int64_t value);

/**
 * @brief Finds a balance by the given color
 *
 * @param[in] t A colored balance hash table
 * @param[in] color The color to find
 * @return balance_ht_t* A point to a balance
 */
static balance_ht_t *balance_ht_find(balance_ht_t **t, byte_t const color[]) {
  balance_ht_t *b;
  HASH_FIND(hh, *t, color, BALANCE_COLOR_BYTES, b);
  return b;
};

/**
 * @brief Removes and frees a balance by the given color
 *
 * @param[in] t A colored balance hash table
 * @param[in] color The color for remove
 */
static void balance_ht_remove(balance_ht_t **t, byte_t const color[]) {
  balance_ht_t *elm = balance_ht_find(t, color);
  HASH_DEL(*t, elm);
  free(elm);
}

/**
 * @brief The size of the balance hash table
 *
 * @param[in] t A colored balance hash table
 * @return size_t The size of the table
 */
static size_t balance_ht_count(balance_ht_t **t) { return HASH_COUNT(*t); }

/**
 * @brief Frees the colored balance hash table
 *
 * @param[in] t A colored balance hash table
 */
static void balance_ht_free(balance_ht_t **t) {
  balance_ht_t *curr_elm, *tmp;
  HASH_ITER(hh, *t, curr_elm, tmp) {
    HASH_DEL(*t, curr_elm);
    free(curr_elm);
  }
}

/**
 * @brief print out a colored balance hash table
 *
 * @param[in] t A balance hash table
 */
void balance_ht_print(balance_ht_t **t);

/**
 * @brief Duplicates a balance hash table
 *
 * @param[in] t A balance hash table
 * @return balance_ht_t* A point to the new table
 */
balance_ht_t *balance_ht_clone(balance_ht_t **t);

#ifdef __cplusplus
}
#endif

#endif
