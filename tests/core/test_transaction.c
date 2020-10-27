#include <stdio.h>
#include <unity/unity.h>

#include "core/transaction.h"

void test_tx_inputs() {
  byte_t tx_output_id[TX_OUTPUT_ID_BYTES];
  char output_id_str[TX_OUTPUT_ID_BASE58_BUF];
  char* const expected_id = "ALC5JTNWc3dxeF4gwiCHnLEPTATbXt3pX2HxoGqV15WWYNXdFV7skxqm7Zs5vh6zpf7DeqAE9qs83sd8ZftQ6dEQ";

  tx_inputs_t* ins = tx_inputs_new();
  TEST_ASSERT_NOT_NULL(ins);

  // creates a random id and push to the list
  tx_output_id_random(tx_output_id);
  tx_inputs_push(ins, tx_output_id);

  // adds a base58 id to the list
  tx_inputs_push_base58(ins, expected_id);
  // should have 2 elements in the list
  TEST_ASSERT_EQUAL_INT32(2, tx_inputs_len(ins));

  // checks out of range
  byte_t* elm = tx_inputs_at(ins, 5);
  TEST_ASSERT_NULL(elm);

  // should be equal to the ramdon id
  elm = tx_inputs_at(ins, 0);
  TEST_ASSERT_EQUAL_MEMORY(tx_output_id, elm, TX_OUTPUT_ID_BYTES);

  elm = tx_inputs_at(ins, 1);
  tx_output_id_2_base58(elm, output_id_str);
  TEST_ASSERT_EQUAL_STRING(expected_id, output_id_str);

  // dump the list
  tx_inputs_print(ins);

  // free the list
  tx_inputs_free(ins);
}

void test_tx_output_list() {
  // creating an output list
  tx_outputs_t* output_list = tx_outputs_new();
  TEST_ASSERT_NOT_NULL(output_list);
  TEST_ASSERT(tx_outputs_len(output_list) == 0);

  // creating an output element
  tx_output_t out = {};
  // random address
  randombytes_buf((void* const)out.address, TANGLE_ADDRESS_BYTES);
  TEST_ASSERT_NULL(out.balances);

  // creating a balance 100 with empty color
  balance_t balance = {};
  balance.value = 100;

  // creating a balance list and push a balance element
  out.balances = balance_list_new();
  TEST_ASSERT_NOT_NULL(out.balances);
  TEST_ASSERT(balance_list_len(out.balances) == 0);
  // adding 1st balance element
  balance_list_push(out.balances, &balance);
  TEST_ASSERT(balance_list_len(out.balances) == 1);

  // adding 2nd balance element with random color
  balance.value = 200;
  randombytes_buf((void* const)balance.color, BALANCE_COLOR_BYTES);
  balance_list_push(out.balances, &balance);
  TEST_ASSERT(balance_list_len(out.balances) == 2);

  // adding 1st output to the output list
  tx_outputs_push(output_list, &out);
  TEST_ASSERT(tx_outputs_len(output_list) == 1);

  // adding 2nd output to the output list
  tx_outputs_push(output_list, &out);
  TEST_ASSERT(tx_outputs_len(output_list) == 2);

  // out of range
  tx_output_t* elm = tx_outputs_at(output_list, 100);
  TEST_ASSERT_NULL(elm);

  // validating 1st output and 2nd balance
  elm = tx_outputs_at(output_list, 0);
  TEST_ASSERT_NOT_NULL(elm);
  TEST_ASSERT_EQUAL_MEMORY(out.address, elm->address, TANGLE_ADDRESS_BYTES);
  balance_t* elm_balance = balance_list_at(elm->balances, 1);
  TEST_ASSERT_EQUAL_MEMORY(balance.color, elm_balance->color, BALANCE_COLOR_BYTES);
  TEST_ASSERT(balance.value == elm_balance->value);

  tx_outputs_print(output_list);

  // clean up
  balance_list_free(out.balances);
  tx_outputs_free(output_list);
}

void test_tx_empty_payload() {
  transaction_t tx = {};
  // seed
  byte_t my_seed[TANGLE_SEED_BYTES];
  seed_from_base58("332Db2RL4NHggDX4utnn5sCwTVTqUQJ3vC42TGZFC8hK", my_seed);

  // address
  byte_t addr_0[TANGLE_ADDRESS_BYTES];
  address_get(my_seed, 0, ADDRESS_VER_ED25519, addr_0);

  // add inputs
  tx.inputs = tx_inputs_new();
  byte_t tx_id[TX_ID_BYTES];
  byte_t output_id[TX_OUTPUT_ID_BYTES];
  tx_id_random(tx_id);
  tx_output_id(addr_0, tx_id, output_id);
  tx_inputs_push(tx.inputs, output_id);

  // add outputs
  tx.outputs = tx_outputs_new();
  tx_output_t out = {};
  out.addr_index = 0;
  memcpy(out.address, addr_0, TANGLE_ADDRESS_BYTES);
  // creating a balance 1 with empty color
  balance_t balance = {};
  balance_init(NULL, 1, &balance);
  // creating a balance list and push a balance element
  out.balances = balance_list_new();
  TEST_ASSERT_NOT_NULL(out.balances);
  TEST_ASSERT(balance_list_len(out.balances) == 0);
  // adding 1st balance element
  balance_list_push(out.balances, &balance);
  TEST_ASSERT(balance_list_len(out.balances) == 1);
  tx_outputs_push(tx.outputs, &out);

  // clean up balance list
  balance_list_free(out.balances);

  tx_outputs_print(tx.outputs);

  // calculate essence of the transaction
  byte_buf_t* essence = tx_essence(&tx);

  // get signature
  byte_t addr_pub[ED_PUBLIC_KEY_BYTES];
  byte_t addr_priv[ED_PRIVATE_KEY_BYTES];
  byte_t addr_sig[ED_SIGNATURE_BYTES];
  sign_signature(my_seed, 0, essence->data, essence->len, addr_sig);
  address_ed25519_keypair(my_seed, 0, addr_pub, addr_priv);

  // add signature to the transaction
  tx.signatures = ed_signatures_init();
  TEST_ASSERT(ed_signatures_add(&tx.signatures, addr_0, addr_pub, addr_sig) == 0);

  // validate tx
  TEST_ASSERT_TRUE(tx_signautres_valid(&tx));

  // dump tx object
  tx_print(&tx);

  byte_buf_t* s = tx_2_base64(&tx);
  // dump tx byte string
  printf("%s\n", s->data);

  // clean up
  tx_inputs_free(tx.inputs);
  tx_outputs_free(tx.outputs);
  byte_buf_free(essence);
  ed_signatures_destory(&tx.signatures);
  byte_buf_free(s);
}

void test_tx_id_ht() {
  // TODO
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_tx_inputs);
  RUN_TEST(test_tx_output_list);
  RUN_TEST(test_tx_empty_payload);
  RUN_TEST(test_tx_id_ht);

  return UNITY_END();
}
