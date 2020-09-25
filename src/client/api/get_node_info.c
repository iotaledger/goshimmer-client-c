#include <stdio.h>
#include <stdlib.h>

#include "client/api/get_node_info.h"
#include "client/api/json_utils.h"
#include "client/network/http.h"
#include "client/network/http_buffer.h"
#include "utils/iota_str.h"

int get_node_info(iota_client_conf_t const *conf, res_node_info_t *res) {
  int ret = 0;
  char const *const cmd_info = "info";
  // compose restful api command
  iota_str_t *cmd = iota_str_new(conf->url);
  if (cmd == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    return -1;
  }

  if (iota_str_append(cmd, cmd_info)) {
    printf("[%s:%d]: string append failed\n", __func__, __LINE__);
    return -1;
  }

  // http client configuration
  http_client_config_t http_conf = {0};
  http_conf.url = cmd->buf;
  if (conf->port) {
    http_conf.port = conf->port;
  }

  http_buf_t *http_res = http_buf_new();
  if (http_res == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    // TODO
    ret = -1;
    goto done;
  }

  // send request via http client
  if (http_client_get(&http_conf, http_res) != 0) {
    printf("[%s:%d]: http get failed\n", __func__, __LINE__);
    ret = -1;
    goto done;
  }
  http_buf2str(http_res);
  // json deserialization
  deser_node_info((char const *const)http_res->data, res);

done:
  // cleanup command
  iota_str_destroy(cmd);
  http_buf_free(http_res);

  return ret;
}

int deser_node_info(char const *const j_str, res_node_info_t *res) {
  char const *const key_version = "version";
  char const *const key_synced = "synced";
  char const *const key_id = "identityID";
  char const *const key_pub_key = "publicKey";
  char const *const key_solid_msg_cnt = "solidMessageCount";
  char const *const key_total_msg_cnt = "totalMessageCount";
  int ret = 0;
  cJSON *json_obj = cJSON_Parse(j_str);
  if (json_obj == NULL) {
    return -1;
  }

  // FIXME: dose node info have error?
  // res_err_t *res_err = deser_error(json_obj);
  // if (res_err) {
  //   // got an error response
  //   return -1;
  // }

  // gets version
  if ((ret = json_get_string(json_obj, key_version, res->version, sizeof(res->version))) != 0) {
    printf("[%s:%d]: gets %s json string failed\n", __func__, __LINE__, key_version);
    ret = -1;
    goto end;
  }

  // gets ID
  if ((ret = json_get_string(json_obj, key_id, res->id, sizeof(res->id))) != 0) {
    printf("[%s:%d]: gets %s json string failed\n", __func__, __LINE__, key_id);
    ret = -1;
    goto end;
  }

  // gets sync status
  if ((ret = json_get_boolean(json_obj, key_synced, &res->is_synced)) != 0) {
    printf("[%s:%d]: gets %s boolean value failed\n", __func__, __LINE__, key_synced);
    ret = -1;
    goto end;
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}