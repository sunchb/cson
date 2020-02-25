/**
 * @file   cson_parser.h
 * @author sun_chb@126.com
 */
#include "cson.h"
#include "cson_util.h"
#include "string.h"
#include "limits.h"
#include "stdio.h"
#include "stdlib.h"

typedef int (*json_obj_proc)(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);

static int parseJsonObject(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);
static int parseJsonArray(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);
static int parseJsonString(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);
static int parseJsonInteger(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);
static int parseJsonReal(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);
static int parseJsonBool(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);

json_obj_proc jsonObjProcTbl[] = {
    parseJsonObject,
    parseJsonArray,
    parseJsonString,
    parseJsonInteger,
    parseJsonReal,
    parseJsonBool,
    parseJsonBool,
    NULL
};

static int parseJsonObjectDefault(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);
static int parseJsonArrayDefault(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);
static int parseJsonStringDefault(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);
static int parseJsonIntegerDefault(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);
static int parseJsonRealDefault(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index);

json_obj_proc jsonObjDefaultTbl[] = {
    parseJsonObjectDefault,
    parseJsonArrayDefault,
    parseJsonStringDefault,
    parseJsonIntegerDefault,
    parseJsonRealDefault,
    parseJsonIntegerDefault,
    parseJsonIntegerDefault,
    NULL
};

static int csonJsonObj2Struct(cson_t jo, void* output, const reflect_item_t* tbl);

int csonJsonStr2Struct(const char* jstr, void* output, const reflect_item_t* tbl)
{
    /* load json string */
    cson_t jo = cson_loadb(jstr, strlen(jstr));

    if (!jo) return ERR_FORMAT;

    int ret = csonJsonObj2Struct(jo, output, tbl);
    cson_decref(jo);

    return ret;
}

int csonJsonObj2Struct(cson_t jo, void* output, const reflect_item_t* tbl)
{
    if (!jo || !output || !tbl) return ERR_ARGS;

    for (int i = 0;; i++) {
        int ret = ERR_NONE;
        if (tbl[i].field == NULL) break;

        cson_t jo_tmp = cson_object_get(jo, tbl[i].field);

        if (jo_tmp == NULL) {
            ret = ERR_MISSING_FIELD;
        } else {

            int jsonType = cson_typeof(jo_tmp);

            if (jsonType == tbl[i].type ||
                (cson_is_number(cson_typeof(jo_tmp)) && cson_is_number(tbl[i].type)) ||
                (cson_is_bool(cson_typeof(jo_tmp)) && cson_is_bool(tbl[i].type))) {
                if (jsonObjProcTbl[tbl[i].type] != NULL) {
                    ret = jsonObjProcTbl[tbl[i].type](jo_tmp, output, tbl, i);
                }
            } else {
                ret = ERR_TYPE;
            }
        }

        if (ret != ERR_NONE ) {
            printf("!!!!parse error on field:%s, cod=%d!!!!\n", tbl[i].field, ret);
            jsonObjDefaultTbl[tbl[i].type](NULL, output, tbl, i);
            if (!tbl[i].nullable) return ret;
        }
    }

    return ERR_NONE;
}

int parseJsonString(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index)
{
    const char* tempstr = cson_string_value(jo_tmp);
    if (NULL != tempstr) {
        char* pDst = malloc(strlen(tempstr) + 1);
        if (pDst == NULL) {
            return ERR_MEMORY;
        }
        strcpy(pDst, tempstr);
        csonSetPropertyFast(output, &pDst, tbl + index);

        return ERR_NONE;
    }

    return ERR_MISSING_FIELD;
}


int parseJsonInteger(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index)
{
    int ret;
    integer_val_t value;
    ret = getIntegerValue(jo_tmp, tbl[index].size, &value);

    if(ret != ERR_NONE){
        printf("Get integer failed!field:%s,errno:%d.\n", tbl[index].field, ret);
    }else{
        csonSetPropertyFast(output, &value, tbl + index);
    }

    return ret;
}


int parseJsonObject(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index)
{
    return csonJsonObj2Struct(jo_tmp, (char*)output + tbl[index].offset, tbl[index].reflect_tbl);
}

