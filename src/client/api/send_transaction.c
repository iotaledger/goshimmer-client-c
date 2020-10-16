#include <stdio.h>
#include <stdlib.h>

#include "client/api/json_utils.h"
#include "client/api/send_transaction.h"
#include "client/network/http.h"
#include "utils/iota_str.h"

// 0 on success
static int request_builder(byte_t const tx_bytes[], http_buf_t *req) {
  int ret = 0;

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    return -1;
  }

  cJSON_AddItemToObject(json_root, "txn_bytes", cJSON_CreateString(tx_bytes));
  char *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text == NULL) {
    ret = -1;
  } else {
    http_buf_append(req, (byte_t *)json_text, strlen(json_text));
    http_buf2str(req);
    free(json_text);
  }
  cJSON_Delete(json_root);

  return ret;
}

int send_tx_bytes(tangle_client_conf_t const *conf, byte_t const tx_bytes[], res_send_tx_t *res) {
  int ret = 0;
  char const *const cmd_send_tx = "value/sendTransaction";
  // compose restful api command
  iota_str_t *cmd = iota_str_new(conf->url);
  if (cmd == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    return -1;
  }

  if (iota_str_append(cmd, cmd_send_tx)) {
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

  http_buf_t *http_req = http_buf_new();
  http_buf_t *http_res = http_buf_new();
  if (http_res == NULL || http_req == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    // TODO
    ret = -1;
    goto done;
  }

  // build request
  if (request_builder(tx_bytes, http_req) != 0) {
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
  http_buf2str(http_res);

  // printf("req: %s\n", http_req->data);
  // printf("res: %s\n", http_res->data);

  // json deserialization
  deser_send_tx((char const *const)http_res->data, res);

done:
  // cleanup command
  iota_str_destroy(cmd);
  http_buf_free(http_res);
  http_buf_free(http_req);

  return ret;
}

int deser_send_tx(char const *const j_str, res_send_tx_t *res) {
  char const *const key_id = "transaction_id";
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
