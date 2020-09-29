#ifndef __WALLET_AM_H__
#define __WALLET_AM_H__

#include <stdbool.h>

#include "core/address.h"
#include "utils/bitmask.h"

/**
 * @brief A wallet address represents an address in a wallet. It extends the normal address type with an index number
 * that was used to generate the address from its seed.
 *
 */
typedef struct {
  byte_t addr[TANGLE_ADDRESS_BYTES];
  uint64_t index;
} wallet_address_t;

typedef struct {
  byte_t seed[TANGLE_SEED_BYTES];
  uint64_t last_addr_index;
  bitmask_t* spent_addr;
  uint64_t first_unspent_idx;
  uint64_t last_unspent_idx;
} wallet_am_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates or initializes an address managerment
 *
 * @param[in] seed The seed of this wallet
 * @param[in] last_addr_index The last address of this wallet, 0 for new a wallet
 * @param[in] spent_addr The spent address bitmask, NULL for new wallet
 * @return wallet_am_t*
 */
wallet_am_t* am_new(byte_t const seed[], uint64_t last_addr_index, bitmask_t* spent_addr);

/**
 * @brief Frees an address managerment instance
 *
 * @param[in] am An address managerment instance
 */
void am_free(wallet_am_t* am);

/**
 * @brief Gets address from a given index, the last_addr_index and last_unspent_idx might got updated.
 *
 * @param[in] am A wallet manager instance
 * @param[in] index The address index
 * @param[out] out_addr The output address
 */
void am_get_address(wallet_am_t* const am, uint64_t index, byte_t out_addr[]);

/**
 * @brief Generates and retruns a new unused address.
 *
 * @param[in] am A wallet manager instance
 * @param[in] out_addr The output address
 */
void am_get_new_address(wallet_am_t* const am, byte_t out_addr[]);

/**
 * @brief Returns True if the index of the address was spent already.
 *
 * @param[in] am A wallet manager instance
 * @param[in] index The address index wanna check
 * @return true A spent address
 * @return false An unspent address
 */
bool am_is_spent_address(wallet_am_t const* const am, uint64_t index);

/**
 * @brief Marks the given address as spent
 *
 * @param[in] am A wallet manager instance
 * @param[in] index The index of an address
 */
void am_mark_spent_address(wallet_am_t* const am, uint64_t index);

/**
 * @brief Gets first unspent address
 *
 * @param[in] am A wallet manager instance
 * @param[out] addr The first unspent address
 */
void am_get_first_unspent_address(wallet_am_t* const am, byte_t addr[]);

/**
 * @brief Gets last unspent address
 *
 * @param[in] am A wallet manager instance
 * @param[out] addr The last unspent address
 */
void am_get_last_unspent_address(wallet_am_t* const am, byte_t addr[]);

/**
 * @brief Gets a list of all addresses of the wallet.
 *
 * @param[in] am A wallet manager instance
 * @return addr_list_t*
 */
addr_list_t* am_addresses(wallet_am_t* const am);

/**
 * @brief Gets a list of all unspent addresses of the wallet.
 *
 * @param[in] am A wallet manager instance
 * @return addr_list_t*
 */
addr_list_t* am_unspent_addresses(wallet_am_t* const am);

/**
 * @brief Gets a list of all spent addresses of the wallet.
 *
 * @param[in] am A wallet manager instance
 * @return addr_list_t*
 */
addr_list_t* am_spent_addresses(wallet_am_t* const am);

#ifdef __cplusplus
}
#endif

#endif