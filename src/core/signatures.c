#include "core/signatures.h"

int ed_signatures_add(ed_signature_t** t, byte_t const addr[], byte_t const pub_key[], byte_t const sig[]) {
  ed_signature_t* e = NULL;
  // checking uniqueness
  e = ed_signatures_find(t, addr);
  if (e != NULL) {
    printf("[%s:%d] address exists in table\n", __func__, __LINE__);
    return -1;
  }

  // adding to table
  e = malloc(sizeof(ed_signature_t));
  if (!e) {
    printf("[Err %s:%d] OOM\n", __func__, __LINE__);
    return -1;
  }
  memcpy(e->address, addr, TANGLE_ADDRESS_BYTES);
  memcpy(e->pub_key, pub_key, ED_PUBLIC_KEY_BYTES);
  memcpy(e->signature, sig, ED_SIGNATURE_BYTES);
  HASH_ADD(hh, *t, address, TANGLE_ADDRESS_BYTES, e);
  return 0;
}

void ed_signatures_print(ed_signature_t** t) {
  ed_signature_t *elm, *tmp;
  char addr_str[TANGLE_ADDRESS_BASE58_BUF] = {};
  size_t counter = 0;
  printf("signatures: [\n");
  HASH_ITER(hh, *t, elm, tmp) {
    address_2_base58(elm->address, addr_str);
    printf("[%zu] address: %s [", counter, addr_str);
    printf("\n    public key: ");
    dump_hex(elm->pub_key, ED_PUBLIC_KEY_BYTES);
    printf("    signature: ");
    dump_hex(elm->signature, ED_SIGNATURE_BYTES);
    printf("  ]\n");
    counter++;
  }
  printf("]\n");
}
