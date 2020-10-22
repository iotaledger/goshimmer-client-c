#include <stdio.h>

#include "core/unspent_outputs.h"
#include "unity/unity.h"

void test_unspent_outputs() {
  byte_t addr[TANGLE_ADDRESS_BYTES] = {};

  byte_t color[BALANCE_COLOR_BYTES] = {};
  inclusion_state_t st = {.confirmed = true};

  // creates output IDs
  output_ids_t* ids = output_ids_init();

  // creates balance table
  balance_ht_t* bals = balance_ht_init();
  balance_ht_add(&bals, color, 100);
  balance_color_random(color);
  balance_ht_add(&bals, color, 1000);

  byte_t tx_id[TX_ID_BYTES] = {};
  randombytes_buf((void* const)tx_id, TX_ID_BYTES);
  st.liked = true;
  TEST_ASSERT(output_ids_add(&ids, tx_id, bals, &st) == 0);
  TEST_ASSERT_EQUAL_UINT32(1, output_ids_count(&ids));

  // 1st unspent output
  unspent_outputs_t* unspent = unspent_outputs_init();
  TEST_ASSERT_NULL(unspent);
  TEST_ASSERT_EQUAL_UINT32(0, unspent_outputs_count(&unspent));
  TEST_ASSERT(unspent_outputs_add(&unspent, addr, 0, ids) == 0);
  // adds the same address and output IDs
  TEST_ASSERT(unspent_outputs_add(&unspent, addr, 0, ids) == -1);
  TEST_ASSERT_NOT_NULL(unspent);
  TEST_ASSERT_EQUAL_UINT32(1, unspent_outputs_count(&unspent));

  // 2nd unspent output
  randombytes_buf((void* const)addr, TANGLE_ADDRESS_BYTES);
  // add one more tx id
  randombytes_buf((void* const)tx_id, TX_ID_BYTES);
  output_ids_add(&ids, tx_id, bals, &st);
  TEST_ASSERT(unspent_outputs_add(&unspent, addr, 0, ids) == 0);
  // appends output IDs to the same address
  TEST_ASSERT(unspent_outputs_append_id(&unspent, addr, ids) == 0);
  TEST_ASSERT_NOT_NULL(unspent);
  TEST_ASSERT_EQUAL_UINT32(2, unspent_outputs_count(&unspent));

  TEST_ASSERT(unspent_outputs_append_id(&unspent, addr, ids) == 0);
  TEST_ASSERT_EQUAL_UINT32(2, unspent_outputs_count(&unspent));

  // creates 2nd output IDs
  output_ids_t* ids_b = output_ids_init();
  randombytes_buf((void* const)tx_id, TX_ID_BYTES);
  TEST_ASSERT(output_ids_add(&ids, tx_id, bals, &st) == 0);
  TEST_ASSERT(unspent_outputs_append_id(&unspent, addr, ids) == 0);
  TEST_ASSERT_EQUAL_UINT32(2, unspent_outputs_count(&unspent));

  memset(addr, 0, TANGLE_ADDRESS_BYTES);
  unspent_outputs_remove(&unspent, addr);
  TEST_ASSERT_EQUAL_UINT32(1, unspent_outputs_count(&unspent));

  unspent_outputs_print(&unspent);

  unspent_outputs_free(&unspent);
  TEST_ASSERT_NULL(unspent);
  balance_ht_free(&bals);
  TEST_ASSERT_NULL(bals);
  output_ids_free(&ids);
  TEST_ASSERT_NULL(ids);
  output_ids_free(&ids_b);
  TEST_ASSERT_NULL(ids_b);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_unspent_outputs);

  return UNITY_END();
}
