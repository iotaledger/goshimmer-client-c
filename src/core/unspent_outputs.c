#include "core/unspent_outputs.h"

int unspent_outputs_add(unspent_outputs_t **t, byte_t const addr[], output_ids_t *ids) {
  unspent_outputs_t *elm = unspent_outputs_find(t, addr);
  if (elm) {
    printf("[%s:%d] address exists in table\n", __func__, __LINE__);
    return -1;
  }

  // adding to table
  elm = malloc(sizeof(unspent_outputs_t));
  if (elm == NULL) {
    printf("[Err %s:%d] OOM\n", __func__, __LINE__);
    return -1;
  }
  memcpy(elm->addr, addr, TANGLE_ADDRESS_BYTES);
  elm->ids = output_ids_clone(&ids);
  HASH_ADD(hh, *t, addr, TANGLE_ADDRESS_BYTES, elm);
  return 0;
}

int unspent_outputs_update(unspent_outputs_t **t, byte_t const addr[], output_ids_t *ids) {
  unspent_outputs_remove(t, addr);
  return unspent_outputs_add(t, addr, ids);
}

int unspent_outputs_append_id(unspent_outputs_t **t, byte_t const addr[], output_ids_t *ids) {
  unspent_outputs_t *elm = unspent_outputs_find(t, addr);
  if (!elm) {
    return unspent_outputs_add(t, addr, ids);
  } else {
    output_ids_t *id_elm, *id_tmp;
    HASH_ITER(hh, ids, id_elm, id_tmp) { output_ids_add(&elm->ids, id_elm->id, id_elm->balances, &id_elm->st); }
  }
  return 0;
}

unspent_outputs_t *unspent_outputs_clone(unspent_outputs_t **t) {
  unspent_outputs_t *dst = unspent_outputs_init();
  unspent_outputs_t *src, *tmp;
  HASH_ITER(hh, *t, src, tmp) { unspent_outputs_add(&dst, src->addr, src->ids); }
  return dst;
}

void unspent_outputs_print(unspent_outputs_t **t) {
  unspent_outputs_t *elm, *tmp;
  char addr_str[TANGLE_ADDRESS_BASE58_BUF] = {};
  char output_id_str[TX_OUTPUT_ID_BASE58_BUF] = {};
  byte_t output_id[TX_OUTPUT_ID_BYTES] = {};

  printf("unspent_outputs: [\n===\n");
  HASH_ITER(hh, *t, elm, tmp) {
    address_2_base58(elm->addr, addr_str);
    printf("address: %s\n", addr_str);
    output_ids_t *id_elm, *id_tmp;
    HASH_ITER(hh, elm->ids, id_elm, id_tmp) {
      memcpy(output_id, elm->addr, TANGLE_ADDRESS_BYTES);
      memcpy(output_id + TANGLE_ADDRESS_BYTES, id_elm->id, TX_ID_BYTES);
      tx_output_id_2_base58(output_id, output_id_str);
      printf("id: %s\n", output_id_str);
      inclustion_state_print(&id_elm->st);
      balance_ht_print(&id_elm->balances);
    }
    printf("===\n");
  }
  printf("]\n");
}
