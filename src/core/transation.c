#include <inttypes.h>

#include "core/transation.h"
#include "libbase58.h"

static UT_icd const ut_inputs_icd = {sizeof(byte_t) * OUTPUT_ID_BYTES, NULL, NULL, NULL};
static UT_icd const ut_outputs_icd = {sizeof(tx_output_t), NULL, NULL, NULL};

void tx_id_random(byte_t id[]) { randombytes_buf((void *const)id, TRANSACTION_ID_BYTES); }

bool tx_id_2_base58(char str_buf[], size_t *buf_len, byte_t id[]) {
  return b58enc((char *)str_buf, buf_len, (const void *)id, TRANSACTION_ID_BYTES);
}

void tx_output_id_random(byte_t output_id[], byte_t addr[]) {
  // output id is constructed by address and id.
  memcpy(output_id, addr, TANGLE_ADDRESS_BYTES);
  randombytes_buf((void *const)output_id + TANGLE_ADDRESS_BYTES, TRANSACTION_ID_BYTES);
}

void tx_output_id(byte_t output_id[], byte_t addr[], byte_t id[]) {
  memcpy(output_id, addr, TANGLE_ADDRESS_BYTES);
  memcpy(output_id + TANGLE_ADDRESS_BYTES, id, TRANSACTION_ID_BYTES);
}

bool tx_output_id_2_base58(char str_buf[], size_t *buf_len, byte_t output_id[]) {
  return b58enc((char *)str_buf, buf_len, (const void *)output_id, OUTPUT_ID_BYTES);
}

tx_inputs_t *tx_inputs_new() {
  tx_inputs_t *tx_inputs = NULL;
  utarray_new(tx_inputs, &ut_inputs_icd);
  return tx_inputs;
}

void tx_inputs_print(tx_inputs_t *tx_in) {
  byte_t *output_id = NULL;
  char output_id_str[OUTPUT_ID_BASE58_LEN];
  size_t id_len = OUTPUT_ID_BASE58_LEN;
  printf("inputs:[\n");
  TX_INPUTS_FOREACH(tx_in, output_id) {
    tx_output_id_2_base58(output_id_str, &id_len, output_id);
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
  char color[BALANCE_COLOR_BASE58_BUF];
  tx_output_t *o = NULL;
  printf("inputs:[\n");
  TX_OUTPUTS_FOREACH(tx_out, o) {
    address_2_base58(addr, o->address);
    printf("  %s: [\n", addr);
    balance_color_2_base58(color, o->balance.color);
    printf("    %" PRId64 ", %s\n", o->balance.value, color);
    printf("  ]\n");
  }
  printf("]\n");
}