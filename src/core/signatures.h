#ifndef __CORE_ED_SIGNATURES_H__
#define __CORE_ED_SIGNATURES_H__

#include <stdbool.h>
#include <stdint.h>

#include "core/address.h"
#include "core/types.h"
#include "uthash.h"

// Signatures represents a container for the address signatures of a value transfer.

typedef struct {
  byte_t address[TANGLE_ADDRESS_BYTES];  // key
  byte_t pub_key[ED_PUBLIC_KEY_BYTES];
  byte_t signature[ED_SIGNATURE_BYTES];
  UT_hash_handle hh;  // hash table handler
} ed_signature_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes an ed25519 signature hash table.
 *
 * @return signatures_list_t* a NULL pointer
 */
static ed_signature_t *ed_signatures_init() { return NULL; }

/**
 * @brief Adds an ed signature element to the table.
 *
 * @param[in] t An ed25519 signature hash table
 * @param[in] addr An address
 * @param[in] pub_key A public key
 * @param[in] sig A signature
 * @return int 0 on success
 */
int ed_signatures_add(ed_signature_t **t, byte_t const addr[], byte_t const pub_key[], byte_t const sig[]);

/**
 * @brief Finds an ed signature element by a given address
 *
 * @param[in] t An ed25519 signature hash table
 * @param[in] addr An address
 * @return ed_signature_t* A point to an ed signautre element
 */
static ed_signature_t *ed_signatures_find(ed_signature_t **t, byte_t const addr[]) {
  ed_signature_t *s;
  HASH_FIND(hh, *t, addr, TANGLE_ADDRESS_BYTES, s);
  return s;
};

/**
 * @brief Removes and frees an ed signature element by given address
 *
 * @param[in] t An ed25519 signature hash table
 * @param[in] addr An address
 */
static void ed_signatures_remove(ed_signature_t **t, byte_t addr[]) {
  ed_signature_t *elm = ed_signatures_find(t, addr);
  HASH_DEL(*t, elm);
  free(elm);
}

/**
 * @brief The size of the ed signature hash table
 *
 * @param[in] t An ed25519 signature hash table
 * @return size_t The size of table
 */
static size_t ed_signatures_count(ed_signature_t **t) { return HASH_COUNT(*t); }

/**
 * @brief Destories the ed signature hash table
 *
 * @param[in] t An ed25519 signature hash table
 */
static void ed_signatures_destory(ed_signature_t **t) {
  ed_signature_t *curr_elm, *tmp;
  HASH_ITER(hh, *t, curr_elm, tmp) {
    HASH_DEL(*t, curr_elm);
    free(curr_elm);
  }
}

/**
 * @brief print out a signatures object
 *
 * @param[in] signatures A signatures object.
 */
void ed_signatures_print(ed_signature_t **t);

#ifdef __cplusplus
}
#endif

#endif
