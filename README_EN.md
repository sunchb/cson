# [简体中文](./README.md) | English

# cson
Transformation between json string and struct.

The transformation between string to JSON objects (e.g. json_t in Jansson) 
is implemented by a third-party library (Jansson or cJSON. Refer to Dependence).
What cson really implements is the transformation between JSON object and structure.

## Compilation 
### Compile static library
``` shell
$ git clone https://github.com/sunchb/cson.git
$ cd cson
$ make
```
### Compile demo
``` shell
$ cd demo
$ make
$ ./test
```
## How to use
1. Define the data structure for JSON.
2. Define the property description table for data structure.
3. Call cson api to decode or encode.

## Dependence
https://github.com/akheron/jansson.git
https://github.com/DaveGamble/cJSON.git

## Demo
Here is a JSON with various data types. eg. Interger, String, Real, Boolean, Object, and Array.
We're going to decode and encode it by cson.

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

### 1. Define the data structure for JSON.
Even if you don't use cson, you usually need to do like this.

#### Attention
- String must be declared as char*.
- Array must be declared as pointer.
- Declare additional properties to hold the array size for every array property.

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

### 2. Define the property description table for data structure.
Use following definitions to help you describe the structure.
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

Args notes:
- type:         type of data structure
- field:        property name
- tbl:          description table of the property type. use when object or object array
- arrayType:    type of the array (Used to calculate size when dynamically get array memory)
- countfild:    property to save array size

You can also use macro definitions with extended parameters, the range of args is _ex_args_nullable, _ex_args_exclude_decode, _ex_args_exclude_encode and thire combinations.
- #define _ex_args_nullable         (0x01)  //continue parse or encode when the field exception occurred. Default is "ON". 
- #define _ex_args_exclude_decode   (0x02)  //do not parse the field.
- #define _ex_args_exclude_encode   (0x04)  //do not encode the field.
- #define _ex_args_all              (_ex_args_nullable | _ex_args_exclude_decode | _ex_args_exclude_encode)

- _property_int_ex(type, field, args)
- _property_real_ex(type, field, args)
- _property_bool_ex(type, field, args)
- _property_string_ex(type, field, args)
- _property_obj_ex(type, field, tbl, args)
- _property_array_ex(type, field, tbl, subType, count, args)
- _property_array_object_ex(type, field, tbl, subType, count, args)
- _property_array_int_ex(type, field, subType, count, args)
- _property_array_string_ex(type, field, subType, count, args)
- _property_array_real_ex(type, field, subType, count, args)
- _property_array_bool_ex(type, field, subType, count, args)

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

### 3. Call cson api to decode or encode.
``` c
PlayList playList;

/* Decode */
csonJsonStr2Struct(jStr, &playList, play_list_ref_tbl);

/* Encode */
char* jstrOutput;
csonStruct2JsonStr(&jstrOutput, &playList, play_list_ref_tbl);
```

## Restrict
- Multidimensional arrays are not supported.
- Cjson is used by default. If you want to use jannson or other json lib, please modify $(JSON_LIB) in makefile.
