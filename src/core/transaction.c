#include <inttypes.h>

#include "core/transaction.h"
#include "libbase58.h"

static UT_icd const ut_inputs_icd = {sizeof(byte_t) * TX_OUTPUT_ID_BYTES, NULL, NULL, NULL};

static void outputs_icd_copy(void *_dst, void const *_src) {
  tx_output_t *dst = (tx_output_t *)_dst;
  tx_output_t *src = (tx_output_t *)_src;
  memcpy(dst->address, src->address, TANGLE_ADDRESS_BYTES);
  dst->balances = balance_list_clone(src->balances);
  if (dst->balances == NULL) {
    printf("[%s:%d] clone balance list failed\n", __func__, __LINE__);
  }
}

static void outputs_icd_dtor(void *_elm) {
  tx_output_t *elm = (tx_output_t *)_elm;
  balance_list_free(elm->balances);
}

static UT_icd const ut_outputs_icd = {sizeof(tx_output_t), NULL, outputs_icd_copy, outputs_icd_dtor};

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

byte_buf_t *tx_essence(transaction_t *tx) {
  byte_buf_t *essence = byte_buf_new();
  // size_t essence_bytes = 0;

  // inputs bytes
  essence->len = sizeof(uint32_t);
  essence->len += tx_inputs_len(tx->inputs) * TX_OUTPUT_ID_BYTES;
  // output bytes
  essence->len += sizeof(uint32_t);
  tx_output_t *out_elm = NULL;
  TX_OUTPUTS_FOREACH(tx->outputs, out_elm) {
    essence->len += TANGLE_ADDRESS_BYTES;
    // balance bytes
    essence->len += sizeof(uint32_t);
    essence->len += balance_list_len(out_elm->balances) * (sizeof(int64_t) + BALANCE_COLOR_BYTES);
  }
  // data payload bytes
  essence->len += sizeof(uint32_t);
  // TODO: add payload size to essence_bytes

  essence->data = malloc(essence->len);
  essence->cap = essence->len;
  if (essence->data == NULL) {
    printf("[%s:%d] OOM\n", __func__, __LINE__);
    byte_buf_free(essence);
    return NULL;
  }

  if (is_little_endian()) {
    size_t offset = 0;
    // input size
    uint32_t u32_v = tx_inputs_len(tx->inputs);
    byte_t *b = NULL;
    memcpy(essence->data, &u32_v, sizeof(u32_v));
    offset += sizeof(u32_v);
    // inputs size
    TX_INPUTS_FOREACH(tx->inputs, b) {
      memcpy(essence->data + offset, b, TX_OUTPUT_ID_BYTES);
      offset += TX_OUTPUT_ID_BYTES;
    }

    // output size
    u32_v = tx_outputs_len(tx->outputs);
    memcpy(essence->data + offset, &u32_v, sizeof(u32_v));
    offset += sizeof(u32_v);

    // outputs
    out_elm = NULL;
    TX_OUTPUTS_FOREACH(tx->outputs, out_elm) {
      memcpy(essence->data + offset, out_elm->address, TANGLE_ADDRESS_BYTES);
      offset += TANGLE_ADDRESS_BYTES;

      // balance bytes
      u32_v = balance_list_len(out_elm->balances);
      memcpy(essence->data + offset, &u32_v, sizeof(uint32_t));
      offset += sizeof(uint32_t);
      balance_t *balance = NULL;
      BALANCE_LIST_FOREACH(out_elm->balances, balance) {
        memcpy(essence->data + offset, &balance->value, sizeof(int64_t));
        offset += sizeof(balance->value);
        memcpy(essence->data + offset, balance->color, BALANCE_COLOR_BYTES);
        offset += BALANCE_COLOR_BYTES;
      }
    }

    // payload
    memset(essence->data + offset, 0, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    if (offset != essence->len) {
      printf("[%s:%d] unexpected length of essence\n", __func__, __LINE__);
    }

    return essence;

  } else {
    // TODO
    return NULL;
  }

  return NULL;
}

bool tx_signautres_valid(transaction_t *tx) {
  if (tx->inputs == NULL || tx->signatures == NULL) {
    return false;
  }

  byte_t *input = NULL;
  ed_signature_t *sig = NULL;
  // byte_t tmp_addr[TANGLE_ADDRESS_BYTES];
  TX_INPUTS_FOREACH(tx->inputs, input) {
    // is address in signature table?
    sig = ed_signatures_find(&tx->signatures, input);
    if (sig == NULL) {
      return false;
    }

    // is signature valid?
    byte_buf_t *essence = tx_essence(tx);
    if (essence) {
      if (sign_verify_signature(sig->signature, essence->data, essence->len, sig->pub_key) == false) {
        byte_buf_free(essence);
        return false;
      }
      byte_buf_free(essence);
    }
  }
  return true;
}

void tx_print(transaction_t *tx) {
  if (tx == NULL) {
    printf("tx: {}n");
    return;
  } else {
    printf("tx: {\n");
  }

  if (tx->inputs) {
    tx_inputs_print(tx->inputs);
  } else {
    printf("inputs: []\n");
  }

  if (tx->outputs) {
    tx_outputs_print(tx->outputs);
  } else {
    printf("outputs: []\n");
  }

  if (tx->signatures) {
    ed_signatures_print(&tx->signatures);
  } else {
    printf("signatures: []\n");
  }
  printf("}\n");
}

byte_buf_t *tx_2_bytes_string(transaction_t *tx) {
  byte_buf_t *bytes = tx_essence(tx);
  byte_t addr_version = 1;  // ed25519 scheme
  size_t signature_bytes = ed_signatures_count(&tx->signatures) * (1 + ED_PUBLIC_KEY_BYTES + ED_SIGNATURE_BYTES);
  size_t tx_bytes_len = bytes->len + signature_bytes + 1;
  byte_buf_reserve(bytes, tx_bytes_len);

  ed_signature_t *elm, *tmp;
  HASH_ITER(hh, tx->signatures, elm, tmp) {
    byte_buf_append(bytes, &addr_version, 1);
    byte_buf_append(bytes, elm->pub_key, ED_PUBLIC_KEY_BYTES);
    byte_buf_append(bytes, elm->signature, ED_SIGNATURE_BYTES);
  }
  // trailing 0 to indicate the end of signatures
  byte_t end = 0x0;
  byte_buf_append(bytes, &end, 1);
  // bin to hex string
  byte_buf_t *str = byte_buf2hex_string(bytes);
  // clean up
  byte_buf_free(bytes);
  return str;
}
