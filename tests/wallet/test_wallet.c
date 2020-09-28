#include <stdio.h>

#include "unity/unity.h"
#include "wallet/wallet.h"

void test_wallet() {
  wallet_t *w = wallet_init("https://api.goshimmer/", 0, NULL);
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_NOT_NULL(w->addr_manager->seed);

  bool synced = wallet_is_node_synced(w);
  printf("Is endpoint synced? %s\n", synced ? "Yes" : "No");

  wallet_free(w);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_wallet);

  return UNITY_END();
}