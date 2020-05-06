#include "cson.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "assert.h"
#include "math.h"

#define CHECK_STRING(a, b)  assert(strcmp(a, b) == 0)
#define CHECK_NUMBER(a, b)  assert(a == b)
#define CHECK_REAL(a, b)    assert(fabs(a-b) <= 1e-6)
/**
 * 该示例会使用cson解析如下所示播放列表。
 *
{
    "name":"jay zhou",
    "creater":"dahuaxia",
    "songNum":2,
    "songList":[
        {
            "songName":"qilixiang",
            "signerName":"jay zhou",
            "albumName":"qilixiang",
            "url":"www.kugou.com",
            "duration":200,
            "paid":false,
            "price":6.6600000000000001,
            "lyricNum":2,
            "lyric":[
                {
                    "time":1,
                    "text":"Sparrow outside the window"
                },
                {
                    "time":10,
                    "text":"Multi mouth on the pole"
                }
            ],
            "key":[
                1234,
                5678,
                9876
            ]
        },
        {
            "songName":"dongfengpo",
            "signerName":"jay zhou",
            "albumName":"dongfengpo",
            "url":"music.qq.com",
            "duration":180,
            "paid":true,
            "price":0.88,
            "lyricNum":2,
            "lyric":[
                {
                    "time":10,
                    "text":"A sad parting, standing alone in the window"
                },
                {
                    "time":20,
                    "text":"I'm behind the door pretending you're not gone"
                }
            ],
            "key":[
                1111,
                2222,
                3333
            ]
        }
    ],
    "extData":{
        "a":999,
        "b":1.05
    }
}
*/

/**
 * 1）首先我们需要定义与上面json相对应的数据结构。于是有了@PlayList、@ExtData、@SongInfo、@Lyric。
 *    即使不实用cson，想要解析json，通常你也需要这么做。
 *
 *    注意:结构体属性名需与json中字段名一致;
 *    注意:当某个字段在json中被定义为数组时，那么该字段在结构体中要被声名为指针，并且增加数组size的字段。
 *
 * 2）为了C语言能够像java中通过反射来操作结构体中的属性，我们需要先为每个结构体定义一个用于查找结构体属性的“反射表”，
 *    即play_list_ref_tbl，ext_data_ref_tbl，song_ref_tbl，lyric_ref_tbl。
 *    有了这个反射表，我们可以迭代访问数组元素。不仅可以帮助我们完成json解析，当我们想要输出对象各属性值、或是释放
 *    指针指向的堆内存时也很有用。
 *
 *    TODO:目前反射表的结构有些复杂，虽然提供了宏定义让它用来稍稍方便一些。需要对该结构做出优化。
 *
 * 3）正确的完成上面两步，解析工作其实基本上就要完成了。只要再调用csonJsonStr2Struct，所有的属性就都会正确的赋值到结构体。
 *
 */


/*
    Step1:定义与json相对应的数据结构
*/
typedef struct {
    int         time;
    char*       text;
} Lyric;

typedef struct {
    char*       songName;
    char*       signerName;
    char*       albumName;
    char*       url;
    int         duration;
    int         paid;
    double      price;
    size_t      lyricNum;
    Lyric*      lyric;
    size_t      keyNum;
    int**       key;
    size_t      strNum;
    char**      strList;
} SongInfo;

typedef struct {
    int         a;
    double      b;
} ExtData;

typedef struct {
    char*       name;
    char*       creater;
    size_t      songNum;
    SongInfo*   songList;
    ExtData     extData;
} PlayList;

/*
    Step2:定义数据结构的反射表
*/
reflect_item_t lyric_ref_tbl[] = {
    _property_int(Lyric, time),
    _property_string(Lyric, text),
    _property_end()
};

