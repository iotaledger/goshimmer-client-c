#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core/balance.h"
#include "libbase58.h"
#include "sodium.h"

static UT_icd const balance_list_icd = {sizeof(balance_t), NULL, NULL, NULL};

static bool empty_color(byte_t color[]) {
  for (int i = 0; i < BALANCE_COLOR_BYTES; i++) {
    if (color[i] != 0) {
      return false;
    }
  }
  return true;
}

void balance_color_random(byte_t color[]) { randombytes_buf((void* const)color, BALANCE_COLOR_BYTES); }

bool balance_color_2_base58(byte_t color[], char color_str[]) {
  size_t buf_len = BALANCE_COLOR_BASE58_BUF;
  bool ret = true;
  if (empty_color(color)) {
    snprintf(color_str, buf_len, "IOTA");
  } else {
    ret = b58enc(color_str, &buf_len, (const void*)color, BALANCE_COLOR_BYTES);
    // printf("len %zu, %d\n", buf_len, ret);
  }
  return ret;
}

bool balance_color_from_base58(char color_str[], byte_t color[]) {
  size_t bin_size = BALANCE_COLOR_BYTES;
  return b58tobin(color, &bin_size, color_str, strlen(color_str));
}

void balance_init(byte_t const color[], int64_t const value, balance_t* balance) {
  balance->value = value;
  memset(balance->color, 0, BALANCE_COLOR_BYTES);
  if (color) {
    balance_set_color(color, balance);
  }
}

void balance_from_bytes(byte_t balance_bytes[], balance_t* balance) {
  memcpy(&balance->value, balance_bytes, sizeof(balance->value));
  memcpy(balance->color, balance_bytes + sizeof(balance->value), sizeof(balance->color));
}

void balance_set_color(byte_t const color[], balance_t* balance) { memcpy(balance->color, color, BALANCE_COLOR_BYTES); }

void balance_2_bytes(balance_t* balance, byte_t balance_bytes[]) {
  // value offset
  int offset = sizeof(balance->value);
  memcpy(balance_bytes, &balance->value, offset);
  memcpy(balance_bytes + offset, balance->color, sizeof(balance->color));
}

void balance_print(balance_t* balance) {
  char color_str[BALANCE_COLOR_BASE58_BUF] = {};

  if (!empty_color(balance->color)) {
    balance_color_2_base58(balance->color, color_str);
    printf("balance[%" PRId64 ", %s]\n", balance->value, color_str);
  } else {
    printf("balance[%" PRId64 ", IOTA]\n", balance->value);
  }
}

balance_t* balance_clone(balance_t* src) {
  balance_t* dst = malloc(sizeof(balance_t));
  if (dst == NULL) {
    printf("[%s:%d] OOM\n", __func__, __LINE__);
    return NULL;
  }
  dst->value = src->value;
  memcpy(dst->color, src->color, BALANCE_COLOR_BYTES);
  return dst;
}

balance_list_t* balance_list_new() {
  balance_list_t* list = NULL;
  utarray_new(list, &balance_list_icd);
  return list;
}

void balance_list_print(balance_list_t* list) {
  balance_t* elm = NULL;

  BALANCE_LIST_FOREACH(list, elm) { balance_print(elm); }
}

balance_list_t* balance_list_clone(balance_list_t* src) {
  balance_t* elm = NULL;
  balance_list_t* dst = balance_list_new();
  if (dst == NULL) {
    printf("[%s:%d] OOM\n", __func__, __LINE__);
    return NULL;
  }

  BALANCE_LIST_FOREACH(src, elm) { balance_list_push(dst, elm); }
  return dst;
}

int balance_ht_add(balance_ht_t** t, byte_t const color[], int64_t value) {
  balance_ht_t* e = NULL;
  // checking uniqueness
  e = balance_ht_find(t, color);
  if (e != NULL) {
    printf("[%s:%d] address exists in table\n", __func__, __LINE__);
    return -1;
  }

  // adding to table
  e = malloc(sizeof(balance_ht_t));
  if (!e) {
    printf("[Err %s:%d] OOM\n", __func__, __LINE__);
    return -1;
  }
  memcpy(e->color, color, BALANCE_COLOR_BYTES);
  e->value = value;
  HASH_ADD(hh, *t, color, BALANCE_COLOR_BYTES, e);
  return 0;
}

uint64_t balance_ht_sum(balance_ht_t** t) {
  balance_ht_t *elm, *tmp;
  uint64_t sum = 0;
  HASH_ITER(hh, *t, elm, tmp) { sum += elm->value; }
  return sum;
}

void balance_ht_print(balance_ht_t** t) {
  balance_ht_t *elm, *tmp;
  char color_str[BALANCE_COLOR_BASE58_BUF] = {};
  printf("balances: [\n");
  HASH_ITER(hh, *t, elm, tmp) {
    if (!empty_color(elm->color)) {
      balance_color_2_base58(elm->color, color_str);
      printf("%s , %" PRId64 "\n", color_str, elm->value);
    } else {
      printf("IOTA, %" PRId64 "\n", elm->value);
    }
  }
  printf("]\n");
}

balance_ht_t* balance_ht_clone(balance_ht_t** t) {
  balance_ht_t* dst = balance_ht_init();
  balance_ht_t *src, *tmp;
  HASH_ITER(hh, *t, src, tmp) { balance_ht_add(&dst, src->color, src->value); }
  return dst;
}