#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libbase58.h"

#include "core/address.h"

static UT_icd const addr_list_icd = {sizeof(address_t), NULL, NULL, NULL};

static void address_from_ed25519(byte_t const seed[], uint64_t index, byte_t addr_out[]) {
  int digest_len = 32;
  // public key of the seed
  byte_t pub_key[ED_PUBLIC_KEY_BYTES];
  byte_t priv_key[ED_PRIVATE_KEY_BYTES];
  address_ed25519_keypair(seed, index, pub_key, priv_key);

  // digest: blake2b the public key
  byte_t digest[digest_len];
  crypto_generichash(digest, digest_len, pub_key, ED_PUBLIC_KEY_BYTES, NULL, 0);

  // address[0] = version, address[1:] = digest
  addr_out[0] = ADDRESS_VER_ED25519;
  memcpy((void *)(addr_out + 1), digest, digest_len);
}

/**
 * @brief hexadecimal text to a string, ex: "48656c6c6f" -> "Hello"
 *
 * @param str the hex text,
 * @param array output string
 */
void hex_decode_string(char const str[], uint8_t array[]) {
  size_t len = strlen(str) / 2;
  for (size_t i = 0; i < len; i++) {
    uint8_t c = 0;
    if (str[i * 2] >= '0' && str[i * 2] <= '9') {
      c += (str[i * 2] - '0') << 4;
    }
    if ((str[i * 2] & ~0x20) >= 'A' && (str[i * 2] & ~0x20) <= 'F') {
      c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
    }
    if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9') {
      c += (str[i * 2 + 1] - '0');
    }
    if ((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F') {
      c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
    }
    array[i] = c;
  }
}

void random_seed(byte_t seed[]) { randombytes_buf((void *const)seed, TANGLE_SEED_BYTES); }

bool seed_2_base58(byte_t const seed[], char str_buf[]) {
  size_t len = TANGLE_ADDRESS_BASE58_BUF;
  return b58enc((char *)str_buf, &len, (const void *)seed, TANGLE_SEED_BYTES);
}

bool seed_from_base58(char const str[], byte_t out_seed[]) {
  size_t out_len = TANGLE_SEED_BYTES;
  return b58tobin((void *)out_seed, &out_len, str, strlen(str));
}

// subSeed generates the n'th sub seed of this Seed which is then used to generate the KeyPair.
static void get_subseed(byte_t const seed[], uint64_t index, byte_t subseed[]) {
  byte_t hash_index[32];
  uint8_t bytes_index[8];

  if (is_little_endian()) {
    memcpy(bytes_index, &index, 8);
    crypto_generichash(hash_index, 32, bytes_index, 8, NULL, 0);
  } else {
    // convert index to 8-byte-array in little-endian
    // TODO: hardware optimization
    bytes_index[0] = index >> 8 * 0;
    bytes_index[1] = index >> 8 * 1;
    bytes_index[2] = index >> 8 * 2;
    bytes_index[3] = index >> 8 * 3;
    bytes_index[4] = index >> 8 * 4;
    bytes_index[5] = index >> 8 * 5;
    bytes_index[6] = index >> 8 * 6;
    bytes_index[7] = index >> 8 * 7;

    // hash index-byte
    crypto_generichash(hash_index, 32, bytes_index, 8, NULL, 0);
  }

  // XOR subseed and hashedIndexBytes
  memcpy(subseed, seed, TANGLE_SEED_BYTES);
  // TODO: hardware optimization
  for (int i = 0; i < TANGLE_SEED_BYTES; i++) {
    subseed[i] = subseed[i] ^ hash_index[i];
  }
}

void address_get(byte_t const seed[], uint64_t index, address_version_t version, byte_t addr_out[]) {
  if (version == ADDRESS_VER_ED25519) {
    address_from_ed25519(seed, index, addr_out);
  } else {
    // TODO
    printf("TODO");
  }
}

bool address_2_base58(byte_t const address[], char str_buf[]) {
  size_t buf_len = TANGLE_ADDRESS_BASE58_BUF;
  return b58enc(str_buf, &buf_len, (const void *)address, TANGLE_ADDRESS_BYTES);
}

bool address_from_base58(char const base58_str[], byte_t addr[]) {
  size_t addr_len = TANGLE_ADDRESS_BYTES;
  return b58tobin((void *)addr, &addr_len, base58_str, strlen(base58_str));
}

void address_ed25519_keypair(byte_t const seed[], uint64_t index, byte_t pub[], byte_t priv[]) {
  byte_t subseed[TANGLE_SEED_BYTES];
  // get subseed from seed
  get_subseed(seed, index, subseed);
  // get ed25519 public and private key from subseed
  crypto_sign_seed_keypair(pub, priv, subseed);
}

// signs the message with privateKey and returns a signature.
void sign_signature(byte_t const seed[], uint64_t index, byte_t const data[], uint64_t data_len, byte_t signature[]) {
  //
  byte_t pub_key[ED_PUBLIC_KEY_BYTES];
  byte_t priv_key[ED_PRIVATE_KEY_BYTES];
  byte_t subseed[TANGLE_SEED_BYTES];
  unsigned long long sign_len = 0;
  // get subseed from seed
  get_subseed(seed, index, subseed);
  // get ed25519 public and private key from subseed
  crypto_sign_seed_keypair(pub_key, priv_key, subseed);

  // crypto_sign(signature, &sign_len, data, data_len, priv_key);
  crypto_sign_detached(signature, &sign_len, data, data_len, priv_key);
}

bool sign_verify_signature(byte_t signature[], byte_t const data[], size_t data_len, byte_t pub_key[]) {
  if (crypto_sign_verify_detached(signature, data, data_len, pub_key) == 0) {
    return true;
  }
  return false;
}

addr_list_t *addr_list_new() {
  addr_list_t *list = NULL;
  utarray_new(list, &addr_list_icd);
  return list;
}

void addr_list_print(addr_list_t *list) {
  char addr_str[TANGLE_ADDRESS_BASE58_BUF];
  address_t *elm = NULL;

  printf("addresses: [\n");
  ADDR_LIST_FOREACH(list, elm) {
    address_2_base58(elm->addr, addr_str);
    printf("[%" PRIu64 "] %s\n", elm->index, addr_str);
  }
  printf("]\n");
}
