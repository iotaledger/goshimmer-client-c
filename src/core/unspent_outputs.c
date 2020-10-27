#include <inttypes.h>

#include "core/unspent_outputs.h"

int unspent_outputs_add(unspent_outputs_t **t, byte_t const addr[], uint64_t addr_index, output_ids_t *ids) {
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
  elm->spent = false;
  elm->addr_index = addr_index;
  elm->ids = output_ids_clone(&ids);
  HASH_ADD(hh, *t, addr, TANGLE_ADDRESS_BYTES, elm);
  return 0;
}

int unspent_outputs_update(unspent_outputs_t **t, byte_t const addr[], output_ids_t *ids) {
  unspent_outputs_t *elm = unspent_outputs_find(t, addr);
  if (elm) {
    output_ids_free(&elm->ids);
    elm->ids = output_ids_clone(&ids);
  }
  return 0;
}

int unspent_outputs_append_id(unspent_outputs_t **t, byte_t const addr[], output_ids_t *ids) {
  unspent_outputs_t *elm = unspent_outputs_find(t, addr);
  if (elm) {
    output_ids_t *id_elm, *id_tmp;
    HASH_ITER(hh, ids, id_elm, id_tmp) { output_ids_add(&elm->ids, id_elm->id, id_elm->balances, &id_elm->st); }
  }
  return 0;
}

unspent_outputs_t *unspent_outputs_clone(unspent_outputs_t **t) {
  unspent_outputs_t *dst = unspent_outputs_init();
  unspent_outputs_t *src, *tmp;
  HASH_ITER(hh, *t, src, tmp) { unspent_outputs_add(&dst, src->addr, src->addr_index, src->ids); }
  return dst;
}

void unspent_outputs_set_spent(unspent_outputs_t **t, byte_t addr[], bool spent) {
  unspent_outputs_t *elm = unspent_outputs_find(t, addr);
  if (elm == NULL) {
    return;
  }
  elm->spent = spent;
}

bool unspent_outputs_get_spent(unspent_outputs_t **t, byte_t addr[]) {
  unspent_outputs_t *elm = unspent_outputs_find(t, addr);
  if (elm == NULL) {
    return false;
  }
  return elm->spent;
}

uint64_t unspent_outputs_balance(unspent_outputs_t **t) {
  unspent_outputs_t *elm, *tmp;
  uint64_t sum = 0;
  HASH_ITER(hh, *t, elm, tmp) {
    if (elm->spent == false && elm->ids) {
      sum += output_ids_balance(&elm->ids);
    }
  }
  return sum;
}

uint64_t unspent_outputs_balance_with_color(unspent_outputs_t **t, byte_t color[]) {
  unspent_outputs_t *elm, *tmp;
  uint64_t sum = 0;
  HASH_ITER(hh, *t, elm, tmp) {
    if (elm->spent == false && elm->ids) {
      sum += output_ids_balance_with_color(&elm->ids, color);
    }
  }
  return sum;
}

unspent_outputs_t *unspent_outputs_required_outputs(unspent_outputs_t **t, uint64_t required_balance, byte_t color[]) {
  unspent_outputs_t *required_outputs = unspent_outputs_init();
  unspent_outputs_t *elm, *tmp;
  uint64_t sum = 0;
  HASH_ITER(hh, *t, elm, tmp) {
    if (elm->spent == false && elm->ids) {
      sum += output_ids_balance_with_color(&elm->ids, color);
      unspent_outputs_add(&required_outputs, elm->addr, elm->addr_index, elm->ids);
      if (sum >= required_balance) {
        break;
      }
    }
  }
  return required_outputs;
}

void unspent_outputs_print(unspent_outputs_t **t) {
  unspent_outputs_t *elm, *tmp;
  char addr_str[TANGLE_ADDRESS_BASE58_BUF] = {};
  char output_id_str[TX_OUTPUT_ID_BASE58_BUF] = {};
  byte_t output_id[TX_OUTPUT_ID_BYTES] = {};

  printf("unspent_outputs: [\n===\n");
  HASH_ITER(hh, *t, elm, tmp) {
    address_2_base58(elm->addr, addr_str);
    printf("address[%" PRIu64 "]: %s %s\n", elm->addr_index, addr_str, elm->spent ? "[spent]" : "");
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
