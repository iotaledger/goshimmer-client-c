#include <stdio.h>
#include <unity/unity.h>

#include "client/api/get_unspent_outputs.h"
#include "client/network/http.h"

void test_unspent_outputs() {
  tangle_client_conf_t ctx = {
      .url = "https://api.goshimmer/",
      .port = 0  // use default port number
  };
  req_unspent_outs_t* addresses = addr_list_new();
  TEST_ASSERT_NOT_NULL(addresses);
  res_unspent_outs_t* res = unspent_list_new();
  TEST_ASSERT_NOT_NULL(res);

  byte_t tmp_addr[TANGLE_ADDRESS_BYTES];
  TEST_ASSERT_TRUE(address_from_base58("XLnYsJLvb3Pj3F1m4Mt8vtYjTBCMYwLmk5jva1UXiPjm", tmp_addr));
  addr_list_push(addresses, tmp_addr);
  TEST_ASSERT_TRUE(address_from_base58("UKUjKvfrKR1RnRiBpXSKxo6DRnvW6oqffsGfDrDEiVMX", tmp_addr));
  addr_list_push(addresses, tmp_addr);
  TEST_ASSERT_TRUE(address_from_base58("YQp3UbW56TX9HTm1XTUw1tRWHhLg8tKnNhT5FDq5MLNb", tmp_addr));
  addr_list_push(addresses, tmp_addr);

  int ret = get_unspent_outputs(&ctx, addresses, res);
  TEST_ASSERT_EQUAL_INT(0, ret);
  unspent_list_print(res);

  addr_list_free(addresses);
  unspent_list_free(res);
}

void test_deser_unspent_outputs() {
  char const data1[] =
      "{\"unspent_outputs\":[{\"address\":\"XLnYsJLvb3Pj3F1m4Mt8vtYjTBCMYwLmk5jva1UXiPjm\",\"output_ids\":[]},{"
      "\"address\":\"UKUjKvfrKR1RnRiBpXSKxo6DRnvW6oqffsGfDrDEiVMX\",\"output_ids\":[]},{\"address\":"
      "\"YQp3UbW56TX9HTm1XTUw1tRWHhLg8tKnNhT5FDq5MLNb\",\"output_ids\":[{\"id\":"
      "\"ALC5JTNWc3dxeF4gwiCHnLEPTATbXt3pX2HxoGqV15WWkT84CuiFty3cCRq5QaF3mYK5b87jpXyPLKbpsAdMPvaS\",\"balances\":[{"
      "\"value\":1337,\"color\":\"IOTA\"}],\"inclusion_state\":{\"confirmed\":true,\"liked\":true,\"finalized\":true}}]"
      "}]}";

  res_unspent_outs_t* res = unspent_list_new();
  TEST_ASSERT(deser_unspent_outputs(data1, res) == 0);
  unspent_list_free(res);
  res = NULL;

  char const data2[] =
      "{\"unspent_outputs\":[{\"address\":\"XLnYsJLvb3Pj3F1m4Mt8vtYjTBCMYwLmk5jva1UXiPjm\",\"output_ids\":[]},{"
      "\"address\":\"UKUjKvfrKR1RnRiBpXSKxo6DRnvW6oqffsGfDrDEiVMX\",\"output_ids\":[]},{\"address\":"
      "\"YQp3UbW56TX9HTm1XTUw1tRWHhLg8tKnNhT5FDq5MLNb\",\"output_ids\":[]}]}";
  res = unspent_list_new();
  TEST_ASSERT(deser_unspent_outputs(data2, res) == 0);
  unspent_list_free(res);
  res = NULL;

  // invalid
  char const data3[] = "";
  res = unspent_list_new();
  TEST_ASSERT(deser_unspent_outputs(data3, res) == -1);
  unspent_list_free(res);
  res = NULL;

  char const data4[] = "{\"unspent_outputs\":[]}";
  res = unspent_list_new();
  TEST_ASSERT(deser_unspent_outputs(data4, res) == -1);
  unspent_list_free(res);
  res = NULL;
}

int main() {
  UNITY_BEGIN();

  http_client_init();

  RUN_TEST(test_deser_unspent_outputs);
  // RUN_TEST(test_unspent_outputs);

  http_client_clean();

  return UNITY_END();
}
