#include <inttypes.h>

#include "core/transaction.h"
#include "libbase58.h"

static UT_icd const ut_inputs_icd = {sizeof(byte_t) * TX_OUTPUT_ID_BYTES, NULL, NULL, NULL};
static UT_icd const ut_outputs_icd = {sizeof(tx_output_t), NULL, NULL, NULL};

void tx_id_random(byte_t id[]) { randombytes_buf((void *const)id, TX_ID_BYTES); }

bool tx_id_2_base58(byte_t id[], char str_buf[], size_t *buf_len) {
  return b58enc((char *)str_buf, buf_len, (const void *)id, TX_ID_BYTES);
}

void tx_output_id_random(byte_t output_id[]) { randombytes_buf((void *const)output_id, TX_OUTPUT_ID_BYTES); }

void tx_output_id(byte_t addr[], byte_t id[], byte_t output_id[]) {
  memcpy(output_id, addr, TANGLE_ADDRESS_BYTES);
  memcpy(output_id + TANGLE_ADDRESS_BYTES, id, TX_ID_BYTES);
}

bool tx_output_id_2_base58(byte_t output_id[], char str_buf[], size_t *buf_len) {
  return b58enc((char *)str_buf, buf_len, (const void *)output_id, TX_OUTPUT_ID_BYTES);
}

bool tx_output_id_from_base58(char str_buf[], size_t str_len, byte_t output_id[]) {
  size_t id_len = TX_OUTPUT_ID_BYTES;
  return b58tobin((void *)output_id, &id_len, str_buf, str_len);
}

tx_inputs_t *tx_inputs_new() {
  tx_inputs_t *tx_inputs = NULL;
  utarray_new(tx_inputs, &ut_inputs_icd);
  return tx_inputs;
}

void tx_inputs_push_base58(tx_inputs_t *tx_in, char base58[]) {
  byte_t output_id[TX_OUTPUT_ID_BYTES] = {};
  if (tx_output_id_from_base58(base58, strlen(base58), output_id)) {
    utarray_push_back(tx_in, output_id);
  } else {
    printf("[%s:%d] base58 decode failed\n", __func__, __LINE__);
  }
}

void tx_inputs_print(tx_inputs_t *tx_in) {
  byte_t *output_id = NULL;
  char output_id_str[TX_OUTPUT_ID_BASE58_BUF];
  size_t id_len = TX_OUTPUT_ID_BASE58_BUF;
  printf("inputs:[\n");
  TX_INPUTS_FOREACH(tx_in, output_id) {
    tx_output_id_2_base58(output_id, output_id_str, &id_len);
    printf("  %s\n", output_id_str);
  }
  printf("]\n");
}

tx_outputs_t *tx_outputs_new() {
  tx_inputs_t *tx_outputs = NULL;
  utarray_new(tx_outputs, &ut_outputs_icd);
  return tx_outputs;
}

void tx_outputs_print(tx_outputs_t *tx_out) {
  char addr[TANGLE_ADDRESS_BASE58_BUF];
  tx_output_t *o = NULL;
  printf("outputs:[\n");
  TX_OUTPUTS_FOREACH(tx_out, o) {
    address_2_base58(o->address, addr);
    printf("  %s: [\n", addr);
    balance_list_print(o->balances);
    printf("  ]\n");
  }
  printf("]\n");
}