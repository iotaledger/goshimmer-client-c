#include <stdio.h>
#include <unity/unity.h>

#include "core/transaction.h"

void test_tx_inputs() {
  byte_t tx_output_id[TX_OUTPUT_ID_BYTES];

  tx_inputs_t *ins = tx_inputs_new();
  TEST_ASSERT_NOT_NULL(ins);

  // creates a random id and push to the list
  tx_output_id_random(tx_output_id);
  tx_inputs_push(ins, tx_output_id);

  // adds a base58 id to the list
  tx_inputs_push_base58(ins,
                        "ALC5JTNWc3dxeF4gwiCHnLEPTATbXt3pX2HxoGqV15WWYNXdFV7skxqm7Zs5vh6zpf7DeqAE9qs83sd8ZftQ6dEQ");
  // should have 2 elements in the list
  TEST_ASSERT_EQUAL_INT32(2, tx_inputs_len(ins));

  // checks out of range
  byte_t *elm = tx_inputs_at(ins, 5);
  TEST_ASSERT_NULL(elm);

  // should be equal to the ramdon id
  elm = tx_inputs_at(ins, 0);
  TEST_ASSERT_EQUAL_MEMORY(tx_output_id, elm, TX_OUTPUT_ID_BYTES);

  // dump the list
  tx_inputs_print(ins);

  // free the list
  tx_inputs_free(ins);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_tx_inputs);

  return UNITY_END();
}
