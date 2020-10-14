#include <string.h>

#include "utils/allocator.h"
#include "utils/byte_buffer.h"

byte_buf_t* byte_buf_new() {
  byte_buf_t* buf = malloc(sizeof(byte_buf_t));
  if (buf) {
    buf->data = NULL;
    buf->len = 0;
  }
  return buf;
}

byte_buf_t* byte_buf_new_with_data(byte_t data[], size_t len) {
  if (data == NULL) {
    return NULL;
  }

  byte_buf_t* buf = malloc(sizeof(byte_buf_t));
  if (buf) {
    buf->data = malloc(len);
    if (buf->data) {
      memcpy(buf->data, data, len);
      buf->len = len;
    } else {
      // Out of Memory
      free(buf);
      return NULL;
    }
  }
  return buf;
}

bool byte_buf_append(byte_buf_t* buf, byte_t data[], size_t len) {
  if (data == NULL || buf == NULL) {
    return false;
  }

  if (buf->data == NULL) {
    buf->data = malloc(len);
    buf->len = 0;
  } else {
    buf->data = realloc(buf->data, buf->len + len);
  }

  if (buf->data == NULL) {
    // Out of memory
    return false;
  }

  memcpy(&(buf->data[buf->len]), data, len);
  buf->len += len;
  return true;
}

void byte_buf_free(byte_buf_t* buf) {
  if (buf) {
    if (buf->data) {
      free(buf->data);
    }
    buf->len = 0;
    free(buf);
  }
}

void byte_buf2str(byte_buf_t* buf) {
  if (buf && buf->data) {
    if (buf->data[buf->len - 1] != '\0') {
      buf->data = realloc(buf->data, buf->len + 1);
      buf->data[buf->len] = '\0';
      buf->len += 1;
    }
  }
}

byte_buf_t* byte_buf_clonen(byte_buf_t* buf, size_t len) {
  byte_buf_t* clone = malloc(sizeof(byte_buf_t));
  if (!clone) {
    return NULL;
  }

  clone->len = len;
  clone->data = malloc(buf->len);
  if (!clone->data) {
    free(clone);
    return NULL;
  }

  memcpy(clone->data, buf->data, len);
  return clone;
}
