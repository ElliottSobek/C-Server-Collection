#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>
#include <linux/limits.h>

#define BYTE_S 1L
#define KBYTE_S 1024L
#define MBYTE_S ((uint64_t) (KBYTE_S * KBYTE_S))

#define NT_LEN 1

bool _verbose_flag, _caching_flag;
char _log_root[PATH_MAX + NT_LEN], _db_path[PATH_MAX + NT_LEN];

#endif /* End GLOBALS_H */