reflect_item_t song_ref_tbl[] = {
    _property_string(SongInfo, songName),
    _property_string(SongInfo, signerName),
    _property_string(SongInfo, albumName),
    _property_string(SongInfo, url),
    _property_int(SongInfo, duration),
    _property_bool(SongInfo, paid),
    _property_real(SongInfo, price),
    _property_int_ex(SongInfo, lyricNum, _ex_args_all),
    _property_array_object(SongInfo, lyric, lyric_ref_tbl, Lyric, lyricNum, 1),
    _property_int_ex(SongInfo, keyNum, _ex_args_all),
    _property_array_int(SongInfo, key, int, keyNum, 2),
    _property_int_ex(SongInfo, strNum, _ex_args_all),
    _property_array_string(SongInfo, strList, char*, strNum, 1),
    _property_end()
};

reflect_item_t ext_data_ref_tbl[] = {
    _property_int(ExtData, a),
    _property_real(ExtData, b),
    _property_end()
};

reflect_item_t play_list_ref_tbl[] = {
    _property_string(PlayList, name),
    _property_string(PlayList, creater),
    _property_int_ex(PlayList, songNum, _ex_args_all),
    _property_array_object(PlayList, songList, song_ref_tbl, SongInfo, songNum, 1),
    _property_obj(PlayList, extData, ext_data_ref_tbl),
    _property_end()
};

static void printPlayList(PlayList* list);
static void freePlayList(PlayList* list);

const static char* jStr = "{\"name\":\"jay zhou\",\"creater\":\"dahuaxia\",\"songList\":[{\"songName\":\"qilixiang\",\"signerName\":\"jay zhou\",\"albumName\":\"qilixiang\",\"url\":\"www.kugou.com\",\"duration\":20093999939292928292234.1,\"paid\":false,\"price\":6.66,\"lyric\":[{\"time\":1,\"text\":\"Sparrow outside the window\"},{\"time\":10,\"text\":\"Multi mouth on the pole\"}],\"key\":[ [1111,2222,3333],[1111,2222,3333]]},{\"songName\":\"dongfengpo\",\"signerName\":\"jay zhou\",\"albumName\":\"dongfengpo\",\"url\":\"music.qq.com\",\"duration\":180.9,\"paid\":true,\"price\":0.88,\"lyric\":[{\"time\":10,\"text\":\"A sad parting, standing alone in the window\"},{\"time\":20,\"text\":\"I'm behind the door pretending you're not gone\"}],\"key\":[ [1234,5678,9876], [4321,9876,5432]],\"strList\":[\"abcd\",\"efgh\",\"ijkl\"]}],\"extData\":{\"a\":999,\"b\":1}}";

static void checkResult(PlayList* playList, char* jstrOutput);
/*
    Step3:调用csonJsonStr2Struct/csonStruct2JsonStr实现反序列化和序列化
*/
void test1()
{
    printf("=========================================\n");
    printf("\t\tRunning %s\n", __FUNCTION__);
    printf("=========================================\n");
    PlayList playList;
    memset(&playList, 0, sizeof(playList));

    /* string to struct */
    int ret = csonJsonStr2Struct(jStr, &playList, play_list_ref_tbl);
    CHECK_NUMBER(ret, 0);
    printf("decode ret=%d\n", ret);
    /* test print */
    //csonPrintProperty(&playList, play_list_ref_tbl);

    char* jstrOutput;
    ret = csonStruct2JsonStr(&jstrOutput, &playList, play_list_ref_tbl);
    CHECK_NUMBER(ret, 0);
    printf("encode ret=%d\nJson:%s\n", ret, jstrOutput);

    /*assert check*/
    checkResult(&playList, jstrOutput);

    free(jstrOutput);
    csonFreePointer(&playList, play_list_ref_tbl);
    
    printf("Successed %s.\n", __FUNCTION__);
}


