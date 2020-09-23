#ifndef __UTILS_BIT_MASK_H__
#define __UTILS_BIT_MASK_H__

#include <stdint.h>
#include <stdbool.h>

#include "core/types.h"

/**
 * @brief It automatically increases the capacity with the bit index
 * 
 */
typedef struct{
  byte_t *byte;
  uint64_t cap;
}bit_mask_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a bit mask with the capacity of 8 bits.
 * 
 * @return bit_mask_t* Success, a pointer to the bit mask. NULL on failed.
 */
bit_mask_t* bit_mask_new();

/**
 * @brief Sets a bit from the given index
 * 
 * @param[in] mask A bit mask object
 * @param[in] index A given index
 * @param[in] v the value to set
 * @return true on success
 * @return false on failed
 */
bool bit_mask_set(bit_mask_t* mask, uint64_t index, bool v);

/**
 * @brief Gets the value of the given index
 * 
 * Need to make sure the index is not out of capacity
 * 
 * @param[in] mask A bit mask object
 * @param[in] index A given index
 * @return true bit value is 1
 * @return false bit value is 0, or out of range
 */
bool bit_mask_get(bit_mask_t* mask, uint64_t index);

/**
 * @brief Release a bit mask object
 * 
 * @param b A bit mask object.
 */
void bit_mask_free(bit_mask_t* b);


#ifdef __cplusplus
}
#endif

#endif