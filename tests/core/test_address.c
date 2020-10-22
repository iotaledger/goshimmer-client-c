#include <stdio.h>
#include <unity/unity.h>

#include "core/address.h"

void test_address_gen() {
  byte_t iota_seed[TANGLE_SEED_BYTES] = {};
  char seed_str[TANGLE_ADDRESS_BASE58_BUF] = {};
  byte_t ed25519_addr[TANGLE_ADDRESS_BYTES] = {};
  char ed25519_addr_str[TANGLE_ADDRESS_BASE58_BUF] = {};
  // random_seed(iota_seed);
  TEST_ASSERT_TRUE(seed_from_base58("332Db2RL4NHggDX4utnn5sCwTVTqUQJ3vC42TGZFC8hK", iota_seed));
  TEST_ASSERT_TRUE(seed_2_base58(iota_seed, seed_str));
  TEST_ASSERT_EQUAL_STRING("332Db2RL4NHggDX4utnn5sCwTVTqUQJ3vC42TGZFC8hK", seed_str);

  address_get(iota_seed, 0, ADDRESS_VER_ED25519, ed25519_addr);
  TEST_ASSERT_TRUE(address_2_base58(ed25519_addr, ed25519_addr_str));
  // printf("addr: %s\n", ed25519_addr_str);
  TEST_ASSERT_EQUAL_STRING("XLnYsJLvb3Pj3F1m4Mt8vtYjTBCMYwLmk5jva1UXiPjm", ed25519_addr_str);

  address_get(iota_seed, 1, ADDRESS_VER_ED25519, ed25519_addr);
  TEST_ASSERT_TRUE(address_2_base58(ed25519_addr, ed25519_addr_str));
  // printf("addr: %s\n", ed25519_addr_str);
  TEST_ASSERT_EQUAL_STRING("UKUjKvfrKR1RnRiBpXSKxo6DRnvW6oqffsGfDrDEiVMX", ed25519_addr_str);

  uint32_t data_len = 4;
  uint8_t signature[ED_SIGNATURE_BYTES + data_len];
  uint8_t data[4] = {1, 3, 3, 8};
  sign_signature(iota_seed, 7, data, data_len, signature);
  // printf("sign: ");
  // dump_hex(signature, ED_SIGNATURE_BYTES + data_len);
  // TEST_ASSERT_TRUE(sign_verify_signature(iota_seed, 7, signature, data, data_len));

  byte_t addr_pub[ED_PUBLIC_KEY_BYTES];
  byte_t addr_priv[ED_PRIVATE_KEY_BYTES];
  address_ed25519_keypair(iota_seed, 7, addr_pub, addr_priv);
  TEST_ASSERT_TRUE(sign_verify_signature(signature, data, data_len, addr_pub));
}

void test_address_conv() {
  byte_t ed25519_addr[TANGLE_ADDRESS_BYTES] = {};
  char addr_base58[TANGLE_ADDRESS_BASE58_BUF] = {};
  address_from_base58("XLnYsJLvb3Pj3F1m4Mt8vtYjTBCMYwLmk5jva1UXiPjm", ed25519_addr);
  address_2_base58(ed25519_addr, addr_base58);
  TEST_ASSERT_EQUAL_STRING("XLnYsJLvb3Pj3F1m4Mt8vtYjTBCMYwLmk5jva1UXiPjm", addr_base58);
}

void test_address_list() {
  byte_t random_addr[TANGLE_ADDRESS_BYTES] = {};
  addr_list_t* list1 = addr_list_new();
  addr_list_t* list2 = addr_list_new();

  for (int i = 0; i < 5; i++) {
    address_t addr = {};
    randombytes_buf((void* const)addr.addr, TANGLE_ADDRESS_BYTES);
    addr.index = i;
    addr_list_push(list1, &addr);
  }
  TEST_ASSERT_EQUAL_INT16(5, addr_list_len(list1));
  TEST_ASSERT_EQUAL_INT16(0, addr_list_len(list2));
  TEST_ASSERT_NULL(addr_list_at(list1, 6));
  TEST_ASSERT_NULL(addr_list_at(list2, 0));
  addr_list_print(list1);

  addr_list_push(list2, addr_list_at(list1, 0));
  addr_list_push(list2, addr_list_at(list1, 2));
  addr_list_push(list2, addr_list_at(list1, 4));
  TEST_ASSERT_EQUAL_INT16(3, addr_list_len(list2));
  TEST_ASSERT_EQUAL_MEMORY(addr_list_at(list1, 0), addr_list_at(list2, 0), TANGLE_ADDRESS_BYTES);
  TEST_ASSERT_EQUAL_MEMORY(addr_list_at(list1, 2), addr_list_at(list2, 1), TANGLE_ADDRESS_BYTES);
  TEST_ASSERT_EQUAL_MEMORY(addr_list_at(list1, 4), addr_list_at(list2, 2), TANGLE_ADDRESS_BYTES);

  addr_list_pop(list2);
  addr_list_pop(list2);
  addr_list_pop(list2);
  TEST_ASSERT_EQUAL_INT16(0, addr_list_len(list2));
  TEST_ASSERT_NULL(addr_list_at(list2, 0));

  addr_list_free(list1);
  addr_list_free(list2);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_address_gen);
  RUN_TEST(test_address_conv);
  RUN_TEST(test_address_list);

  return UNITY_END();
}
