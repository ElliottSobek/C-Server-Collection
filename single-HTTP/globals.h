#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

#define BYTE_S 1L
#define KBYTE_S 1024L
#define MBYTE_S ((uint64_t) (KBYTE_S * KBYTE_S))

bool verbose_flag;

#endif /* End GLOBALS_H */
