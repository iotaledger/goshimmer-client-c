#include <stdio.h>
#include <stdlib.h>

#include "client/api/get_funds.h"
#include "client/api/json_utils.h"
#include "client/network/http.h"
#include "utils/iota_str.h"

// 0 on success
static int request_builder(byte_t const addr[], byte_buf_t *req) {
  int ret = 0;
  char addr_str[TANGLE_ADDRESS_BASE58_BUF];

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    return -1;
  }
  address_2_base58(addr, addr_str);

  cJSON_AddItemToObject(json_root, "address", cJSON_CreateString(addr_str));
  char const *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text == NULL) {
    ret = -1;
  } else {
    byte_buf_append(req, (byte_t *)json_text, strlen(json_text));
    byte_buf2str(req);
  }
  cJSON_Delete(json_root);

  return ret;
}

int get_funds(tangle_client_conf_t const *conf, byte_t const addr[], res_get_funds_t *res) {
  int ret = 0;
  char const *const cmd_faucet = "faucet";
  byte_buf_t *http_req = NULL;
  byte_buf_t *http_res = NULL;
  // compose restful api command
  iota_str_t *cmd = iota_str_new(conf->url);
  if (cmd == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    return -1;
  }

  if (iota_str_append(cmd, cmd_faucet)) {
    printf("[%s:%d]: string append failed\n", __func__, __LINE__);
    ret = -1;
    goto done;
  }

  // http client configuration
  http_client_config_t http_conf = {0};
  http_conf.url = cmd->buf;
  if (conf->port) {
    http_conf.port = conf->port;
  }

  http_req = byte_buf_new();
  http_res = byte_buf_new();
  if (http_res == NULL || http_req == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    // TODO
    ret = -1;
    goto done;
  }

  // build request
  if (request_builder(addr, http_req) != 0) {
    printf("[%s:%d]: build request failed\n", __func__, __LINE__);
    ret = -1;
    goto done;
  }

  // send request via http client
  if (http_client_post(&http_conf, http_req, http_res) != 0) {
    printf("[%s:%d]: http client post failed\n", __func__, __LINE__);
    ret = -1;
    goto done;
  }
  byte_buf2str(http_res);

  // printf("req: %s\n", http_req->data);
  // printf("res: %s\n", http_res->data);

  // json deserialization
  deser_get_funds((char const *const)http_res->data, res);

done:
  // cleanup command
  iota_str_destroy(cmd);
  byte_buf_free(http_res);
  byte_buf_free(http_req);

  return ret;
}

int deser_get_funds(char const *const j_str, res_get_funds_t *res) {
  char const *const key_id = "id";
  int ret = 0;
  cJSON *json_obj = cJSON_Parse(j_str);
  if (json_obj == NULL) {
    return -1;
  }

  res_err_t *res_err = deser_error(json_obj);
  if (res_err) {
    // got an error response
    printf("[%s:%d] Error response: %s\n", __func__, __LINE__, res_err->msg);
    return -1;
  }

  // gets ID
  if ((ret = json_get_string(json_obj, key_id, res->msg_id, sizeof(res->msg_id))) != 0) {
    printf("[%s:%d]: gets %s json string failed\n", __func__, __LINE__, key_id);
    ret = -1;
    goto end;
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}
