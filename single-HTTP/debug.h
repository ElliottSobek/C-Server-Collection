#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#include "lib/colors/colors.h"

#define _DEBUG_INT(x) (printf(CYAN "__Debug__ File: %s, Function: %s, Line: %d; %lli\n" RESET, __FILE__, __func__, __LINE__, (x)))
#define _DEBUG_CHAR(x) (printf(CYAN "__Debug__ File: %s, Function: %s, Line: %d; %c\n" RESET, __FILE__, __func__, __LINE__, (x)))
#define _DEBUG_STRING(x) (printf(CYAN "__Debug__ File: %s, Function: %s, Line: %d; %s\n" RESET, __FILE__, __func__, __LINE__, (x)))
#define _DEBUG_DOUBLE(x) (printf(CYAN "__Debug__ File: %s, Function: %s, Line: %d; %f\n" RESET, __FILE__, __func__, __LINE__, (x)))
#define _DEBUG_POINTER(x) (printf(CYAN "__Debug__ File: %s, Function: %s, Line: %d; %p\n" RESET, __FILE__, __func__, __LINE__, (x)))

#endif /* End DEBUG_H */
