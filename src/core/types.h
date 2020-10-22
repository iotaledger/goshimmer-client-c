#ifndef __CORE_TYPES_H__
#define __CORE_TYPES_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef uint8_t byte_t;

/**
 * @brief Checks the endian type of this host
 *
 * @return true Host is little endian
 * @return false Host is big endian
 */
static inline bool is_little_endian() { return (1 == *(unsigned char *)&(const int){1}); }

/**
 * @brief Prints out hexmal value in a byte array.
 *
 * @param[in] data A byte_t buffer
 * @param[in] len The length of data
 */
static inline void dump_hex(byte_t const data[], size_t len) {
  for (int i = 0; i < len; i++) {
    printf("0x%x, ", data[i]);
  }
  printf("\n");
}

/**
 * @brief Check is the byte array is empty
 *
 * @param[in] data A pointer to the byte array
 * @param[in] len The length of this array
 * @return true An empty array
 * @return false Array with values
 */
static inline bool empty_byte_array(byte_t const data[], size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (data[i] != 0x0) {
      return false;
    }
  }
  return true;
}

#endif