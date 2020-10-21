#include <stdio.h>
#include <unity/unity.h>

#include "client/api/get_unspent_outputs.h"
#include "client/network/http.h"

void test_unspent_outputs() {
  tangle_client_conf_t ctx = {
      .url = "https://api.goshimmer/",
      .port = 0  // use default port number
  };
  addr_list_t* addresses = addr_list_new();
  TEST_ASSERT_NOT_NULL(addresses);
  unspent_outputs_t* unspents = unspent_outputs_init();
  TEST_ASSERT_NULL(unspents);

  byte_t tmp_addr[TANGLE_ADDRESS_BYTES];
  TEST_ASSERT_TRUE(address_from_base58("XLnYsJLvb3Pj3F1m4Mt8vtYjTBCMYwLmk5jva1UXiPjm", tmp_addr));
  addr_list_push(addresses, tmp_addr);
  TEST_ASSERT_TRUE(address_from_base58("UKUjKvfrKR1RnRiBpXSKxo6DRnvW6oqffsGfDrDEiVMX", tmp_addr));
  addr_list_push(addresses, tmp_addr);
  TEST_ASSERT_TRUE(address_from_base58("YQp3UbW56TX9HTm1XTUw1tRWHhLg8tKnNhT5FDq5MLNb", tmp_addr));
  addr_list_push(addresses, tmp_addr);

  int ret = get_unspent_outputs(&ctx, addresses, &unspents);
  TEST_ASSERT_EQUAL_INT(0, ret);
  unspent_outputs_print(&unspents);

  addr_list_free(addresses);
  unspent_outputs_free(&unspents);
}

void test_deser_unspent_outputs() {
  char const data1[] =
      "{\"unspent_outputs\":[{\"address\":\"XLnYsJLvb3Pj3F1m4Mt8vtYjTBCMYwLmk5jva1UXiPjm\",\"output_ids\":[]},{"
      "\"address\":\"UKUjKvfrKR1RnRiBpXSKxo6DRnvW6oqffsGfDrDEiVMX\",\"output_ids\":[]},{\"address\":"
      "\"YQp3UbW56TX9HTm1XTUw1tRWHhLg8tKnNhT5FDq5MLNb\",\"output_ids\":[{\"id\":"
      "\"ALC5JTNWc3dxeF4gwiCHnLEPTATbXt3pX2HxoGqV15WWkT84CuiFty3cCRq5QaF3mYK5b87jpXyPLKbpsAdMPvaS\",\"balances\":[{"
      "\"value\":1337,\"color\":\"IOTA\"}],\"inclusion_state\":{\"confirmed\":true,\"liked\":true,\"finalized\":true}}]"
      "}]}";

  unspent_outputs_t* unspents = unspent_outputs_init();
  TEST_ASSERT(deser_unspent_outputs(data1, &unspents) == 0);
  unspent_outputs_print(&unspents);
  unspent_outputs_free(&unspents);
  TEST_ASSERT_NULL(unspents);

  char const data2[] =
      "{\"unspent_outputs\":[{\"address\":\"XLnYsJLvb3Pj3F1m4Mt8vtYjTBCMYwLmk5jva1UXiPjm\",\"output_ids\":[]},{"
      "\"address\":\"UKUjKvfrKR1RnRiBpXSKxo6DRnvW6oqffsGfDrDEiVMX\",\"output_ids\":[]},{\"address\":"
      "\"YQp3UbW56TX9HTm1XTUw1tRWHhLg8tKnNhT5FDq5MLNb\",\"output_ids\":[]}]}";

  TEST_ASSERT(deser_unspent_outputs(data2, &unspents) == 0);
  unspent_outputs_print(&unspents);
  unspent_outputs_free(&unspents);
  TEST_ASSERT_NULL(unspents);

  // invalid
  char const data3[] = "";
  TEST_ASSERT(deser_unspent_outputs(data3, &unspents) == -1);
  unspent_outputs_free(&unspents);
  TEST_ASSERT_NULL(unspents);

  char const data4[] = "{\"unspent_outputs\":[]}";
  TEST_ASSERT(deser_unspent_outputs(data4, &unspents) == -1);
  unspent_outputs_free(&unspents);
  TEST_ASSERT_NULL(unspents);

  char const data5[] =
      "{\"unspent_outputs\":[{\"address\":\"XLnYsJLvb3Pj3F1m4Mt8vtYjTBCMYwLmk5jva1UXiPjm\",\"output_ids\":[]},{"
      "\"address\":\"UKUjKvfrKR1RnRiBpXSKxo6DRnvW6oqffsGfDrDEiVMX\",\"output_ids\":[{\"id\":"
      "\"97gvUtcshzi6DG9zpqbLoVktVs4kmrz9mn4YcgWfNt2roRjPBZLQWvn3Ky4hBtrYT1stY1fRMqcrAH6mq9SzH6SF\",\"balances\":[{"
      "\"value\":100,\"color\":\"IOTA\"}],\"inclusion_state\":{\"confirmed\":true,\"liked\":true,\"finalized\":true}}]}"
      ",{\"address\":\"YQp3UbW56TX9HTm1XTUw1tRWHhLg8tKnNhT5FDq5MLNb\",\"output_ids\":[{\"id\":"
      "\"ALC5JTNWc3dxeF4gwiCHnLEPTATbXt3pX2HxoGqV15WWaC78z1UUf6eVXSUn9kudWc5zGNsX8threiQEKazMHfhM\",\"balances\":[{"
      "\"value\":1237,\"color\":\"IOTA\"}],\"inclusion_state\":{\"confirmed\":true,\"liked\":true,\"finalized\":true}},"
      "{\"id\":\"ALC5JTNWc3dxeF4gwiCHnLEPTATbXt3pX2HxoGqV15WWkT84CuiFty3cCRq5QaF3mYK5b87jpXyPLKbpsAdMPvaS\","
      "\"balances\":[{\"value\":1337,\"color\":\"IOTA\"}],\"inclusion_state\":{\"confirmed\":true,\"liked\":true,"
      "\"finalized\":true}}]}]}";
  TEST_ASSERT(deser_unspent_outputs(data5, &unspents) == 0);
  unspent_outputs_print(&unspents);
  unspent_outputs_free(&unspents);
  TEST_ASSERT_NULL(unspents);
}

int main() {
  UNITY_BEGIN();

  http_client_init();

  RUN_TEST(test_deser_unspent_outputs);
  // RUN_TEST(test_unspent_outputs);

  http_client_clean();

  return UNITY_END();
}
