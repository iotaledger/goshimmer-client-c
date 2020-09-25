#include <stdio.h>
#include <unity/unity.h>

#include "core/address.h"

void test_address_gen() {
  byte_t iota_seed[TANGLE_SEED_BYTES] = {};
  char seed_str[TANGLE_ADDRESS_BASE58_BUF] = {};
  byte_t ed25519_addr[TANGLE_ADDRESS_BYTES] = {};
  char ed25519_addr_str[TANGLE_ADDRESS_BASE58_BUF] = {};
  // random_seed(iota_seed);
  TEST_ASSERT_TRUE(seed_from_base58("7rxy9mMdYKjcqh4V1xHRvFK2FRMieRgzzt4txA2m8Hqq", iota_seed));
  size_t len = TANGLE_ADDRESS_BASE58_BUF;
  TEST_ASSERT_TRUE(seed_2_base58(iota_seed, seed_str, &len));
  TEST_ASSERT_EQUAL_STRING("7rxy9mMdYKjcqh4V1xHRvFK2FRMieRgzzt4txA2m8Hqq", seed_str);

  address_get(iota_seed, 0, ADDRESS_VER_ED25519, ed25519_addr);
  TEST_ASSERT_TRUE(address_2_base58(ed25519_addr, ed25519_addr_str));
  TEST_ASSERT_EQUAL_STRING("Lie4veK3knebo9b5LTxZU4hodWesHf3p321kzv1GNT9x", ed25519_addr_str);
  // printf("addr: %s\n", ed25519_addr_str);

  uint32_t data_len = 4;
  uint8_t signature[ED_SIGNATURE_BYTES + data_len];
  uint8_t data[4] = {1, 3, 3, 8};
  sign_signature(iota_seed, 7, data, data_len, signature);
  // printf("sign: ");
  // dump_hex(signature, ED_SIGNATURE_BYTES + data_len);
  TEST_ASSERT_TRUE(sign_verify_signature(iota_seed, 7, signature, data, data_len));
}

void test_address_list() {
  byte_t random_addr[TANGLE_ADDRESS_BYTES] = {};
  addr_list_t* list1 = addr_list_new();
  addr_list_t* list2 = addr_list_new();

  for (int i = 0; i < 5; i++) {
    randombytes_buf((void* const)random_addr, TANGLE_ADDRESS_BYTES);
    addr_list_push(list1, random_addr);
  }
  TEST_ASSERT_EQUAL_INT16(5, addr_list_len(list1));
  TEST_ASSERT_EQUAL_INT16(0, addr_list_len(list2));
  TEST_ASSERT_NULL(addr_list_at(list1, 6));
  TEST_ASSERT_NULL(addr_list_at(list2, 0));

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
  RUN_TEST(test_address_list);

  return UNITY_END();
}
