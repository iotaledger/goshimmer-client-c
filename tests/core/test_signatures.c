#include <stdio.h>

#include "core/signatures.h"
#include "sodium.h"
#include "unity/unity.h"

void test_signatures() {
  ed_signature_t* table = ed_signatures_init();
  TEST_ASSERT_NULL(table);
  TEST_ASSERT_EQUAL_INT32(0, ed_signatures_count(&table));

  byte_t addr[TANGLE_ADDRESS_BYTES];
  byte_t pub[ED_PUBLIC_KEY_BYTES];
  byte_t sig[ED_SIGNATURE_BYTES];
  randombytes_buf((void* const)addr, TANGLE_ADDRESS_BYTES);
  randombytes_buf((void* const)pub, ED_PUBLIC_KEY_BYTES);
  randombytes_buf((void* const)sig, ED_SIGNATURE_BYTES);

  TEST_ASSERT(ed_signatures_add(&table, addr, pub, sig) == 0);
  TEST_ASSERT_EQUAL_INT32(1, ed_signatures_count(&table));
  TEST_ASSERT(ed_signatures_add(&table, addr, pub, sig) == -1);
  TEST_ASSERT_EQUAL_INT32(1, ed_signatures_count(&table));

  randombytes_buf((void* const)addr, TANGLE_ADDRESS_BYTES);
  TEST_ASSERT(ed_signatures_add(&table, addr, pub, sig) == 0);
  TEST_ASSERT_EQUAL_INT32(2, ed_signatures_count(&table));

  randombytes_buf((void* const)addr, TANGLE_ADDRESS_BYTES);
  TEST_ASSERT_NULL(ed_signatures_find(&table, addr));
  TEST_ASSERT(ed_signatures_add(&table, addr, pub, sig) == 0);
  TEST_ASSERT_EQUAL_INT32(3, ed_signatures_count(&table));
  // ed_signatures_print(&table);

  ed_signatures_remove(&table, addr);
  TEST_ASSERT_NULL(ed_signatures_find(&table, addr));
  TEST_ASSERT_EQUAL_INT32(2, ed_signatures_count(&table));

  ed_signatures_print(&table);

  ed_signatures_destory(&table);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_signatures);

  return UNITY_END();
}
