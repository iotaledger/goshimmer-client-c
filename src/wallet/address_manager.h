#ifndef __WALLET_AM_H__
#define __WALLET_AM_H__

#include <stdbool.h>

#include "core/address.h"
#include "utils/bitmask.h"

typedef struct {
  byte_t seed[IOTA_SEED_BYTES];
  uint64_t last_addr_index;
  bitmask_t* spent_addr;
  uint64_t first_unspent_idx;
  uint64_t last_unspent_idx;
} wallet_am_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief News a wallet manager instance
 *
 * @return wallet_am_t*
 */
wallet_am_t* am_new();

/**
 * @brief Gets address from a given index, the last_addr_index and last_unspent_idx might got updated.
 *
 * @param[in] am A wallet manager instance
 * @param[in] index The address index
 * @param[out] out_addr The output address
 */
void am_get_address(wallet_am_t* const am, uint64_t index, wallet_address_t* const out_addr);

/**
 * @brief Generates and retruns a new unused address.
 *
 * @param[in] am A wallet manager instance
 * @param[in] out_addr The output address
 */
void am_get_new_address(wallet_am_t* const am, wallet_address_t* const out_addr);

/**
 * @brief Returns True if the index of the address was spent already.
 *
 * @param[in] am A wallet manager instance
 * @param[in] index The address index wanna check
 * @return true
 * @return false
 */
bool am_is_spent_address(wallet_am_t const* const am, uint64_t index);

/**
 * @brief Marks the given address as spent
 *
 * @param[in] am A wallet manager instance
 * @param index
 */
void am_mark_spent_address(wallet_am_t* const am, uint64_t index);

/**
 * @brief Gets first unspent address
 *
 * @param[in] am A wallet manager instance
 * @param[out] addr The first unspent address
 */
void am_get_first_unspent_address(wallet_am_t* const am, wallet_address_t* addr);

/**
 * @brief Gets last unspent address
 *
 * @param[in] am A wallet manager instance
 * @param[out] addr The last unspent address
 */
void am_get_last_unspent_address(wallet_am_t* const am, wallet_address_t* addr);

#ifdef __cplusplus
}
#endif

#endif