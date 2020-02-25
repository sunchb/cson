
#ifndef _UTIL_H_
#define _UTIL_H_

#include "cson.h"

typedef union {
    char c;
    short s;
    int i;
    long long l;
}integer_val_t;

typedef struct {
    integer_val_t   val;
    int             size;
}integer_t;

int getIntegerValue(cson_t jo_tmp, int size, integer_val_t* i);
int checkIntegerValue(long long val, int size, integer_val_t* i);
long long getIntegerValueFromPointer(void* ptr, int size);

#endif