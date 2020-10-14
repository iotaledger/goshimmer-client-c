#ifndef __CORE_TYPES_H__
#define __CORE_TYPES_H__

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t byte_t;

static inline bool is_little_endian(){return (1 == *(unsigned char *)&(const int){1});}

#endif