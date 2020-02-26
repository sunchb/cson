#include "cson_util.h"
#include "cson_interface.h"
#include "limits.h"
#include "stdio.h"

long long getIntegerValueFromPointer(void* ptr, int size){
    long long ret = 0;

    if(!ptr) return 0;

    if(size == sizeof(char)){
        ret = *((char*)ptr);
    }else if(size == sizeof(short)){
        ret = *((short*)ptr);
    }else if(size == sizeof(int)){
        ret = *((int*)ptr);
    }else if(size == sizeof(long long)){
        ret = *((long long*)ptr);
    }else{
        printf("Unsupported size(=%d) of integer.\n", size);
    }

    return ret;
}