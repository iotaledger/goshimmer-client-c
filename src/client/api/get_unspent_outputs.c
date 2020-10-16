#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "client/api/get_unspent_outputs.h"
#include "client/api/json_utils.h"
#include "client/api/response_error.h"
#include "utarray.h"

static void output_id_icd_init(void *elm) {
  output_id_t *id = (output_id_t *)elm;
  id->balances = NULL;
  memset(&(id->st), 0, sizeof(inclusion_state_t));
  memset(id->output_id, 0, TX_OUTPUT_ID_BASE58_BUF);
}

static void output_id_icd_copy(void *_dst, void const *_src) {
  output_id_t *dst = (output_id_t *)_dst;
  output_id_t *src = (output_id_t *)_src;
  memcpy(dst->output_id, src->output_id, TX_OUTPUT_ID_BASE58_BUF);

  dst->balances = balance_list_clone(src->balances);
  if (dst->balances == NULL) {
    printf("[%s:%d] clone balance list failed\n", __func__, __LINE__);
  }
}

static void output_id_icd_dtor(void *_elm) {
  output_id_t *elm = (output_id_t *)_elm;
  balance_list_free(elm->balances);
}

static UT_icd const ut_output_id_icd = {sizeof(output_id_t), output_id_icd_init, output_id_icd_copy,
                                        output_id_icd_dtor};

static void unspent_icd_init(void *elm) {
  res_unspent_t *unspent = (res_unspent_t *)elm;
  memset(unspent->address, 0, TANGLE_ADDRESS_BASE58_BUF);
  unspent->output_ids = NULL;
}

static void unspent_icd_copy(void *_dst, void const *_src) {
  res_unspent_t *dst = (res_unspent_t *)_dst;
  res_unspent_t *src = (res_unspent_t *)_src;
  memcpy(dst->address, src->address, TANGLE_ADDRESS_BASE58_BUF);

  dst->output_ids = output_id_list_clone(src->output_ids);
  if (dst->output_ids == NULL) {
    printf("[%s:%d] clone balance list failed\n", __func__, __LINE__);
  }
}

static void unspent_icd_dtor(void *_elm) {
  res_unspent_t *elm = (res_unspent_t *)_elm;
  output_id_list_free(elm->output_ids);
}

static UT_icd const ut_unspent_icd = {sizeof(res_unspent_t), unspent_icd_init, unspent_icd_copy, unspent_icd_dtor};

