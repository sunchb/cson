/**
 * @file   cson_interface.h
 * @author sun_chb@126.com
 */
#ifndef _CSON_INTERFACE_
#define _CSON_INTERFACE_

/**
 * Initially I used the Jansson library as a parser. The Jansson library
 *  is powerful, but it obviously increases the size of the executable. 
 * So, I found cjson, which is very small and convenient for code 
 * integration. Now I want to support Jansson and cjson, so I need 
 * to abstract the parser into an interface. Users can choose which one
 * to use, or define other parser which conforms to this interface.
 * */

#include "stddef.h"

/**
 * @brief the type of json object.
 */
typedef enum {
    CSON_OBJECT,
    CSON_ARRAY,
    CSON_STRING,
    CSON_INTEGER,
    CSON_REAL,
    CSON_TRUE,
    CSON_FALSE,
    CSON_NULL
} cson_type;

typedef void*   cson_t;

/**
 * @brief define the function type of json parse/pack function group
 */
typedef cson_t      (*func_cson_object_get)(const cson_t object, const char* key);
typedef cson_type   (*func_cson_typeof)(cson_t object);
typedef cson_t      (*func_cson_loadb)(const char *buffer, size_t buflen);
typedef void        (*func_cson_decref)(cson_t object);
typedef const char* (*func_cson_string_value)(const cson_t object);
typedef size_t      (*func_cson_string_length)(const cson_t object);
typedef long long   (*func_cson_integer_value)(const cson_t object);
typedef double      (*func_cson_real_value)(const cson_t object);
typedef char        (*func_cson_bool_value)(const cson_t object);
typedef size_t      (*func_cson_array_size)(const cson_t object);
typedef cson_t      (*func_cson_array_get)(const cson_t object, size_t index);
typedef cson_t      (*func_cson_new)();
typedef char*       (*func_cson_to_string)(cson_t object);
typedef cson_t      (*func_cson_integer)(long long val);
typedef cson_t      (*func_cson_string)(const char* val);
typedef cson_t      (*func_cson_bool)(char val);
typedef cson_t      (*func_cson_real)(double val);
typedef cson_t      (*func_cson_array)();
typedef int         (*func_cson_array_add)(cson_t array, cson_t obj);
typedef int         (*func_cson_object_set_new)(cson_t rootObj, const char* field, cson_t obj);

/**
 * @brief define the cson interface
 */
typedef struct {
    func_cson_object_get cson_object_get;
    func_cson_typeof cson_typeof;
    func_cson_loadb cson_loadb;
    func_cson_decref cson_decref;
    func_cson_string_value cson_string_value;
    func_cson_string_length cson_string_length;
    func_cson_integer_value cson_integer_value;
    func_cson_real_value cson_real_value;
    func_cson_bool_value cson_bool_value;
    func_cson_array_size cson_array_size;
    func_cson_array_get cson_array_get;
    func_cson_new cson_object;
    func_cson_to_string cson_to_string;
    func_cson_integer cson_integer;
    func_cson_string cson_string;
    func_cson_bool cson_bool;
    func_cson_real cson_real;
    func_cson_array cson_array;
    func_cson_array_add cson_array_add;
    func_cson_object_set_new cson_object_set_new;
} cson_interface;

extern cson_interface csomImpl;

#define cson_object_get csomImpl.cson_object_get
#define cson_typeof csomImpl.cson_typeof
#define cson_loadb csomImpl.cson_loadb
#define cson_decref csomImpl.cson_decref
#define cson_string_value csomImpl.cson_string_value
#define cson_string_length csomImpl.cson_string_length
#define cson_integer_value csomImpl.cson_integer_value
#define cson_real_value csomImpl.cson_real_value
#define cson_bool_value csomImpl.cson_bool_value
#define cson_array_size csomImpl.cson_array_size
#define cson_array_get csomImpl.cson_array_get
#define cson_object csomImpl.cson_object
#define cson_to_string csomImpl.cson_to_string
#define cson_integer csomImpl.cson_integer
#define cson_string csomImpl.cson_string
#define cson_bool csomImpl.cson_bool
#define cson_real csomImpl.cson_real
#define cson_array csomImpl.cson_array
#define cson_array_add csomImpl.cson_array_add
#define cson_object_set_new csomImpl.cson_object_set_new

#define cson_is_number(type) (type == CSON_REAL || type == CSON_INTEGER)
#define cson_is_bool(type)    (type == CSON_TRUE || type == CSON_FALSE)

#endif