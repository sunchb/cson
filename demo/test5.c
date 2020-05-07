#include "cson.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


#include "assert.h"
#include "math.h"

#define CHECK_STRING(a, b)  assert(strcmp(a, b) == 0)
#define CHECK_NUMBER(a, b)  assert(a == b)
#define CHECK_REAL(a, b)    assert(fabs(a-b) <= 1e-6)

typedef struct {
    char*           name;
    int             num;
} Data;

typedef struct {
    int***          array;
    char****        strArray;
    Data***         objArray;
} MultiArrayTest;

reflect_item_t  data_ref_tbl[] = {
    _property_string(Data, name),
    _property_int(Data, num),
    _property_end()
};

reflect_item_t multi_array_ref_tbl[] = {
    _property_array_int(MultiArrayTest, array, int, NULL, 3),
    _property_array_string(MultiArrayTest, strArray, char*, NULL, 3),
    _property_array_object(MultiArrayTest, objArray, data_ref_tbl, Data, NULL, 3),
    _property_end()
};

void* freePointerOfArray(void* pData, const reflect_item_t* tbl)
{
    if(tbl->type == CSON_ARRAY){
        csonArrayFree(*(void**)pData);
        *(void**)pData = NULL;
    }

    return NULL;
}

void test5(){
    cson_array_size_t dimenSize[3] = {10, 10, 10};

    MultiArrayTest testObj;

    testObj.array = (int***)csonAllocMultiDimenArray(3, dimenSize, sizeof(int));
    testObj.strArray = (char****)csonAllocMultiDimenArray(3, dimenSize, sizeof(char*));
    testObj.objArray = (Data***)csonAllocMultiDimenArray(3, dimenSize, sizeof(Data));
    char* testStr = "str";

    int num = 1;
    for(int i = 0; i < dimenSize[0]; i++){
        for(int j = 0; j < dimenSize[1]; j++){
            for(int k = 0; k < dimenSize[2]; k++){
                testObj.array[i][j][k] = num++;
                testObj.strArray[i][j][k] = testStr;
                testObj.objArray[i][j][k].num = num++;
                testObj.objArray[i][j][k].name = testStr;
            }
        }
    }

    num = 1;
    for(int i = 0; i < dimenSize[0]; i++){
        for(int j = 0; j < dimenSize[1]; j++){
            for(int k = 0; k < dimenSize[2]; k++){
                CHECK_NUMBER(testObj.array[i][j][k], num++);
                CHECK_NUMBER(testObj.objArray[i][j][k].num, num++);
            }
        }
    }
    
    char* jstrOutput;
    int ret = csonStruct2JsonStr(&jstrOutput, &testObj, multi_array_ref_tbl);
    printf("encode ret=%d\nJson:%s\n", ret, jstrOutput);

    free(jstrOutput);
    csonLoopProperty(&testObj, multi_array_ref_tbl, freePointerOfArray);
}

void test6(){
    cson_array_size_t dimenSize[4] = {10, 10, 10, 100};
    int**** pArray3D = (int****)csonAllocMultiDimenArray(4, dimenSize, sizeof(int));

    int num = 1;
    for(int i = 0; i < dimenSize[0]; i++){
        for(int j = 0; j < dimenSize[1]; j++){
            for(int k = 0; k < dimenSize[2]; k++){
                for(int m = 0; m < dimenSize[3]; m++){
                    pArray3D[i][j][k][m] = num++;
                }
            }
        }
    }

    num = 1;
    for(int i = 0; i < dimenSize[0]; i++){
        for(int j = 0; j < dimenSize[1]; j++){
            for(int k = 0; k < dimenSize[2]; k++){
                for(int m = 0; m < dimenSize[3]; m++){
                    CHECK_NUMBER(pArray3D[i][j][k][m], num++);
                }
            }
        }
    }

    csonFreeMultiDimenArray(pArray3D);
}