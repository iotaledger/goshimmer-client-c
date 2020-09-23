#include <stdio.h>

#include "unity/unity.h"
#include "utils/bit_mask.h"

void test_bit_mask() {
  bit_mask_t* b = bit_mask_new();
  
  bit_mask_free(b);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_bit_mask);

  return UNITY_END();
}