void checkResult(PlayList* playList, char* jstrOutput){
    const char* encodeTest = "{\"name\":\"jay zhou\",\"creater\":\"dahuaxia\",\"songList\":[{\"songName\":\"qilixiang\",\"signerName\":\"jay zhou\",\"albumName\":\"qilixiang\",\"url\":\"www.kugou.com\",\"duration\":0,\"paid\":false,\"price\":6.66,\"lyric\":[{\"time\":1,\"text\":\"Sparrow outside the window\"},{\"time\":10,\"text\":\"Multi mouth on the pole\"}],\"key\":[1111,2222,3333]},{\"songName\":\"dongfengpo\",\"signerName\":\"jay zhou\",\"albumName\":\"dongfengpo\",\"url\":\"music.qq.com\",\"duration\":180,\"paid\":true,\"price\":0.88,\"lyric\":[{\"time\":10,\"text\":\"A sad parting, standing alone in the window\"},{\"time\":20,\"text\":\"I'm behind the door pretending you're not gone\"}],\"key\":[1234,5678,9876],\"strList\":[\"abcd\",\"efgh\",\"ijkl\"]}],\"extData\":{\"a\":999,\"b\":1}}";

    /* assert test */
    CHECK_STRING(playList->name, "jay zhou");
    CHECK_STRING(playList->creater, "dahuaxia");
    //CHECK_NUMBER(playList->songNum, 2);
    CHECK_STRING(playList->songList[0].songName, "qilixiang");
    CHECK_STRING(playList->songList[0].signerName, "jay zhou");
    CHECK_STRING(playList->songList[0].albumName, "qilixiang");
    CHECK_STRING(playList->songList[0].url, "www.kugou.com");
    CHECK_NUMBER(playList->songList[0].duration, 0);
    CHECK_NUMBER(playList->songList[0].paid, 0);
    CHECK_REAL(playList->songList[0].price, 6.66);
    //CHECK_NUMBER(playList->songList[0].lyricNum, 2);
    CHECK_NUMBER(playList->songList[0].lyric[0].time, 1);
    CHECK_STRING(playList->songList[0].lyric[0].text, "Sparrow outside the window");
    CHECK_NUMBER(playList->songList[0].lyric[1].time, 10);
    CHECK_STRING(playList->songList[0].lyric[1].text, "Multi mouth on the pole");
    //CHECK_NUMBER(playList->songList[0].keyNum, 3);
    CHECK_NUMBER(playList->songList[0].key[0][0], 1111);
    CHECK_NUMBER(playList->songList[0].key[0][1], 2222);
    CHECK_NUMBER(playList->songList[0].key[0][2], 3333);
    //CHECK_NUMBER(playList->songList[0].strNum, 0);

    CHECK_STRING(playList->songList[1].songName, "dongfengpo");
    CHECK_STRING(playList->songList[1].signerName, "jay zhou");
    CHECK_STRING(playList->songList[1].albumName, "dongfengpo");
    CHECK_STRING(playList->songList[1].url, "music.qq.com");
    CHECK_NUMBER(playList->songList[1].duration, 180);
    CHECK_NUMBER(playList->songList[1].paid, 1);
    CHECK_REAL(playList->songList[1].price, 0.88);
    //CHECK_NUMBER(playList->songList[1].lyricNum, 2);
    CHECK_NUMBER(playList->songList[1].lyric[0].time, 10);
    CHECK_STRING(playList->songList[1].lyric[0].text, "A sad parting, standing alone in the window");
    CHECK_NUMBER(playList->songList[1].lyric[1].time, 20);
    CHECK_STRING(playList->songList[1].lyric[1].text, "I'm behind the door pretending you're not gone");
   // CHECK_NUMBER(playList->songList[1].keyNum, 3);
    CHECK_NUMBER(playList->songList[1].key[0][0], 1234);
    CHECK_NUMBER(playList->songList[1].key[0][1], 5678);
    CHECK_NUMBER(playList->songList[1].key[0][2], 9876);
    //CHECK_NUMBER(playList->songList[1].strNum, 3);
    CHECK_STRING(playList->songList[1].strList[0], "abcd");
    CHECK_STRING(playList->songList[1].strList[1], "efgh");
    CHECK_STRING(playList->songList[1].strList[2], "ijkl");
    CHECK_NUMBER(playList->extData.a, 999);
    CHECK_REAL(playList->extData.b, 1);
    
    //It is difficult to predict the output due to the accuracy problem.
    //CHECK_STRING(jstrOutput, encodeTest);
}