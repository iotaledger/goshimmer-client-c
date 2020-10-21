#include "core/output_ids.h"

int output_ids_add(output_ids_t **t, byte_t const id[], balance_ht_t *balances, inclusion_state_t *st) {
  output_ids_t *elm = output_ids_find(t, id);
  if (elm) {
    // printf("[%s:%d] output id exists in table\n", __func__, __LINE__);
    return 0;
  }

  // adding to table
  elm = malloc(sizeof(output_ids_t));
  if (elm == NULL) {
    printf("[Err %s:%d] OOM\n", __func__, __LINE__);
    return -1;
  }
  elm->balances = balance_ht_clone(&balances);
  memcpy(elm->id, id, TX_ID_BYTES);
  memcpy(&elm->st, st, sizeof(inclusion_state_t));
  HASH_ADD(hh, *t, id, TX_ID_BYTES, elm);
  return 0;
}

int output_ids_update(output_ids_t **t, byte_t const id[], balance_ht_t *balances, inclusion_state_t *st) {
  output_ids_remove(t, id);
  return output_ids_add(t, id, balances, st);
}

output_ids_t *output_ids_clone(output_ids_t **t) {
  output_ids_t *dst = output_ids_init();
  output_ids_t *src, *tmp;
  HASH_ITER(hh, *t, src, tmp) { output_ids_add(&dst, src->id, src->balances, &src->st); }
  return dst;
}

uint64_t output_ids_balance(output_ids_t **t) {
  uint64_t sum = 0;
  output_ids_t *elm, *tmp;
  HASH_ITER(hh, *t, elm, tmp) {
    if (elm->st.confirmed) {
      sum += balance_ht_sum(&elm->balances);
    }
  }
  return sum;
}

void output_ids_print(output_ids_t **t) {
  output_ids_t *elm, *tmp;
  char id_str[TX_ID_BASE58_BUF] = {};
  printf("output_ids: [\n===\n");
  HASH_ITER(hh, *t, elm, tmp) {
    tx_id_2_base58(elm->id, id_str);
    printf("id: %s\n", id_str);
    inclustion_state_print(&elm->st);
    balance_ht_print(&elm->balances);
    printf("===\n");
  }
  printf("]\n");
}

void inclustion_state_print(inclusion_state_t *st) {
  printf("inclusion_state: [ ");
  if (st->confirmed) {
    printf("confirmed: true, ");
  }
  if (st->conflicting) {
    printf("conflicting: true, ");
  }
  if (st->finalized) {
    printf("finalized: true, ");
  }
  if (st->liked) {
    printf("liked: true, ");
  }
  if (st->preferred) {
    printf("preferred: true, ");
  }
  if (st->rejected) {
    printf("rejected: true, ");
  }
  if (st->solid) {
    printf("solid: true ");
  }
  printf(" ]\n");
}
