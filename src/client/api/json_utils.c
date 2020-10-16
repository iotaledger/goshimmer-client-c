#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client/api/json_utils.h"

int json_get_string(cJSON const* const json_obj, char const key[], char str[], size_t str_len) {
  if (json_obj == NULL || key == NULL || str == NULL) {
    // invalid parameters
    printf("[%s:%d invalid parameters\n", __func__, __LINE__);
    return -1;
  }

  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, key);
  if (json_value == NULL) {
    printf("[%s:%d] JSON key not found %s\n", __func__, __LINE__, key);
    return -1;
  }

  if (cJSON_IsString(json_value) && (json_value->valuestring != NULL)) {
    strncpy(str, json_value->valuestring, str_len);
  } else {
    printf("[%s:%d] %s not string\n", __func__, __LINE__, key);
    return -1;
  }
  return 0;
}

int json_get_boolean(cJSON const* const json_obj, char const key[], bool* const value) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, key);
  if (json_value == NULL) {
    printf("[%s:%d] JSON key not found %s\n", __func__, __LINE__, key);
    return -1;
  }

  if (cJSON_IsBool(json_value)) {
    *value = cJSON_IsTrue(json_value);
  } else {
    printf("[%s:%d] %s is not boolean value\n", __func__, __LINE__, key);
    return -1;
  }
  return 0;
}

int json_get_int64(cJSON const* const json_obj, char const* const obj_name, int64_t* const num) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    printf("[%s:%d] %s is not found.\n", __func__, __LINE__, obj_name);
    return -1;
  }

  if (cJSON_IsNumber(json_value) && json_value->valuedouble >= 0) {
    *num = (int64_t)json_value->valuedouble;
  } else {
    printf("[%s:%d] %s is not number\n", __func__, __LINE__, obj_name);
    return -1;
  }

  return 0;
}
