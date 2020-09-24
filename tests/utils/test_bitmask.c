#include <stdio.h>

#include "unity/unity.h"
#include "utils/bitmask.h"

void test_bitmask() {
  printf("bitmask new\n");
  bitmask_t* b = bitmask_new();
  bitmask_show(b);

  printf("bit 0 set\n");
  TEST_ASSERT(bitmask_op(b, 0, BITMASK_SET) == 0);
  TEST_ASSERT(bitmask_get(b, 0) == true);
  bitmask_show(b);

  printf("bit 2 set\n");
  TEST_ASSERT(bitmask_op(b, 2, BITMASK_SET) == 0);
  TEST_ASSERT(bitmask_get(b, 2) == true);
  bitmask_show(b);

  printf("bit 2 clear\n");
  TEST_ASSERT(bitmask_op(b, 2, BITMASK_CLEAR) == 0);
  TEST_ASSERT(bitmask_get(b, 2) == false);
  bitmask_show(b);

  printf("bit 16 set\n");
  TEST_ASSERT(bitmask_op(b, 16, BITMASK_SET) == 0);
  TEST_ASSERT(bitmask_get(b, 16) == true);
  bitmask_show(b);

  printf("bit 16 toggle\n");
  TEST_ASSERT(bitmask_op(b, 16, BITMASK_TOGGLE) == 0);
  TEST_ASSERT(bitmask_get(b, 16) == false);
  bitmask_show(b);

  printf("bit 128 set\n");
  TEST_ASSERT(bitmask_op(b, 128, BITMASK_SET) == 0);
  TEST_ASSERT(bitmask_get(b, 128) == true);
  bitmask_show(b);

  printf("bit 64 toggle\n");
  TEST_ASSERT(bitmask_op(b, 64, BITMASK_TOGGLE) == 0);
  TEST_ASSERT(bitmask_get(b, 64) == true);
  bitmask_show(b);

  printf("bit 0 toggle\n");
  TEST_ASSERT(bitmask_op(b, 0, BITMASK_TOGGLE) == 0);
  TEST_ASSERT(bitmask_get(b, 0) == false);
  bitmask_show(b);

  bitmask_free(b);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_bitmask);

  return UNITY_END();
}