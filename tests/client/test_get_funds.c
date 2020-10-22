#include <stdio.h>
#include <unity/unity.h>

#include "client/api/get_funds.h"
#include "client/network/http.h"

void test_get_funds() {
  tangle_client_conf_t ctx = {
      .url = "https://api.goshimmer/",
      .port = 0  // use default port number
  };
  res_get_funds_t funds = {};
  byte_t addr[TANGLE_ADDRESS_BYTES];
  address_from_base58("L6JurtKXPufmThEoK1aBwmyiJ1DKSMMxkMmFypiDksRx", addr);

  TEST_ASSERT(get_funds(&ctx, addr, &funds) == 0);
}

void test_deser_get_funds() {
  char const* const json_data =
      "{\"id\":\"2ndCgbwovEkYSkQrFtwBWzXkewfa3MmYyCs85mF3xQKDHj8rESpJTQXZbj89b8ZmfBjA9Pf81B1BzyMBYSncyKfe\"}";

  res_get_funds_t funds = {};
  TEST_ASSERT(deser_get_funds(json_data, &funds) == 0);
  TEST_ASSERT_EQUAL_STRING("2ndCgbwovEkYSkQrFtwBWzXkewfa3MmYyCs85mF3xQKDHj8rESpJTQXZbj89b8ZmfBjA9Pf81B1BzyMBYSncyKfe",
                           funds.msg_id);
}

int main() {
  UNITY_BEGIN();

  http_client_init();

  RUN_TEST(test_deser_get_funds);
  // RUN_TEST(test_get_funds);

  http_client_clean();

  return UNITY_END();
}