#include "cson_util.h"
#include "cson_interface.h"
#include "limits.h"
#include "stdio.h"

int getIntegerValue(cson_t jo_tmp, int size, integer_val_t* i){
    long long temp;
    
    if(size != sizeof(char) &&
        size != sizeof(short) &&
        size != sizeof(int) &&
        size != sizeof(long long)){
        return ERR_OVERFLOW;
    }

    if(cson_typeof(jo_tmp) == CSON_INTEGER){
        temp = cson_integer_value(jo_tmp);
    }else if(cson_typeof(jo_tmp) == CSON_TRUE){
        temp = 1;
    }else if(cson_typeof(jo_tmp) == CSON_FALSE){
        temp = 0;
    }else if(cson_typeof(jo_tmp) == CSON_REAL){
        double tempDouble = cson_real_value(jo_tmp);
        if(tempDouble > LLONG_MAX || tempDouble < LLONG_MIN){
            return ERR_OVERFLOW;
        }else{
            temp = tempDouble;
        }
    }else{
        return ERR_ARGS;
    }

    if(size == sizeof(char) && (temp > CHAR_MAX || temp < CHAR_MIN)){
        return ERR_OVERFLOW;
    }else if(size == sizeof(short) && (temp > SHRT_MAX || temp < SHRT_MIN)){
        return ERR_OVERFLOW;
    }else if(size == sizeof(int)  && (temp > INT_MAX || temp < INT_MIN)){
        return ERR_OVERFLOW;
    }else{
    }
    
    /* avoid error on big endian */
    if(size == sizeof(char)){
        i->c = temp;
    }else if(size == sizeof(short)){

        i->s = temp;
    }else if(size == sizeof(int)){
        i->i = temp;
    }else{
        i->l = temp;
    }
    
    return ERR_NONE;
}

int checkIntegerValue(long long val, int size, integer_val_t* i){
    if(size != sizeof(char) &&
        size != sizeof(short) &&
        size != sizeof(int) &&
        size != sizeof(long long)){
        return ERR_OVERFLOW;
    }

    if(size == sizeof(char) && (val > CHAR_MAX || val < CHAR_MIN)){
        return ERR_OVERFLOW;
    }else if(size == sizeof(short) && (val > SHRT_MAX || val < SHRT_MIN)){
        return ERR_OVERFLOW;
    }else if(size == sizeof(int)  && (val > INT_MAX || val < INT_MIN)){
        return ERR_OVERFLOW;
    }else{
    }
    
    /* avoid error on big endian */
    if(size == sizeof(char)){
        i->c = val;
    }else if(size == sizeof(short)){

        i->s = val;
    }else if(size == sizeof(int)){
        i->i = val;
    }else{
        i->l = val;
    }

    return ERR_NONE;
}

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