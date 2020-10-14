#ifndef __CORE_ADDRESS_H__
#define __CORE_ADDRESS_H__

#include <stdbool.h>

#include "core/types.h"
#include "sodium.h"
#include "utarray.h"

// the length of an address (digest length = 32 + version byte length = 1).
#define TANGLE_ADDRESS_BYTES 33
#define TANGLE_ADDRESS_BASE58_BUF 48
#define TANGLE_SEED_BYTES crypto_sign_ed25519_SEEDBYTES
#define TANGLE_SEED_BASE58_BUF 48

#define ED_PUBLIC_KEY_BYTES crypto_sign_ed25519_PUBLICKEYBYTES
#define ED_PRIVATE_KEY_BYTES crypto_sign_ed25519_SECRETKEYBYTES
#define ED_SIGNATURE_BYTES crypto_sign_ed25519_BYTES
#define ED_DIGEST_BYTES 32

// address signature version
typedef enum { ADDRESS_VER_ED25519 = 1, ADDRESS_VER_BLS = 2 } address_version_t;

typedef UT_array addr_list_t;

/**
 * @brief loops address list
 *
 */
#define ADDR_LIST_FOREACH(list, elm) \
  for (elm = (byte_t *)utarray_front(list); elm != NULL; elm = (byte_t *)utarray_next(list, elm))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets a random seed.
 *
 * @param[out] seed An output seed
 */
void random_seed(byte_t seed[]);

/**
 * @brief Gets a human readable version of the Seed (base58 encoded).
 *
 * @param[in] seed The seed in bytes
 * @param[out] str_buf The seed string
 * @return true
 * @return false
 */
bool seed_2_base58(byte_t const seed[], char str_buf[]);

/**
 * @brief Gets seed bytes from a human readable seed string.
 *
 * @param[in] str the seed string
 * @param[out] out_seed seed in bytes array
 * @return true
 * @return false
 */
bool seed_from_base58(char const str[], byte_t out_seed[]);

/**
 * @brief Gets the address from corresponding seed and index
 *
 * @param[in] seed The seed for genrate address
 * @param[in] index The index of address
 * @param[in] version The address signature version
 * @param[out] addr_out An address
 */
void address_get(byte_t seed[], uint64_t index, address_version_t version, byte_t addr_out[]);

/**
 * @brief Gets a human readable version of the address (base58 encoded).
 *
 * @param[in] address An address in bytes
 * @param[out] str_buf A buffer holds string address
 * @return true on success
 * @return false on failed
 */
bool address_2_base58(byte_t const address[], char str_buf[]);

/**
 * @brief Converts base58 into IOTA address raw data.
 *
 * @param[in] base58_str A base58 string
 * @param[out] addr A buffer holds address
 * @return true on success
 * @return false on failed
 */
bool address_from_base58(char const base58_str[], byte_t addr[]);

/**
 * @brief Gets address keypair
 *
 * @param[in] seed The seed
 * @param[in] index The index of an address
 * @param[out] pub The public key of the address
 * @param[out] priv The private key of the address
 */
void address_ed25519_keypair(byte_t const seed[], uint64_t index, byte_t pub[], byte_t priv[]);

/**
 * @brief signs data/message and returns the signature.
 *
 * @param[in] seed The seed
 * @param[in] index The index of address
 * @param[in] data The message or data
 * @param[in] data_len The length of data
 * @param[out] signature The signed signature.
 */
void sign_signature(byte_t const seed[], uint64_t index, byte_t const data[], uint64_t data_len, byte_t signature[]);

/**
 * @brief Validates signature by given data and public key
 *
 * @param[in] signature The ed25519 signature
 * @param[in] data The data
 * @param[in] data_len The length of the data
 * @param[in] pub_key A public key
 * @return true Valid signature
 * @return false Invalid signature
 */
bool sign_verify_signature(byte_t signature[], byte_t const data[], size_t data_len, byte_t pub_key[]);

/**
 * @brief Allocates an address list object.
 *
 * @return addr_list_t* a pointer to addr_list_t object
 */
addr_list_t *addr_list_new();

/**
 * @brief Appends an address to the list.
 *
 * @param[in] list The address list
 * @param[in] addr An address to be appended to the list.
 */
static void addr_list_push(addr_list_t *list, byte_t const addr[]) { utarray_push_back(list, addr); }

/**
 * @brief Removes an address from tail.
 *
 * @param[in] list The address list
 */
static void addr_list_pop(addr_list_t *list) { utarray_pop_back(list); }

/**
 * @brief Gets address list size
 *
 * @param[in] list An addr_list_t object
 * @return size_t
 */
static size_t addr_list_len(addr_list_t *list) { return utarray_len(list); }

/**
 * @brief Gets an address from list by given index.
 *
 * @param[in] list An address list object
 * @param[in] index The index of the address
 * @return addr_list_t*
 */
static byte_t *addr_list_at(addr_list_t *list, size_t index) {
  // return NULL if not found.
  return (byte_t *)utarray_eltptr(list, index);
}

/**
 * @brief Frees an address list.
 *
 * @param[in] list An address list object.
 */
static void addr_list_free(addr_list_t *list) { utarray_free(list); }

void addr_list_print(addr_list_t *list);

#ifdef __cplusplus
}
#endif

#endif