#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "client/api/get_unspent_outputs.h"
#include "client/api/json_utils.h"
#include "client/api/response_error.h"
#include "client/network/http.h"
#include "utarray.h"
#include "utils/iota_str.h"

static int deser_output_ids(cJSON *j_ids, output_ids_t **ids) {
  char const key_id[] = "id";
  char const key_balances[] = "balances";
  char const key_value[] = "value";
  char const key_color[] = "color";
  char const key_st[] = "inclusion_state";
  char const key_solid[] = "solid";
  char const key_confirm[] = "confirmed";
  char const key_reject[] = "rejected";
  char const key_like[] = "liked";
  char const key_conflict[] = "conflicting";
  char const key_finalize[] = "finalized";
  char const key_prefer[] = "preferred";

  char color_str[BALANCE_COLOR_BASE58_BUF] = {};
  char output_id_str[TX_OUTPUT_ID_BASE58_BUF] = {};
  byte_t output_id[TX_OUTPUT_ID_BYTES] = {};

  int ret = -1;
  cJSON *ids_elm = NULL;

  cJSON_ArrayForEach(ids_elm, j_ids) {
    inclusion_state_t st = {};
    // get id from JSON
    json_get_string(ids_elm, key_id, output_id_str, sizeof(output_id_str));
    // printf("id: %s\n", output_id_str);
    tx_output_id_from_base58(output_id_str, strlen(output_id_str), output_id);

    // get inclusion state
    cJSON *j_state = cJSON_GetObjectItemCaseSensitive(ids_elm, key_st);
    if (j_state) {
      json_get_boolean(j_state, key_reject, &st.solid);
      json_get_boolean(j_state, key_confirm, &st.confirmed);
      json_get_boolean(j_state, key_reject, &st.rejected);
      json_get_boolean(j_state, key_like, &st.liked);
      json_get_boolean(j_state, key_conflict, &st.conflicting);
      json_get_boolean(j_state, key_finalize, &st.finalized);
      json_get_boolean(j_state, key_prefer, &st.preferred);
    }

    balance_ht_t *balances = balance_ht_init();
    // get balances
    cJSON *j_balances = cJSON_GetObjectItemCaseSensitive(ids_elm, key_balances);
    if (j_balances) {
      if (!cJSON_IsArray(j_balances)) {
        printf("[%s:%d] invalid JSON format, %s is not an array\n", __func__, __LINE__, key_balances);
        goto end;
      }
      cJSON *bal_elm = NULL;
      cJSON_ArrayForEach(bal_elm, j_balances) {
        balance_t bal = {};
        json_get_int64(bal_elm, key_value, &bal.value);
        json_get_string(bal_elm, key_color, color_str, BALANCE_COLOR_BASE58_BUF);
        if (strncmp("IOTA", color_str, 4) != 0) {
          balance_color_from_base58(color_str, bal.color);
        }
        balance_ht_add(&balances, bal.color, bal.value);
      }
    }
    output_ids_add(ids, output_id + TANGLE_ADDRESS_BYTES, balances, &st);
    // clean up
    balance_ht_free(&balances);
  }
  ret = 0;
end:
  return ret;
}

// 0 on success
static int request_builder(addr_list_t *addresses, http_buf_t *req) {
  int ret = 0;
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    printf("[%s:%d] OOM\n", __func__, __LINE__);
    return -1;
  }

  cJSON *j_addrs = cJSON_CreateArray();
  if (j_addrs == NULL) {
    ret = -1;
    printf("[%s:%d] OOM\n", __func__, __LINE__);
    goto end;
  }

  address_t *addr = NULL;
  char base58_addr[TANGLE_ADDRESS_BASE58_BUF];
  ADDR_LIST_FOREACH(addresses, addr) {
    address_2_base58(addr->addr, base58_addr);
    cJSON_AddItemToArray(j_addrs, cJSON_CreateString(base58_addr));
  }

  cJSON_AddItemToObject(json_root, "addresses", j_addrs);
  char *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text == NULL) {
    ret = -1;
  } else {
    http_buf_append(req, (byte_t *)json_text, strlen(json_text));
    http_buf2str(req);
    free(json_text);
  }

