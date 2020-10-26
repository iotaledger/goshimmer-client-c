#include <stdio.h>
#include <string.h>

#include "unity/unity.h"
#include "utils/byte_buffer.h"

void test_byte_buf() {
  // create a byte buffer
  byte_buf_t *buf = byte_buf_new();
  TEST_ASSERT_NOT_NULL(buf);
  TEST_ASSERT_NULL(buf->data);
  TEST_ASSERT(buf->cap == 0);
  TEST_ASSERT(buf->len == 0);

  // append two data sets to the buffer
  TEST_ASSERT_TRUE(byte_buf_append(buf, (byte_t *)"ABCDE", 5));
  TEST_ASSERT(buf->cap == 5);
  TEST_ASSERT(buf->len == 5);
  TEST_ASSERT_TRUE(byte_buf_append(buf, (byte_t *)"FGHIJ", 5));
  TEST_ASSERT(buf->cap == 10);
  TEST_ASSERT(buf->len == 10);
  // set data to the buffer
  TEST_ASSERT_TRUE(byte_buf_set(buf, (byte_t *)"VWXZY", 5));
  TEST_ASSERT(buf->cap == 10);
  TEST_ASSERT(buf->len == 5);
  // byte_buf_print(buf);

  // append an null terminator to buffer data
  byte_buf2str(buf);
  TEST_ASSERT(buf->cap == 10);
  TEST_ASSERT(buf->len == 6);
  // should not append the null terminator again
  byte_buf2str(buf);
  TEST_ASSERT(buf->cap == 10);
  TEST_ASSERT(buf->len == 6);
  printf("%s\n", buf->data);

  // reserve size smaller than capacity
  TEST_ASSERT_TRUE(byte_buf_reserve(buf, 5));
  TEST_ASSERT(buf->cap == 10);
  TEST_ASSERT(buf->len == 6);

  // reserve size bigger than capacity
  TEST_ASSERT_TRUE(byte_buf_reserve(buf, 100));
  TEST_ASSERT(buf->cap == 100);
  TEST_ASSERT(buf->len == 6);

  // byte_buf_print(buf);

  // duplicates a byte buffer
  byte_buf_t *c = byte_buf_clone(buf);
  TEST_ASSERT(c->cap == 6);
  TEST_ASSERT(c->len == 6);
  TEST_ASSERT(c != buf);
  TEST_ASSERT(c->data != buf->data);
  TEST_ASSERT_EQUAL_MEMORY(c->data, buf->data, buf->len);

  byte_buf_free(buf);
  byte_buf_free(c);
}

void test_byte2base64() {
  char test_data[] = "HELLO IOTA!";
  char exp_encode[] = "SEVMTE8gSU9UQSE=";

  // data to byte buffer
  byte_buf_t *buf = byte_buf_new_with_data((byte_t *)test_data, strlen(test_data));
  TEST_ASSERT_NOT_NULL(buf);
  TEST_ASSERT_EQUAL_MEMORY(test_data, buf->data, buf->len);

  // byte buffer to base64
  byte_buf_t *base64 = byte_buf2base64(buf);
  TEST_ASSERT_NOT_NULL(base64);

  // validating
  TEST_ASSERT_EQUAL_MEMORY(exp_encode, base64->data, base64->len);

  // clean up
  byte_buf_free(buf);
  byte_buf_free(base64);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_byte_buf);
  RUN_TEST(test_byte2base64);

  return UNITY_END();
}