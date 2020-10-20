#include <stdio.h>

#include "core/balance.h"
#include "sodium.h"
#include "unity/unity.h"

void test_balance() {
  balance_t balan;
  byte_t balance_byte[sizeof(balance_t)];
  byte_t color[BALANCE_COLOR_BYTES];
  balance_t balan_exp;

  // create a new balance
  balance_init(NULL, 100, &balan);
  balance_print(&balan);

  // convert balance to bytes
  balance_2_bytes(&balan, balance_byte);
  // init balance object from bytes
  balance_from_bytes(balance_byte, &balan_exp);

  // expect: balan_exp == balan
  TEST_ASSERT_EQUAL_MEMORY(balan_exp.color, balan.color, BALANCE_COLOR_BYTES);
  TEST_ASSERT_EQUAL_UINT64(balan_exp.value, balan.value);

  for (int i = 0; i < BALANCE_COLOR_BYTES; i++) {
    color[i] = 0x22;
  }

  balance_set_color(color, &balan);
  TEST_ASSERT_EQUAL_MEMORY(balan.color, color, BALANCE_COLOR_BYTES);
  balance_print(&balan);
}

void test_balance_list() {
  balance_t tmp_bal;
  byte_t color[BALANCE_COLOR_BYTES];
  randombytes_buf((void* const)color, BALANCE_COLOR_BYTES);

  balance_init(color, 100, &tmp_bal);

  TEST_ASSERT_EQUAL_MEMORY(color, tmp_bal.color, BALANCE_COLOR_BYTES);
  TEST_ASSERT_EQUAL_INT32(100, tmp_bal.value);

  balance_list_t* list = balance_list_new();
  TEST_ASSERT_NOT_NULL(list);
  TEST_ASSERT(balance_list_len(list) == 0);

  balance_list_push(list, &tmp_bal);
  TEST_ASSERT(balance_list_len(list) == 1);

  balance_init(color, 200, &tmp_bal);
  balance_list_push(list, &tmp_bal);
  TEST_ASSERT(balance_list_len(list) == 2);

  balance_t* b = balance_list_at(list, 1);
  TEST_ASSERT_EQUAL_MEMORY(color, b->color, BALANCE_COLOR_BYTES);
  TEST_ASSERT_EQUAL_INT32(200, b->value);

  balance_list_print(list);

  balance_list_pop(list);
  TEST_ASSERT(balance_list_len(list) == 1);
  b = balance_list_at(list, 1);
  TEST_ASSERT_NULL(b);
  b = balance_list_at(list, 0);
  TEST_ASSERT_NOT_NULL(b);
  TEST_ASSERT_EQUAL_MEMORY(color, b->color, BALANCE_COLOR_BYTES);
  TEST_ASSERT_EQUAL_INT32(100, b->value);

  balance_list_free(list);
}

void test_balance_ht() {
  balance_ht_t* table = balance_ht_init();
  TEST_ASSERT_NULL(table);
  TEST_ASSERT_EQUAL_INT32(0, balance_ht_count(&table));

  byte_t color[BALANCE_COLOR_BYTES] = {};
  randombytes_buf((void* const)color, BALANCE_COLOR_BYTES);

  TEST_ASSERT(balance_ht_add(&table, color, -100) == 0);
  TEST_ASSERT_EQUAL_INT32(1, balance_ht_count(&table));
  TEST_ASSERT(balance_ht_add(&table, color, 200) == -1);
  TEST_ASSERT_EQUAL_INT32(1, balance_ht_count(&table));

  // randombytes_buf((void* const)color, BALANCE_COLOR_BYTES);
  memset(color, 0, BALANCE_COLOR_BYTES);
  TEST_ASSERT(balance_ht_add(&table, color, 1000) == 0);
  TEST_ASSERT_EQUAL_INT32(2, balance_ht_count(&table));

  randombytes_buf((void* const)color, BALANCE_COLOR_BYTES);
  TEST_ASSERT_NULL(balance_ht_find(&table, color));
  TEST_ASSERT(balance_ht_add(&table, color, 2000) == 0);
  TEST_ASSERT_EQUAL_INT32(3, balance_ht_count(&table));
  balance_ht_print(&table);

  balance_ht_remove(&table, color);
  TEST_ASSERT_NULL(balance_ht_find(&table, color));
  TEST_ASSERT_EQUAL_INT32(2, balance_ht_count(&table));

  balance_ht_print(&table);

  balance_ht_free(&table);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_balance);
  RUN_TEST(test_balance_list);
  RUN_TEST(test_balance_ht);
  // TODO
  // RUN_TEST(test_balance_color);

  return UNITY_END();
}