end:
  cJSON_Delete(json_root);
  return ret;
}

int deser_unspent_outputs(char const *const j_str, unspent_outputs_t **unspent) {
  char const key_unspent[] = "unspent_outputs";
  char const key_addr[] = "address";
  char const key_out_ids[] = "output_ids";

  int ret = -1;
  cJSON *json_obj = cJSON_Parse(j_str);
  if (json_obj == NULL) {
    printf("[%s:%d] invalid JSON object\n", __func__, __LINE__);
    return -1;
  }

  res_err_t *res_err = deser_error(json_obj);
  if (res_err) {
    // got an error response
    printf("[%s:%d] Error response: %s\n", __func__, __LINE__, res_err->msg);
    ret = 0;
    goto end;
  }

  cJSON *j_unspent = cJSON_GetObjectItemCaseSensitive(json_obj, key_unspent);
  if (j_unspent == NULL) {
    printf("[%s:%d] invalid JSON format, %s not found in\n%s\n", __func__, __LINE__, key_unspent, j_str);
    goto end;
  }

  if (!cJSON_IsArray(j_unspent)) {
    printf("[%s:%d] invalid JSON format, %s is not an array\n", __func__, __LINE__, key_unspent);
    goto end;
  }

  if (cJSON_GetArraySize(j_unspent) == 0) {
    printf("[%s:%d] invalid JSON format, %s is empty, %s\n", __func__, __LINE__, key_unspent, j_str);
    goto end;
  }

  char addr_str[TANGLE_ADDRESS_BASE58_BUF] = {};
  byte_t addr[TANGLE_ADDRESS_BYTES] = {};

  cJSON *unspent_elm = NULL;
  cJSON_ArrayForEach(unspent_elm, j_unspent) {
    // get address hash
    json_get_string(unspent_elm, key_addr, addr_str, sizeof(addr_str));
    // printf("addr: %s\n", addr_str);
    address_from_base58(addr_str, addr);

    // get output id objects
    output_ids_t *ids = output_ids_init();
    cJSON *j_ids = cJSON_GetObjectItemCaseSensitive(unspent_elm, key_out_ids);
    if (j_ids) {
      if (!cJSON_IsArray(j_ids)) {
        printf("[%s:%d] invalid JSON format, %s is not an array\n", __func__, __LINE__, key_out_ids);
        goto end;
      }

      ret = deser_output_ids(j_ids, &ids);
      if (ret != 0) {
        printf("[%s:%d] parsing output IDs failed\n", __func__, __LINE__);
        if (ids != NULL) {
          output_ids_free(&ids);
        }
        goto end;
      }
    }

    // set the index to 0 and update it later
    unspent_outputs_add(unspent, addr, 0, ids);
    // clean up
    if (ids != NULL) {
      output_ids_free(&ids);
    }
  }
  ret = 0;

end:
  cJSON_Delete(json_obj);
  return ret;
}

int get_unspent_outputs(tangle_client_conf_t const *conf, addr_list_t *addrs, unspent_outputs_t **unspent) {
  int ret = 0;
  char const *cmd_unspent_outputs = "value/unspentOutputs";
  // compose restful api command
  iota_str_t *cmd = iota_str_new(conf->url);
  if (cmd == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    return -1;
  }

  if (iota_str_append(cmd, cmd_unspent_outputs)) {
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
    ret = -1;
    goto done;
  }

  // build request
  if (request_builder(addrs, http_req) != 0) {
    printf("[%s:%d]: build request failed\n", __func__, __LINE__);
    ret = -1;
    goto done;
  }

  // printf("req: %s\n", http_req->data);

  // send request via http client
  if (http_client_post(&http_conf, http_req, http_res) != 0) {
    printf("[%s:%d]: http client post failed\n", __func__, __LINE__);
    ret = -1;
    goto done;
  }
  http_buf2str(http_res);

  // printf("[%s:%d] res: %s\n", __func__, __LINE__, http_res->data);

  // json deserialization
  ret = deser_unspent_outputs((char const *const)http_res->data, unspent);

done:
  // cleanup command
  iota_str_destroy(cmd);
  http_buf_free(http_res);
  http_buf_free(http_req);
  return ret;
}
