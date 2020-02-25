/**
 * @file   cson_packer.c
 * @author sun_chb@126.com
 */
#include "cson.h"
#include "cson_util.h"
#include "stdio.h"
#include "stdlib.h"

typedef int (*json_pack_proc)(void* input, const reflect_item_t* tbl, int index, cson_t* obj);

static int getJsonObject(void* input, const reflect_item_t* tbl, int index, cson_t* obj);
static int getJsonArray(void* input, const reflect_item_t* tbl, int index, cson_t* obj);
static int getJsonString(void* input, const reflect_item_t* tbl, int index, cson_t* obj);
static int getJsonInteger(void* input, const reflect_item_t* tbl, int index, cson_t* obj);
static int getJsonReal(void* input, const reflect_item_t* tbl, int index, cson_t* obj);
static int getJsonBool(void* input, const reflect_item_t* tbl, int index, cson_t* obj);

json_pack_proc jsonPackTbl[] = {
    getJsonObject,
    getJsonArray,
    getJsonString,
    getJsonInteger,
    getJsonReal,
    getJsonBool,
    getJsonBool,
    NULL
};

static int csonStruct2JsonObj(cson_t obj, void* input, const reflect_item_t* tbl);

int csonStruct2JsonStr(char** jstr, void* input, const reflect_item_t* tbl)
{
    cson_t jsonPack = cson_object();

    if (!jsonPack) return ERR_MEMORY;

    int ret = csonStruct2JsonObj(jsonPack, input, tbl);

    if (ret == ERR_NONE) {
        char* dumpStr = cson_to_string(jsonPack);
        if (dumpStr == NULL) {
            ret = ERR_MEMORY;
        } else {
            *jstr = dumpStr;
        }
    }

    cson_decref(jsonPack);
    return ret;
}

int csonStruct2JsonObj(cson_t obj, void* input, const reflect_item_t* tbl)
{
    int i = 0;
    int ret = ERR_NONE;

    if (!obj || !input || !tbl) return ERR_ARGS;

    while (1) {
        if (tbl[i].field == NULL) break;

        cson_t joTmp = NULL;
        int jsonType = tbl[i].type;

        if (jsonPackTbl[jsonType] != NULL) {
            ret = jsonPackTbl[jsonType](input, tbl, i, &joTmp);
        }

        if (ret != ERR_NONE ) {
            printf("!!!!pack error on field:%s, cod=%d!!!!\n", tbl[i].field, ret);
            if (!tbl[i].nullable) return ret;
        } else {
            cson_object_set_new(obj, tbl[i].field, joTmp);
        }

        i++;
    }

    return ERR_NONE;
}

int getJsonInteger(void* input, const reflect_item_t* tbl, int index, cson_t* obj)
{
    if(tbl[index].size != sizeof(char) &&
        tbl[index].size != sizeof(short) &&
        tbl[index].size != sizeof(int) &&
        tbl[index].size != sizeof(long long)){
        printf("Unsupported size(=%ld) of integer.\n", tbl[index].size);
        printf("Please check if the type of field %s in char/short/int/long long!\n", tbl[index].field);
        return ERR_OVERFLOW;
    }
    
    void* pSrc = (void*)((char*)input + tbl[index].offset);

    long long val = getIntegerValueFromPointer(pSrc, tbl[index].size);

    *obj = cson_integer(val);

    return ERR_NONE;
}

int getJsonString(void* input, const reflect_item_t* tbl, int index, cson_t* obj)
{
    void* pSrc = (void*)((char*)input + tbl[index].offset);

    if (*((char**)pSrc) == NULL) return ERR_MISSING_FIELD;

    *obj = cson_string(*((char**)pSrc));
    return ERR_NONE;
}

int getJsonObject(void* input, const reflect_item_t* tbl, int index, cson_t* obj)
{
    void* pSrc = (void*)((char*)input + tbl[index].offset);
    cson_t jotmp = cson_object();
    int ret = csonStruct2JsonObj(jotmp, pSrc, tbl[index].reflect_tbl);

    if (ret == ERR_NONE) {
        *obj = jotmp;
    } else {
        cson_decref(jotmp);
    }

    return ret;
}

int getJsonArray(void* input, const reflect_item_t* tbl, int index, cson_t* obj)
{
    int ret = ERR_NONE;
    int countIndex = -1;
    char* pSrc = (*(char**)((char*)input + tbl[index].offset));

    if (pSrc == NULL) return ERR_MISSING_FIELD;

    void* ptr = csonGetProperty(input, tbl[index].arrayCountField, tbl, &countIndex);
    
    if(ptr == NULL || countIndex == -1){
        return ERR_MISSING_FIELD;
    }
    long long size = getIntegerValueFromPointer(ptr, tbl[countIndex].size);

    cson_t joArray = cson_array();

    long long successCount = 0;

    for (long long i = 0; i < size; i++) {
        cson_t jotmp;
        
        if(tbl[index].reflect_tbl[0].field[0] == '0'){      /* field start with '0' mean basic types. */
            ret = jsonPackTbl[tbl[index].reflect_tbl[0].type](pSrc + (i * tbl[index].arraySize), tbl[index].reflect_tbl, 0, &jotmp);
        }else{        
            jotmp = cson_object();    
            ret = csonStruct2JsonObj(jotmp, pSrc + (i * tbl[index].arraySize), tbl[index].reflect_tbl);
        }

        if (ret == ERR_NONE) {
            successCount++;
            cson_array_add(joArray, jotmp);
        } else {
            printf("create array item faild.\n");
            cson_decref(jotmp);
        }
    }

    if (successCount == 0) {
        cson_decref(joArray);
        return ERR_MISSING_FIELD;
    } else {
        *obj = joArray;
        return ERR_NONE;
    }

    return ret;
}

int getJsonReal(void* input, const reflect_item_t* tbl, int index, cson_t* obj)
{
    if(tbl[index].size != sizeof(double)){
        printf("Unsupported size(=%ld) of real.\n", tbl[index].size);
        printf("Please check if the type of field %s is double!\n", tbl[index].field);
        return ERR_OVERFLOW;
    }

    void* pSrc = (void*)((char*)input + tbl[index].offset);
    *obj = cson_real(*((double*)pSrc));
    return ERR_NONE;
}

int getJsonBool(void* input, const reflect_item_t* tbl, int index, cson_t* obj)
{
    if(tbl[index].size != sizeof(char) &&
        tbl[index].size != sizeof(short) &&
        tbl[index].size != sizeof(int) &&
        tbl[index].size != sizeof(long long)){
        printf("Unsupported size(=%ld) of bool.\n", tbl[index].size);
        printf("Please check if the type of field %s in char/short/int/long long!\n", tbl[index].field);
        return ERR_OVERFLOW;
    }
    
    void* pSrc = (void*)((char*)input + tbl[index].offset);

    

    if(tbl[index].size == sizeof(char)){
        *obj = cson_bool(*((char*)pSrc));
    }else if(tbl[index].size == sizeof(short)){
        *obj = cson_bool(*((short*)pSrc));
    }else if(tbl[index].size == sizeof(int)){
        *obj = cson_bool(*((int*)pSrc));
    }else{
        *obj = cson_bool(*((long long*)pSrc));
    }

    return ERR_NONE;
}