#ifndef __CORE_BALANCE_H__
#define __CORE_BALANCE_H__

#include <stdbool.h>
#include <stdint.h>

#include "core/types.h"
#include "utarray.h"

// Color represents a marker that is associated to a token balance and that gives it a certain "meaning". The zero value
// represents "vanilla" IOTA tokens but it is also possible to define tokens that represent i.e. real world assets.

#define BALANCE_COLOR_BYTES 32
#define BALANCE_COLOR_BASE58_BUF 48

// Balance represents a balance in the IOTA ledger. It consists out of a numeric value and a color.
typedef struct {
  int64_t value;
  byte_t color[BALANCE_COLOR_BYTES];
} balance_t;

typedef UT_array balance_list_t;

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
 * @param[out] balance The balance object
 * @param[in] color The color is optional, NULL for a default value.
 * @param[in] value The value
 */
void balance_init(balance_t *balance, byte_t color[], int64_t value);

/**
 * @brief Inits a balance object from the given bytes.
 *
 * @param[out] balance The balance object
 * @param[in] balance_bytes The balance bytes for serializing to the balance object
 */
void balance_from_bytes(balance_t *balance, byte_t balance_bytes[]);

/**
 * @brief Serialize balance object to bytes
 *
 * @param[out] balance_bytes A buffer holds bytes data
 * @param[in] balance The balance object to be serialized
 */
void balance_2_bytes(byte_t balance_bytes[], balance_t *balance);

/**
 * @brief Sets color to a balance object.
 *
 * @param[out] balance the balance object
 * @param[in] color The color bytes to be set
 */
void balance_set_color(balance_t *balance, byte_t color[]);

/**
 * @brief Gets a human readable color string (base58 encoded).
 *
 * @param[out] color_str A buffer holds color string
 * @param[in] color A color object in bytes
 * @return true
 * @return false
 */
bool balance_color_2_base58(char color_str[], byte_t color[]);

/**
 * @brief print out a balance object
 *
 * @param[in] balance A balance object.
 */
void print_balance(balance_t *balance);

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

#ifdef __cplusplus
}
#endif

#endif
