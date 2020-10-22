#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>  // sleep

#include "unity/unity.h"
#include "wallet/wallet.h"

byte_t g_seed[TANGLE_SEED_BYTES];
char const *const g_endpoint = "https://api.goshimmer/";

void test_wallet_address_manager() {
  byte_t tmp[TANGLE_ADDRESS_BYTES];

  wallet_am_t *am = am_new(g_seed, 0, NULL);
  TEST_ASSERT_NOT_NULL(am);
  TEST_ASSERT_EQUAL_MEMORY(g_seed, am->seed, TANGLE_SEED_BYTES);

  // check address status
  TEST_ASSERT_EQUAL_INT64(0, am->first_unspent_idx);
  TEST_ASSERT_EQUAL_INT64(0, am->last_addr_index);
  TEST_ASSERT_EQUAL_INT64(0, am->last_unspent_idx);

  // createing addresses
  am_get_new_address(am, tmp);
  am_get_new_address(am, tmp);
  am_get_new_address(am, tmp);
  // check new address status
  TEST_ASSERT_EQUAL_INT64(0, am->first_unspent_idx);
  TEST_ASSERT_EQUAL_INT64(3, am->last_addr_index);
  TEST_ASSERT_EQUAL_INT64(3, am->last_unspent_idx);
  TEST_ASSERT_FALSE(am_is_spent_address(am, 0));
  TEST_ASSERT_FALSE(am_is_spent_address(am, 1));
  TEST_ASSERT_FALSE(am_is_spent_address(am, 2));
  TEST_ASSERT_FALSE(am_is_spent_address(am, 3));

  // Gets all addresses
  addr_list_t *addr_list = am_addresses(am);
  TEST_ASSERT(addr_list_len(addr_list) == 4);
  // addr_list_print(addr_list);
  addr_list_free(addr_list);
  addr_list = NULL;

  // Gets all unspent addresses
  addr_list = am_unspent_addresses(am);
  TEST_ASSERT(addr_list_len(addr_list) == 4);
  // addr_list_print(addr_list);
  addr_list_free(addr_list);
  addr_list = NULL;

  // no spent address yet
  addr_list = am_spent_addresses(am);
  TEST_ASSERT(addr_list_len(addr_list) == 0);
  // addr_list_print(addr_list);
  addr_list_free(addr_list);
  addr_list = NULL;

  // mark address[1] as spent address
  am_mark_spent_address(am, 1);

  // Get a spent address in list
  addr_list = am_spent_addresses(am);
  TEST_ASSERT(addr_list_len(addr_list) == 1);
  // addr_list_print(addr_list);
  addr_list_free(addr_list);
  addr_list = NULL;

  // should have 3 unspent addresses
  addr_list = am_unspent_addresses(am);
  TEST_ASSERT(addr_list_len(addr_list) == 3);
  // addr_list_print(addr_list);
  addr_list_free(addr_list);
  addr_list = NULL;

  // still 4 addresses in the list
  addr_list = am_addresses(am);
  TEST_ASSERT(addr_list_len(addr_list) == 4);
  // addr_list_print(addr_list);
  addr_list_free(addr_list);
  addr_list = NULL;

  am_free(am);
}

void test_wallet() {
  wallet_t *w = wallet_init(g_endpoint, 0, g_seed, 3, 1, 3);
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_NOT_NULL(w->addr_manager->seed);

  bool synced = wallet_is_node_synced(w);
  printf("Is endpoint synced? %s\n", synced ? "Yes" : "No");
  printf("balance: %" PRIu64 "\n", wallet_balance(w));
  unspent_outputs_print(&w->unspent);

  wallet_free(w);
}

void test_wallet_request_funds() {
  wallet_t *w = wallet_init(g_endpoint, 0, g_seed, 3, 1, 3);
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_NOT_NULL(w->addr_manager->seed);

  uint64_t balance_a = wallet_balance(w);
  TEST_ASSERT(wallet_request_funds(w) == 0);
  sleep(10);
  uint64_t balance_b = wallet_balance(w);
  TEST_ASSERT(balance_b > balance_a);

  wallet_free(w);
}

void test_wallet_send_funds() {
  wallet_t *w = wallet_init(g_endpoint, 0, g_seed, 3, 1, 3);
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_NOT_NULL(w->addr_manager->seed);

  send_funds_op_t send_op = {};
  send_op.amount = 100;
  address_from_base58("YQp3UbW56TX9HTm1XTUw1tRWHhLg8tKnNhT5FDq5MLNb", send_op.receiver);

  wallet_send_funds(w, &send_op);

  wallet_free(w);
}

int main() {
  UNITY_BEGIN();
  seed_from_base58("332Db2RL4NHggDX4utnn5sCwTVTqUQJ3vC42TGZFC8hK", g_seed);

  RUN_TEST(test_wallet_address_manager);
  // RUN_TEST(test_wallet);
  // RUN_TEST(test_wallet_request_funds);
  // RUN_TEST(test_wallet_send_funds);

  return UNITY_END();
}