static int deser_output_ids(cJSON *j_ids, output_id_list_t *ids) {
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
  char balance_color[BALANCE_COLOR_BASE58_BUF];

  int ret = -1;
  cJSON *ids_elm = NULL;
  cJSON_ArrayForEach(ids_elm, j_ids) {
    // creates a new output id object
    output_id_t id = {};
    // get id from JSON
    json_get_string(ids_elm, key_id, (char *)id.output_id, sizeof(id.output_id));
    printf("id: %s\n", id.output_id);

    // get inclusion state
    cJSON *j_state = cJSON_GetObjectItemCaseSensitive(ids_elm, key_st);
    if (j_state) {
      if (json_get_boolean(j_state, key_reject, &id.st.solid) == 0) {
        printf("%s : %s\n", key_solid, id.st.solid ? "true" : "false");
      }
      if (json_get_boolean(j_state, key_confirm, &id.st.confirmed) == 0) {
        printf("%s : %s\n", key_confirm, id.st.confirmed ? "true" : "false");
      }
      if (json_get_boolean(j_state, key_reject, &id.st.rejected) == 0) {
        printf("%s : %s\n", key_reject, id.st.rejected ? "true" : "false");
      }
      if (json_get_boolean(j_state, key_like, &id.st.liked) == 0) {
        printf("%s : %s\n", key_like, id.st.liked ? "true" : "false");
      }
      if (json_get_boolean(j_state, key_conflict, &id.st.conflicting) == 0) {
        printf("%s : %s\n", key_conflict, id.st.conflicting ? "true" : "false");
      }
      if (json_get_boolean(j_state, key_finalize, &id.st.finalized) == 0) {
        printf("%s : %s\n", key_finalize, id.st.finalized ? "true" : "false");
      }
      if (json_get_boolean(j_state, key_prefer, &id.st.preferred) == 0) {
        printf("%s : %s\n", key_prefer, id.st.preferred ? "true" : "false");
      }
    }

    // get balances
    cJSON *j_balances = cJSON_GetObjectItemCaseSensitive(ids_elm, key_balances);
    if (j_balances) {
      if (!cJSON_IsArray(j_balances)) {
        printf("[%s:%d] invalid JSON format, %s is not an array\n", __func__, __LINE__, key_balances);
        goto end;
      }
      id.balances = balance_list_new();
      cJSON *bal_elm = NULL;
      cJSON_ArrayForEach(bal_elm, j_balances) {
        balance_t bal = {};
        json_get_int64(bal_elm, key_value, &bal.value);
        // printf("value: %" PRId64 "\n", bal.value);
        json_get_string(bal_elm, key_color, balance_color, BALANCE_COLOR_BASE58_BUF);
        // printf("color: %s\n", tmp_str);
        if (strncmp("IOTA", balance_color, 4) != 0) {
          balance_color_from_base58(balance_color, bal.color);
        }
        // balance_print(&bal);
        // add balance to output id
        balance_list_push(id.balances, &bal);
      }
    }
    // add id to list
    output_id_list_push(ids, &id);
    // clean up
    balance_list_free(id.balances);
  }
  ret = 0;
end:
  return ret;
}

output_id_list_t *output_id_list_new() {
  output_id_list_t *list = NULL;
  utarray_new(list, &ut_output_id_icd);
  return list;
}

output_id_list_t *output_id_list_clone(output_id_list_t *src) {
  output_id_list_t *dst = output_id_list_new();
  if (dst == NULL) {
    printf("[%s:%d] OOM\n", __func__, __LINE__);
    return NULL;
  }

  output_id_t *id = NULL;
  OUTPUT_ID_FOREACH(src, id) { output_id_list_push(dst, id); }

  return dst;
}

res_unspent_outs_t *unspent_list_new() {
  res_unspent_outs_t *list = NULL;
  utarray_new(list, &ut_unspent_icd);
  return list;
}

int deser_unspent_outputs(char const *const j_str, res_unspent_outs_t *res) {
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

  cJSON *unspent_elm = NULL;
  char tmp_str[256];
  cJSON_ArrayForEach(unspent_elm, j_unspent) {
    res_unspent_t unspent = {};

    // get address hash
    json_get_string(unspent_elm, key_addr, (char *)unspent.address, sizeof(unspent.address));
    printf("addr: %s\n", unspent.address);

    // get output id objects
    cJSON *j_ids = cJSON_GetObjectItemCaseSensitive(unspent_elm, key_out_ids);
    if (j_ids) {
      if (!cJSON_IsArray(j_ids)) {
        printf("[%s:%d] invalid JSON format, %s is not an array\n", __func__, __LINE__, key_out_ids);
        goto end;
      }
      if (cJSON_GetArraySize(j_ids) == 0) {
        printf("output_ids: []\n");
        continue;
      }
      unspent.output_ids = output_id_list_new();
      if (unspent.output_ids == NULL) {
        printf("[%s:%d] OOM\n", __func__, __LINE__);
        goto end;
      }
      ret = deser_output_ids(j_ids, unspent.output_ids);
    }

    // adds to response object
    unspent_list_push(res, &unspent);
    // clean up
    if (unspent.output_ids != NULL) {
      output_id_list_free(unspent.output_ids);
    }
  }
  ret = 0;

end:
  cJSON_Delete(json_obj);
  return ret;
}
