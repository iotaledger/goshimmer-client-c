#ifndef __UTILS_ALLOCATOR_H__
#define __UTILS_ALLOCATOR_H__

/**
 * @brief Memory Allocator abstract layer
 *
 */

#ifdef USE_JEMALLOC
#warning "Use jemalloc allocator"
#include "jemalloc/jemalloc.h"
#else
#include <stdlib.h>
#endif

#endif
