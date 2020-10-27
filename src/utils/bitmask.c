#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "utils/allocator.h"
#include "utils/bitmask.h"

bitmask_t* bitmask_new() {
  bitmask_t* b = malloc(sizeof(bitmask_t));
  if (!b) {
    printf("[%s:%d] malloc failed\n", __func__, __LINE__);
    return NULL;
  }

  /* Note: In esp-idf `calloc` is equivalent to `heap_caps_calloc`
  that we use `malloc` and `memset` instead of `calloc`. */
  // b->byte = calloc(0, 1);
  b->byte = malloc(1);
  if (!b->byte) {
    printf("[%s:%d] calloc failed\n", __func__, __LINE__);
    free(b);
    return NULL;
  }
  memset(b->byte, 0, sizeof(byte_t));
  b->cap = 1;
  return b;
}

void bitmask_free(bitmask_t* b) {
  if (b) {
    if (b->byte) {
      free(b->byte);
    }
    free(b);
  }
}

int bitmask_op(bitmask_t* mask, uint64_t index, bitmask_op_t op) {
  uint64_t byte_index = index / 8;
  uint8_t bit_index = index % 8;

  if (byte_index + 1 > mask->cap) {
    // extand capacity
    byte_t* n = NULL;
    n = realloc(mask->byte, byte_index + 1);
    if (!n) {
      // realloc failed
      return -1;
    }
    // clean bits in new allocated bytes
    memset(n + mask->cap, 0, (byte_index + 1) - mask->cap);
    mask->byte = n;
    mask->cap = byte_index + 1;
  }

  switch (op) {
    case BITMASK_CLEAR:
      mask->byte[byte_index] &= ~(1 << bit_index);
      break;
    case BITMASK_SET:
      mask->byte[byte_index] |= (1 << bit_index);
      break;
    case BITMASK_TOGGLE:
      mask->byte[byte_index] ^= (1 << bit_index);
      break;
    default:
      break;
  }
  return 0;
}

bool bitmask_get(bitmask_t* mask, uint64_t index) {
  uint64_t byte_index = index / 8;
  uint8_t bit_index = index % 8;
  if (byte_index > mask->cap) {
    printf("Err[%s:%d] out of range\n", __func__, __LINE__);
    return false;
  }
  return mask->byte[byte_index] & (1 << bit_index);
}

bitmask_t* bitmask_clone(bitmask_t* b) {
  bitmask_t* n = malloc(sizeof(bitmask_t));
  if (!n) {
    return NULL;
  }

  n->byte = malloc(b->cap);
  if (!n->byte) {
    free(n);
    return NULL;
  }
  n->cap = b->cap;
  memcpy(n->byte, b->byte, n->cap);
  return n;
}

void bitmask_show(bitmask_t* b) {
  printf("bitmask capacity = %" PRIu64 "\n", b->cap);
  for (uint64_t i = 0; i < b->cap; i++) {
    for (int j = 0; j < 8; j++) {
      int mask = 1 << j;
      printf("%s ", b->byte[i] & mask ? "1" : "0");
    }
    printf("| ");
  }
  printf("\n");
}
