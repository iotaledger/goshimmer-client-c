#ifndef __UTILS_BITMASK_H__
#define __UTILS_BITMASK_H__

#include <stdbool.h>
#include <stdint.h>

#include "core/types.h"

/**
 * @brief Bitmask object
 *
 * It automatically increases the capacity with the bit index
 *
 */
typedef struct {
  byte_t* byte;
  uint64_t cap;
} bitmask_t;

/**
 * @brief Bitmask opreations
 *
 */
typedef enum { BITMASK_CLEAR = 0, BITMASK_SET, BITMASK_TOGGLE } bitmask_op_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a bitmask with the capacity of 8 bits.
 *
 * @return bitmask_t* Success, a pointer to the bit mask. NULL on failed.
 */
bitmask_t* bitmask_new();

/**
 * @brief bit operation
 *
 * @param[in] mask A bit mask object
 * @param[in] index The index of a bit
 * @param[in] op Bit operation
 * @return int 0 on success, -1 on failed
 */
int bitmask_op(bitmask_t* mask, uint64_t index, bitmask_op_t op);

/**
 * @brief Gets the value of the given index
 *
 * Need to make sure the index is not out of capacity
 *
 * @param[in] mask A bitmask object
 * @param[in] index A given index
 * @return true bit value is 1
 * @return false bit value is 0, or out of range
 */
bool bitmask_get(bitmask_t* mask, uint64_t index);

/**
 * @brief Copy a bitmask to a new object.
 *
 * @param b A bitmask will be copied
 * @return bitmask_t* A copied bitmask
 */
bitmask_t* bitmask_clone(bitmask_t* b);

/**
 * @brief Release a bitmask object
 *
 * @param b A bitmask object.
 */
void bitmask_free(bitmask_t* b);

/**
 * @brief prints out bitmask
 *
 * @param b A bitmask object
 */
void bitmask_show(bitmask_t* b);

#ifdef __cplusplus
}
#endif

#endif