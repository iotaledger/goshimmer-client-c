#include <stdio.h>
#include <unity/unity.h>

#include "client/api/get_unspent_outputs.h"
#include "client/network/http.h"

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

  http_client_clean();

  return UNITY_END();
}
