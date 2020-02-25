/**
 * @file   cson_reflect.c
 * @author sun_chb@126.com
 */
#include "cson_reflect.h"
#include "cson_util.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

const reflect_item_t integerReflectTbl[] = {
    {"0Integer", 0, sizeof(int), CSON_INTEGER, NULL, 0, NULL, 1},
    {}
};

const reflect_item_t stringReflectTbl[] = {
    {"0String", 0, sizeof(char*), CSON_STRING, NULL, 0, NULL, 1},
    {}
};

const reflect_item_t realReflectTbl[] = {
    {"0Real", 0, sizeof(double), CSON_REAL, NULL, 0, NULL, 1},
    {}
};

const reflect_item_t boolReflectTbl[] = {
    {"0Bool", 0, sizeof(int), CSON_TRUE, NULL, 0, NULL, 1},
    {}
};

static const reflect_item_t* getReflexItem(const char* field, const reflect_item_t* tbl, int* pIndex)
{
    const reflect_item_t* ret = NULL;

    for (int i = 0;; i++) {
        if (!(tbl[i].field)) break;
        if (strcmp(field, tbl[i].field) == 0) {
            ret = &(tbl[i]);

            if(pIndex) *pIndex = i;
            break;
        }
    }

    if (!ret) printf("Can not find field:%s.", field);

    return ret;
}

void*   csonGetProperty(void* obj, const char* field, const reflect_item_t* tbl, int* pIndex)
{
    if (!(obj && field && tbl)) return NULL;
    const reflect_item_t* ret = getReflexItem(field, tbl, pIndex);

    if (!ret) return NULL;

    return (void*)((char*)obj + ret->offset);
}

void    csonSetProperty(void* obj, const char* field, void* data, const reflect_item_t* tbl)
{
    if (!(obj && field && data && tbl)) return;

    const reflect_item_t* ret = getReflexItem(field, tbl, NULL);

    if (!ret) return;

    void* pDst = (void*)((char*)obj + ret->offset);
    memcpy(pDst, data, ret->size);
    return;
}

void    csonSetPropertyFast(void* obj, const void* data, const reflect_item_t* tbl)
{
    if (!(obj && data && tbl)) return;

    void* pDst = (void*)((char*)obj + tbl->offset);
    memcpy(pDst, data, tbl->size);
    return;
}

void csonLoopProperty(void* pData, const reflect_item_t* tbl, loop_func_t func)
{
    int i = 0;
    while (1) {
        if (!tbl[i].field) break;

        char* pProperty = (char*)pData + tbl[i].offset;
        if (tbl[i].type == CSON_ARRAY) {
            int countIndex = -1;
            void* ptr = csonGetProperty(pData, tbl[i].arrayCountField, tbl, &countIndex);
            
            if(ptr == NULL || countIndex == -1){
                continue;
            }
            long long size = getIntegerValueFromPointer(ptr, tbl[countIndex].size);

            for (int j = 0; j < size; j++) {
                csonLoopProperty(*((char**)pProperty) + j * tbl[i].arraySize, tbl[i].reflect_tbl, func);
            }
        } else if (tbl[i].type == CSON_OBJECT) {
            csonLoopProperty(pProperty, tbl[i].reflect_tbl, func);
        }

        func(pProperty, tbl + i);

        i++;
    }
}