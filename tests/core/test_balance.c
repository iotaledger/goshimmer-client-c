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
  balance_init(&balan, NULL, 100);
  print_balance(&balan);

  // convert balance to bytes
  balance_2_bytes(balance_byte, &balan);
  // init balance object from bytes
  balance_from_bytes(&balan_exp, balance_byte);

  // expect: balan_exp == balan
  TEST_ASSERT_EQUAL_MEMORY(balan_exp.color, balan.color, BALANCE_COLOR_BYTES);
  TEST_ASSERT_EQUAL_UINT64(balan_exp.value, balan.value);

  for (int i = 0; i < BALANCE_COLOR_BYTES; i++) {
    color[i] = 0x22;
  }

  balance_set_color(&balan, color);
  TEST_ASSERT_EQUAL_MEMORY(balan.color, color, BALANCE_COLOR_BYTES);
  print_balance(&balan);
}

void test_balance_list() {
  balance_t tmp_bal;
  byte_t color[BALANCE_COLOR_BYTES];
  randombytes_buf((void* const)color, BALANCE_COLOR_BYTES);

  balance_init(&tmp_bal, color, 100);

  TEST_ASSERT_EQUAL_MEMORY(color, tmp_bal.color, BALANCE_COLOR_BYTES);
  TEST_ASSERT_EQUAL_INT32(100, tmp_bal.value);

  balance_list_t* list = balance_list_new();
  TEST_ASSERT_NOT_NULL(list);
  TEST_ASSERT(balance_list_len(list) == 0);

  balance_list_push(list, &tmp_bal);
  TEST_ASSERT(balance_list_len(list) == 1);

  balance_init(&tmp_bal, color, 200);
  balance_list_push(list, &tmp_bal);
  TEST_ASSERT(balance_list_len(list) == 2);

  balance_t* b = balance_list_at(list, 1);
  TEST_ASSERT_EQUAL_MEMORY(color, b->color, BALANCE_COLOR_BYTES);
  TEST_ASSERT_EQUAL_INT32(200, b->value);

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

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_balance);
  RUN_TEST(test_balance_list);

  return UNITY_END();
}
