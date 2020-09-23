#include <stdlib.h>
#include <string.h>

#include "utils/bit_mask.h"

bit_mask_t* bit_mask_new() {
  bit_mask_t* b = malloc(sizeof(bit_mask_t));
  if (!b) {
    return NULL;
  }

  b->byte = calloc(0, 1);
  if (!b->byte) {
    free(b);
    return NULL;
  }
  b->cap = 1;
  return b;
}

void bit_mask_free(bit_mask_t* b) {
  if (b) {
    if (b->byte) {
      free(b->byte);
    }
    free(b);
  }
}

bool bit_mask_set(bit_mask_t* mask, uint64_t index, bool v) {
  uint64_t byte_index = index / 8;
  uint8_t bit_index = index % 8;

  if (byte_index > mask->cap) {
    // extand capacity
    byte_t* n = NULL;
    n = realloc(mask->byte, byte_index + 1);
    if (!n) {
      // realloc failed
      return false;
    }
    // update bit mask
    memset(n + mask->cap, 0, byte_index + 1);
    mask->byte = n;
    mask->cap = byte_index + 1;
  }
  return true;
}

bool bit_mask_get(bit_mask_t* mask, uint64_t index){
  uint64_t byte_index = index / 8;
  uint8_t bit_index = index % 8;
  if(byte_index > mask->cap){
    return false;
  }

  return mask->byte[byte_index] & (1 << bit_index);
}