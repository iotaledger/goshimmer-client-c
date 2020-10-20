#include <stdio.h>

#include "unity/unity.h"
#include "wallet/wallet.h"

void test_wallet_address_manager() {
  byte_t seed[TANGLE_SEED_BYTES];
  byte_t tmp[TANGLE_ADDRESS_BYTES];
  char *tmp_addr[TANGLE_ADDRESS_BASE58_BUF];

  // init address manager with a given seed
  seed_from_base58("332Db2RL4NHggDX4utnn5sCwTVTqUQJ3vC42TGZFC8hK", seed);
  wallet_am_t *am = am_new(seed, 0, NULL);
  TEST_ASSERT_NOT_NULL(am);
  TEST_ASSERT_EQUAL_MEMORY(seed, am->seed, TANGLE_SEED_BYTES);

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
  wallet_t *w = wallet_init("https://api.goshimmer/", 0, NULL);
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_NOT_NULL(w->addr_manager->seed);

  bool synced = wallet_is_node_synced(w);
  printf("Is endpoint synced? %s\n", synced ? "Yes" : "No");

  wallet_free(w);
}

void test_wallet_send_funds() {
  wallet_t *w = wallet_init("https://api.goshimmer/", 0, NULL);
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_NOT_NULL(w->addr_manager->seed);

  // int ret = wallet_send_funds(w);
  // printf("Is endpoint synced? %s\n", synced ? "Yes" : "No");

  wallet_free(w);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_wallet_address_manager);
  // RUN_TEST(test_wallet);
  // RUN_TEST(test_wallet_send_funds);

  return UNITY_END();
}