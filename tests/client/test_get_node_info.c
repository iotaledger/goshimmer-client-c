#include <stdio.h>
#include <unity/unity.h>

#include "client/api/get_node_info.h"

void test_get_info() {
  iota_client_conf_t ctx = {
      .url = "https://api.goshimmer/",
      .port = 0  // use default port number
  };
  res_node_info_t info;

  int ret = get_node_info(&ctx, &info);
  TEST_ASSERT_EQUAL_INT(0, ret);
  // TEST_ASSERT_EQUAL_STRING("v0.2.2", info.version);
}

void test_deser_node_info() {
  char const* const json_info =
      "{\"version\":\"v0.2.2\",\"synced\":false,\"beacons\":[{\"public_key\":"
      "\"Gm7W191NDnqyF7KJycZqK7V6ENLwqxTwoKQN4SmpkB24\",\"msg_id\":"
      "\"JXLXpBovGdCFZvPzCgzq9onQ1biCp3sS3JhCod4NivY972f7j4nuPwj8kEQ4KzWg62Lm1mZ7CP99Gw3wUZUBhyG\",\"sent_time\":"
      "1597144480364120758,\"synced\":false},{\"public_key\":\"9DB3j9cWYSuEEtkvanrzqkzCQMdH1FGv3TawJdVbDxkd\",\"msg_"
      "id\":\"5bvWvrEhsLLbVNY2xQTD5xMR7wxh8NHWJgU8FjtXaGGdqj1z2uPZE5MG9qVczdFZtjtvV8Aj8AiTgESXH9LQSa9U\",\"sent_time\":"
      "1597144509080066256,\"synced\":false}],\"identityID\":\"KBTmE299rMU\",\"publicKey\":"
      "\"ANeRVnSV9xiqV9wLMn6iwXg65jphxm7kwAyHtamBZrnd\",\"messageRequestQueueSize\":2,\"solidMessageCount\":1486889,"
      "\"totalMessageCount\":1486889,\"enabledPlugins\":[\"Analysis-Client\",\"Autopeering\",\"CLI\",\"Config\","
      "\"DRNG\",\"Dashboard\",\"Database\",\"Gossip\",\"Graceful "
      "Shutdown\",\"Issuer\",\"Logger\",\"MessageLayer\",\"Metrics\",\"NetworkDelay\",\"PoW\",\"Profiling\","
      "\"Prometheus\",\"RemoteLog\",\"Spammer\",\"SyncBeaconFollower\",\"ValueTransfers\",\"WebAPI\",\"WebAPI DRNG "
      "Endpoint\",\"WebAPI Value Endpoint\",\"WebAPI autopeering Endpoint\",\"WebAPI data Endpoint\",\"WebAPI faucet "
      "Endpoint\",\"WebAPI healthz Endpoint\",\"WebAPI info Endpoint\",\"WebAPI message "
      "Endpoint\"],\"disabledPlugins\":[\"Analysis-Dashboard\",\"Analysis-Server\",\"Banner\",\"Faucet\",\"PortCheck\","
      "\"SyncBeacon\",\"WebAPI Auth\",\"WebAPI tools Endpoint\"]}";

  res_node_info_t info = {};
  int ret = deser_node_info(json_info, &info);
  TEST_ASSERT_EQUAL_INT(0, ret);
  TEST_ASSERT_EQUAL_STRING("v0.2.2", info.version);
  TEST_ASSERT_EQUAL_STRING("KBTmE299rMU", info.id);
  TEST_ASSERT_FALSE(info.is_synced);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_deser_node_info);
  // RUN_TEST(test_get_info);

  return UNITY_END();
}