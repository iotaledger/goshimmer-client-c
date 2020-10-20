#include <stdio.h>

#include "core/output_ids.h"
#include "unity/unity.h"

void test_output_ids() {
  byte_t tx_id[TX_ID_BYTES] = {};
  byte_t color[BALANCE_COLOR_BYTES] = {};
  inclusion_state_t st = {};

  output_ids_t* ids = output_ids_init();
  TEST_ASSERT_NULL(ids);
  TEST_ASSERT_EQUAL_UINT32(0, output_ids_count(&ids));

  balance_ht_t* bals = balance_ht_init();
  balance_ht_add(&bals, color, 100);
  balance_color_random(color);
  balance_ht_add(&bals, color, 1000);

  // adds 1st id
  randombytes_buf((void* const)tx_id, TX_ID_BYTES);
  st.confirmed = true;
  TEST_ASSERT(output_ids_add(&ids, tx_id, bals, &st) == 0);
  TEST_ASSERT_EQUAL_UINT32(1, output_ids_count(&ids));
  balance_ht_free(&bals);
  TEST_ASSERT_NULL(bals);

  // adds 2nd id
  bals = balance_ht_init();
  balance_color_random(color);
  balance_ht_add(&bals, color, INT64_MAX - 1);
  randombytes_buf((void* const)tx_id, TX_ID_BYTES);
  st.solid = true;
  TEST_ASSERT(output_ids_add(&ids, tx_id, bals, &st) == 0);
  TEST_ASSERT_EQUAL_UINT32(2, output_ids_count(&ids));
  balance_ht_free(&bals);
  TEST_ASSERT_NULL(bals);

  // gets the element of 2nd id
  output_ids_t* elm = output_ids_find(&ids, tx_id);
  TEST_ASSERT_NOT_NULL(elm);
  TEST_ASSERT(elm->st.confirmed == true);
  TEST_ASSERT(elm->st.solid == true);
  TEST_ASSERT_EQUAL_MEMORY(tx_id, elm->id, TX_ID_BYTES);
  // gets balance of 2nd id
  balance_ht_t* bal_elm = balance_ht_find(&elm->balances, color);
  TEST_ASSERT_NOT_NULL(bal_elm);
  TEST_ASSERT_EQUAL_INT64(bal_elm->value, INT64_MAX - 1);
  TEST_ASSERT_EQUAL_MEMORY(bal_elm->color, color, BALANCE_COLOR_BYTES);

  // adds 3rd id
  randombytes_buf((void* const)tx_id, TX_ID_BYTES);
  st.finalized = true;
  TEST_ASSERT(output_ids_add(&ids, tx_id, bals, &st) == 0);
  TEST_ASSERT_EQUAL_UINT32(3, output_ids_count(&ids));

  // duplicates a output id hash table
  output_ids_t* clone = output_ids_clone(&ids);
  TEST_ASSERT_NOT_NULL(clone);
  TEST_ASSERT(output_ids_count(&ids) == output_ids_count(&clone));
  TEST_ASSERT(ids != clone);
  TEST_ASSERT(ids->balances != clone->balances);
  TEST_ASSERT(&ids->st != &clone->st);

  // gets the element of 3rd id
  elm = output_ids_find(&clone, tx_id);
  TEST_ASSERT_NULL(elm->balances);
  TEST_ASSERT(elm->st.liked == false);

  // gets an element from random id
  randombytes_buf((void* const)tx_id, TX_ID_BYTES);
  elm = output_ids_find(&clone, tx_id);
  TEST_ASSERT_NULL(elm);

  output_ids_print(&ids);

  output_ids_free(&ids);
  output_ids_free(&clone);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_output_ids);

  return UNITY_END();
}