int parseJsonArray(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index)
{
    printf("parseJsonArray:%s\n", tbl[index].field);
    size_t arraySize = cson_array_size(jo_tmp);

    if (arraySize == 0) {
        csonSetProperty(output, tbl[index].arrayCountField, &arraySize, tbl);
        return ERR_NONE;
    }
    
    int countIndex = -1;
    csonGetProperty(output, tbl[index].arrayCountField, tbl, &countIndex);

    if(countIndex == -1){
        return ERR_MISSING_FIELD;
    }

    char* pMem = (char*)malloc(arraySize * tbl[index].arraySize);
    if (pMem == NULL) return ERR_MEMORY;

    long long successCount = 0;
    for (size_t j = 0; j < arraySize; j++) {
        cson_t item = cson_array_get(jo_tmp, j);
        if (item != NULL) {
            int ret;

            if(tbl[index].reflect_tbl[0].field[0] == '0'){      /* field start with '0' mean basic types. */
                ret = jsonObjProcTbl[tbl[index].reflect_tbl[0].type](item, pMem + (successCount * tbl[index].arraySize), tbl[index].reflect_tbl, 0);
            }else{
                ret = csonJsonObj2Struct(item, pMem + (successCount * tbl[index].arraySize), tbl[index].reflect_tbl);
            }

            if (ret == ERR_NONE) {
                successCount++;
            }
        }
    }

    integer_val_t val;
    if(checkIntegerValue(successCount, tbl[countIndex].size, &val) != ERR_NONE){
        successCount = 0;
    }        

    if (successCount == 0) {
        csonSetPropertyFast(output, &successCount, tbl + countIndex);
        free(pMem);
        pMem = NULL;
        csonSetPropertyFast(output, &pMem, tbl + index);
        return ERR_MISSING_FIELD;
    } else {
        csonSetPropertyFast(output, &val, tbl + countIndex);
        csonSetPropertyFast(output, &pMem, tbl + index);
        return ERR_NONE;
    }
}

int parseJsonReal(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index)
{
    if(tbl[index].size != sizeof(double)){
        printf("Unsupported size(=%ld) of real.\n", tbl[index].size);
        printf("Please check if the type of field %s is double!\n", tbl[index].field);
        return ERR_OVERFLOW;
    }

    double temp;
    if(cson_typeof(jo_tmp) == CSON_REAL){
        temp = cson_real_value(jo_tmp);
    }else{
        temp = cson_integer_value(jo_tmp);
    }

    csonSetPropertyFast(output, &temp, tbl + index);
    return ERR_NONE;
}

int parseJsonBool(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index)
{
    int ret;
    integer_val_t value;
    ret = getIntegerValue(jo_tmp, tbl[index].size, &value);
    if(ret != ERR_NONE){
        printf("Get integer failed!field:%s,errno:%d.\n", tbl[index].field, ret);
    }else{
        csonSetPropertyFast(output, &value, tbl + index);
    }
    return ret;
}

int parseJsonObjectDefault(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index){
    int i = 0;
    while(1){
        if (tbl[index].reflect_tbl[i].field == NULL) break;
        jsonObjDefaultTbl[tbl[index].reflect_tbl[i].type](NULL, output, tbl[index].reflect_tbl, i);
        i++;
    };
    return ERR_NONE;
}

int parseJsonArrayDefault(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index){
    void* temp = NULL;
    csonSetPropertyFast(output, &temp, tbl + index);
    return ERR_NONE;
}

int parseJsonStringDefault(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index){
    char* temp = NULL;
    csonSetPropertyFast(output, &temp, tbl + index);
    return ERR_NONE;
}

int parseJsonIntegerDefault(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index){
    long long temp = 0;
    
    if(tbl[index].size != sizeof(char) &&
        tbl[index].size != sizeof(short) &&
        tbl[index].size != sizeof(int) &&
        tbl[index].size != sizeof(long long)){
        printf("Unsupported size(=%ld) of bool.\n", tbl[index].size);
        printf("Please check if the type of field %s in char/short/int/long long!\n", tbl[index].field);
        return ERR_OVERFLOW;
    }

    union{
        char c;
        short s;
        int i;
        long long l;
    }ret;
	
    /* avoid error on big endian */
    if(tbl[index].size == sizeof(char)){
        ret.c = temp;
    }else if(tbl[index].size == sizeof(short)){
        ret.s = temp;
    }else if(tbl[index].size == sizeof(int)){
        ret.i = temp;
    }else{
        ret.l = temp;
    }

    csonSetPropertyFast(output, &ret, tbl + index);
    return ERR_NONE;
}

int parseJsonRealDefault(cson_t jo_tmp, void* output, const reflect_item_t* tbl, int index){
    if(tbl[index].size != sizeof(double)){
        printf("Unsupported size(=%ld) of bool.\n", tbl[index].size);
        printf("Please check if the type of field %s is double!\n", tbl[index].field);
        return ERR_OVERFLOW;
    }

    double temp = 0.0;
    csonSetPropertyFast(output, &temp, tbl + index);
    return ERR_NONE;
}
