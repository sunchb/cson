# 简体中文 | [English](./README.md)

# cson
轻松完成C语言结构体和Json的转换。

其中Json字符串与Json对象（例如Jansson库中的json_t）之间的转换由第三方库实现（例如Jansson或者cJSON，请参考依赖库）。
cson真正实现的是Json对象与结构体间的转换。

## 编译
### 编译静态库
``` shell
$ git clone https://github.com/sunchb/cson.git
$ cd cson
$ make
```
### 编译示例
``` shell
$ cd demo
$ make
$ ./test
```
## 如何使用cson
1. 定义与Json协议对应的结构体。
2. 定义结构体的属性描述表。
3. 调用cson接口完成转换。

## 依赖
https://github.com/akheron/jansson.git
https://github.com/DaveGamble/cJSON.git

## 示例
下面具有各种数据类型的JSON。例如整数，字符串，实数，布尔值，对象和数组。我们将通过cson对其进行解码和编码。

``` json
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
            ]
        }
    ],
    "extData":{
        "a":999,
        "b":1.05
    }
}
```

### 1. 定义与Json协议对应的结构体。
即使不使用cson，通常我们也会这么做。

#### 注意事项
- 字符串必须定义为char*类型。
- 数组必须定义为指针类型。
- 如果结构体包含数组，需要为每一个数组定义一个额外的属性，用于保存数组大小。

``` c
typedef struct {
    int         time;
    char*       text;         /* String must be declared as char* */
} Lyric;

typedef struct {
    char*       songName;
    char*       signerName;
    char*       albumName;
    char*       url;
    int         duration;
    int         paid;
    double      price;
    size_t      lyricNum;     /* Declare additional properties to hold the array size */
    Lyric*      lyric;        /* Array must be declared as pointer */
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
```

### 2. 定义结构体的属性描述表。
使用以下宏定义描述结构体属性。
- _property_int(type, field) 
- _property_real(type, field)
- _property_bool(type, field)
- _property_string(type, field)
- _property_obj(type, field, tbl) 
- _property_array_object(type, field, tbl, arrayType, countfild) # I know it's a little cumbersome, I'm trying to simplify it.
- _property_array_int(type, field, arrayType, countfild)
- _property_array_string(type, field, arrayType, countfild)
- _property_array_real(type, field, arrayType, countfild)
- _property_array_bool(type, field, arrayType, countfild)

参数说明：
- type:         type of data structure
- field:        property name
- tbl:          description table of the property type. use when object or object array
- arrayType:    type of the array (Used to calculate size when dynamically get array memory)
- countfild:    property to save array size

``` c
/* description for Lyric */
reflect_item_t lyric_ref_tbl[] = {
    _property_int(Lyric, time),
    _property_string(Lyric, text),
    _property_end()
};

/* description for SongInfo */
reflect_item_t song_ref_tbl[] = {
    _property_string(SongInfo, songName),
    _property_string(SongInfo, signerName),
    _property_string(SongInfo, albumName),
    _property_string(SongInfo, url),
    _property_int(SongInfo, duration),
    _property_bool(SongInfo, paid),
    _property_real(SongInfo, price),
    _property_int(SongInfo, lyricNum),          
    _property_array(SongInfo, lyric, lyric_ref_tbl, Lyric, lyricNum),   /* Lyric: type of array; lyricNum: property to save array size */
    _property_end()
};

/* description for ExtData */
reflect_item_t ext_data_ref_tbl[] = {
    _property_int(ExtData, a),
    _property_real(ExtData, b),
    _property_end()
};

/* description for PlayList */
reflect_item_t play_list_ref_tbl[] = {
    _property_string(PlayList, name),
    _property_string(PlayList, creater),
    _property_int(PlayList, songNum),
    _property_array(PlayList, songList, song_ref_tbl, SongInfo, songNum),
    _property_obj(PlayList, extData, ext_data_ref_tbl),
    _property_end()
};
```

### 3. 调用cson接口编解码Json。
``` c
PlayList playList;

/* Decode */
csonJsonStr2Struct(jStr, &playList, play_list_ref_tbl);

/* Encode */
char* jstrOutput;
csonStruct2JsonStr(&jstrOutput, &playList, play_list_ref_tbl);
```

## 限制
- 暂不支持多维数组。
- 默认使用Cjson库。如果需要使用jannson，请修改Makefile中的$(JSON_LIB)变量。
