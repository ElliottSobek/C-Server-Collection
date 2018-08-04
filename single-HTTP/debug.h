#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define _DEBUG_STRING(x) (printf("__Debug__ File: %s, Function: %s, Line: %d; %s\n", __FILE__, __func__, __LINE__, (x)))
#define _DEBUG_INT(x) (printf("__Debug__ File: %s, Function: %s, Line: %d; %lli\n", __FILE__, __func__, __LINE__, (x)))
#define _DEBUG_CHAR(x) (printf("__Debug__ File: %s, Function: %s, Line: %d; %c\n", __FILE__, __func__, __LINE__, (x)))

#endif /* End DEBUG_H */
