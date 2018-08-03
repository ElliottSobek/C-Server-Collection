#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <linux/limits.h>

#define BYTE_S 1L
#define KBYTE_S 1024L
#define MBYTE_S ((uint64_t) (KBYTE_S * KBYTE_S))

#define NT_LEN 1

#define _debug_string(x) (printf("__Debug__ File: %s, Function: %s, Line: %d; %s\n", __FILE__, __func__, __LINE__, (x)))
#define _debug_int(x) (printf("__Debug__ File: %s, Function: %s, Line: %d; %lli\n", __FILE__, __func__, __LINE__, (x)))
#define _debug_char(x) (printf("__Debug__ File: %s, Function: %s, Line: %d; %c\n", __FILE__, __func__, __LINE__, (x)))

bool verbose_flag;
char _log_root[PATH_MAX];

#endif /* End GLOBALS_H */
