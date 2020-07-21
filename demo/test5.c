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
    _property_array_int32(MultiArrayTest, array, int, NULL, 3),                     //3D array
    _property_array_string(MultiArrayTest, strArray, char*, NULL, 3),               //3D array
    _property_array_object(MultiArrayTest, objArray, data_ref_tbl, Data, NULL, 3),  //3D array
    _property_end()
};

static void checkResult(MultiArrayTest* testObj);

const static char* testStr = "{\"array\":[[[1,2,3,4,5,6],[7,8,9,10,11,12],[13,14,15,16,17,18],[19,20,21,22,23,24],[25,26,27,28,29,30]],[[31,32,33,34,35,36],[37,38,39,40,41,42],[43,44,45,46,47,48],[49,50,51,52,53,54],[55,56,57,58,59,60]],[[61,62,63,64,65,66],[67,68,69,70,71,72],[73,74,75,76,77,78],[79,80,81,82,83,84],[85,86,87,88,89,90]],[[91,92,93,94,95,96],[97,98,99,100,101,102],[103,104,105,106,107,108],[109,110,111,112,113,114],[115,116,117,118,119,120]],[[121,122,123,124,125,126],[127,128,129,130,131,132],[133,134,135,136,137,138],[139,140,141,142,143,144],[145,146,147,148,149,150]]],\"strArray\":[[[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"]],[[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"]],[[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"]],[[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"]],[[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"],[\"str\",\"str\",\"str\",\"str\",\"str\",\"str\"]]],\"objArray\":[[[{\"name\":\"str\",\"num\":1000},{\"name\":\"str\",\"num\":1001},{\"name\":\"str\",\"num\":1002},{\"name\":\"str\",\"num\":1003},{\"name\":\"str\",\"num\":1004},{\"name\":\"str\",\"num\":1005}],[{\"name\":\"str\",\"num\":1006},{\"name\":\"str\",\"num\":1007},{\"name\":\"str\",\"num\":1008},{\"name\":\"str\",\"num\":1009},{\"name\":\"str\",\"num\":1010},{\"name\":\"str\",\"num\":1011}],[{\"name\":\"str\",\"num\":1012},{\"name\":\"str\",\"num\":1013},{\"name\":\"str\",\"num\":1014},{\"name\":\"str\",\"num\":1015},{\"name\":\"str\",\"num\":1016},{\"name\":\"str\",\"num\":1017}],[{\"name\":\"str\",\"num\":1018},{\"name\":\"str\",\"num\":1019},{\"name\":\"str\",\"num\":1020},{\"name\":\"str\",\"num\":1021},{\"name\":\"str\",\"num\":1022},{\"name\":\"str\",\"num\":1023}],[{\"name\":\"str\",\"num\":1024},{\"name\":\"str\",\"num\":1025},{\"name\":\"str\",\"num\":1026},{\"name\":\"str\",\"num\":1027},{\"name\":\"str\",\"num\":1028},{\"name\":\"str\",\"num\":1029}]],[[{\"name\":\"str\",\"num\":1030},{\"name\":\"str\",\"num\":1031},{\"name\":\"str\",\"num\":1032},{\"name\":\"str\",\"num\":1033},{\"name\":\"str\",\"num\":1034},{\"name\":\"str\",\"num\":1035}],[{\"name\":\"str\",\"num\":1036},{\"name\":\"str\",\"num\":1037},{\"name\":\"str\",\"num\":1038},{\"name\":\"str\",\"num\":1039},{\"name\":\"str\",\"num\":1040},{\"name\":\"str\",\"num\":1041}],[{\"name\":\"str\",\"num\":1042},{\"name\":\"str\",\"num\":1043},{\"name\":\"str\",\"num\":1044},{\"name\":\"str\",\"num\":1045},{\"name\":\"str\",\"num\":1046},{\"name\":\"str\",\"num\":1047}],[{\"name\":\"str\",\"num\":1048},{\"name\":\"str\",\"num\":1049},{\"name\":\"str\",\"num\":1050},{\"name\":\"str\",\"num\":1051},{\"name\":\"str\",\"num\":1052},{\"name\":\"str\",\"num\":1053}],[{\"name\":\"str\",\"num\":1054},{\"name\":\"str\",\"num\":1055},{\"name\":\"str\",\"num\":1056},{\"name\":\"str\",\"num\":1057},{\"name\":\"str\",\"num\":1058},{\"name\":\"str\",\"num\":1059}]],[[{\"name\":\"str\",\"num\":1060},{\"name\":\"str\",\"num\":1061},{\"name\":\"str\",\"num\":1062},{\"name\":\"str\",\"num\":1063},{\"name\":\"str\",\"num\":1064},{\"name\":\"str\",\"num\":1065}],[{\"name\":\"str\",\"num\":1066},{\"name\":\"str\",\"num\":1067},{\"name\":\"str\",\"num\":1068},{\"name\":\"str\",\"num\":1069},{\"name\":\"str\",\"num\":1070},{\"name\":\"str\",\"num\":1071}],[{\"name\":\"str\",\"num\":1072},{\"name\":\"str\",\"num\":1073},{\"name\":\"str\",\"num\":1074},{\"name\":\"str\",\"num\":1075},{\"name\":\"str\",\"num\":1076},{\"name\":\"str\",\"num\":1077}],[{\"name\":\"str\",\"num\":1078},{\"name\":\"str\",\"num\":1079},{\"name\":\"str\",\"num\":1080},{\"name\":\"str\",\"num\":1081},{\"name\":\"str\",\"num\":1082},{\"name\":\"str\",\"num\":1083}],[{\"name\":\"str\",\"num\":1084},{\"name\":\"str\",\"num\":1085},{\"name\":\"str\",\"num\":1086},{\"name\":\"str\",\"num\":1087},{\"name\":\"str\",\"num\":1088},{\"name\":\"str\",\"num\":1089}]],[[{\"name\":\"str\",\"num\":1090},{\"name\":\"str\",\"num\":1091},{\"name\":\"str\",\"num\":1092},{\"name\":\"str\",\"num\":1093},{\"name\":\"str\",\"num\":1094},{\"name\":\"str\",\"num\":1095}],[{\"name\":\"str\",\"num\":1096},{\"name\":\"str\",\"num\":1097},{\"name\":\"str\",\"num\":1098},{\"name\":\"str\",\"num\":1099},{\"name\":\"str\",\"num\":1100},{\"name\":\"str\",\"num\":1101}],[{\"name\":\"str\",\"num\":1102},{\"name\":\"str\",\"num\":1103},{\"name\":\"str\",\"num\":1104},{\"name\":\"str\",\"num\":1105},{\"name\":\"str\",\"num\":1106},{\"name\":\"str\",\"num\":1107}],[{\"name\":\"str\",\"num\":1108},{\"name\":\"str\",\"num\":1109},{\"name\":\"str\",\"num\":1110},{\"name\":\"str\",\"num\":1111},{\"name\":\"str\",\"num\":1112},{\"name\":\"str\",\"num\":1113}],[{\"name\":\"str\",\"num\":1114},{\"name\":\"str\",\"num\":1115},{\"name\":\"str\",\"num\":1116},{\"name\":\"str\",\"num\":1117},{\"name\":\"str\",\"num\":1118},{\"name\":\"str\",\"num\":1119}]],[[{\"name\":\"str\",\"num\":1120},{\"name\":\"str\",\"num\":1121},{\"name\":\"str\",\"num\":1122},{\"name\":\"str\",\"num\":1123},{\"name\":\"str\",\"num\":1124},{\"name\":\"str\",\"num\":1125}],[{\"name\":\"str\",\"num\":1126},{\"name\":\"str\",\"num\":1127},{\"name\":\"str\",\"num\":1128},{\"name\":\"str\",\"num\":1129},{\"name\":\"str\",\"num\":1130},{\"name\":\"str\",\"num\":1131}],[{\"name\":\"str\",\"num\":1132},{\"name\":\"str\",\"num\":1133},{\"name\":\"str\",\"num\":1134},{\"name\":\"str\",\"num\":1135},{\"name\":\"str\",\"num\":1136},{\"name\":\"str\",\"num\":1137}],[{\"name\":\"str\",\"num\":1138},{\"name\":\"str\",\"num\":1139},{\"name\":\"str\",\"num\":1140},{\"name\":\"str\",\"num\":1141},{\"name\":\"str\",\"num\":1142},{\"name\":\"str\",\"num\":1143}],[{\"name\":\"str\",\"num\":1144},{\"name\":\"str\",\"num\":1145},{\"name\":\"str\",\"num\":1146},{\"name\":\"str\",\"num\":1147},{\"name\":\"str\",\"num\":1148},{\"name\":\"str\",\"num\":1149}]]]}";

void test5()
{
    printf("=========================================\n");
    printf("\t\tRunning %s\n", __FUNCTION__);
    printf("=========================================\n");
    MultiArrayTest testObj;

    int ret = csonJsonStr2Struct(testStr, &testObj, multi_array_ref_tbl);
    CHECK_NUMBER(ret, 0);
    checkResult(&testObj);
    csonFreePointer(&testObj, multi_array_ref_tbl);
    printf("Successed %s.\n", __FUNCTION__);
}


void checkResult(MultiArrayTest* testObj)
{
    CHECK_NUMBER(array_size(testObj->array), 5);
    CHECK_NUMBER(array_size(testObj->strArray), 5);
    CHECK_NUMBER(array_size(testObj->objArray), 5);
    CHECK_NUMBER(array_dimen(testObj->array), 3);
    CHECK_NUMBER(array_dimen(testObj->strArray), 3);
    CHECK_NUMBER(array_dimen(testObj->objArray), 3);

    int temp = 1;
    for (int i = 0; i < array_size(testObj->array); i++) {
        CHECK_NUMBER(array_size(testObj->array[i]), 5);
        CHECK_NUMBER(array_dimen(testObj->array[i]), 2);
        for (int j = 0; j < array_size(testObj->array[i]); j++) {
            CHECK_NUMBER(array_size(testObj->array[i][j]), 6);
            CHECK_NUMBER(array_dimen(testObj->array[i][j]), 1);
            for (int k = 0; k < array_size(testObj->array[i][j]); k++) {
                CHECK_NUMBER(testObj->array[i][j][k], temp++);
            }
        }
    }

    for (int i = 0; i < array_size(testObj->strArray); i++) {
        CHECK_NUMBER(array_size(testObj->strArray[i]), 5);
        CHECK_NUMBER(array_dimen(testObj->strArray[i]), 2);
        for (int j = 0; j < array_size(testObj->strArray[i]); j++) {
            CHECK_NUMBER(array_size(testObj->strArray[i][j]), 6);
            CHECK_NUMBER(array_dimen(testObj->strArray[i][j]), 1);
            for (int k = 0; k < array_size(testObj->strArray[i][j]); k++) {
                CHECK_STRING(testObj->strArray[i][j][k], "str");
            }
        }
    }

    temp = 1000;
    for (int i = 0; i < array_size(testObj->objArray); i++) {
        CHECK_NUMBER(array_size(testObj->objArray[i]), 5);
        CHECK_NUMBER(array_dimen(testObj->objArray[i]), 2);
        for (int j = 0; j < array_size(testObj->objArray[i]); j++) {
            CHECK_NUMBER(array_size(testObj->objArray[i][j]), 6);
            CHECK_NUMBER(array_dimen(testObj->objArray[i][j]), 1);
            for (int k = 0; k < array_size(testObj->objArray[i][j]); k++) {
                CHECK_STRING(testObj->objArray[i][j][k].name, "str");
                CHECK_NUMBER(testObj->objArray[i][j][k].num, temp++);
            }
        }
    }
}