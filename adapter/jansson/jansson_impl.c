/**
 * @file   jansson_impl.c
 * @author sun_chb@126.com
 */
#include "cson.h"
#include "jansson.h"
#include "stdio.h"

cson_t  jansson_impl_object_get(const cson_t object, const char* key){
    return json_object_get((json_t*)object, key);
}

cson_type jansson_impl_typeof(cson_t object){
    return (cson_type)json_typeof((json_t*)object);
}

cson_t jansson_impl_loadb(const char *buffer, size_t buflen){
    cson_t ret = NULL;
    json_error_t err;

    ret = json_loadb(buffer, buflen, JSON_DECODE_ANY, &err);

    if(!ret){
        printf("line:%d,column:%d,pos:%d,source:%s,text:%s\n",
                    err.line,
                    err.column,
                    err.position,
                    err.source,
                    err.text);
    }
    return ret;
}

void jansson_impl_decref(cson_t object){
    json_decref((json_t*)object);
}

const char *jansson_impl_string_value(const cson_t object){
    return json_string_value((json_t*)object);
}

size_t jansson_impl_string_length(const cson_t object){
    return json_string_length((json_t*)object);
}

long long jansson_impl_integer_value(const cson_t object){
    return json_integer_value((json_t*)object);
}

double jansson_impl_real_value(const cson_t object){
    return json_real_value((json_t*)object);
}

char jansson_impl_bool_value(const cson_t object){
    return json_boolean_value((json_t*)object);
}

size_t jansson_impl_array_size(const cson_t object){
    return json_array_size((json_t*)object);
}

cson_t jansson_impl_array_get(const cson_t object, size_t index){
    return json_array_get((json_t*)object, index);
}

cson_t jansson_impl_new(){
    return json_object();
}

char* jansson_impl_to_string(cson_t object){
    return json_dumps((json_t*)object, JSON_COMPACT | JSON_PRESERVE_ORDER);
}

cson_t jansson_impl_integer(long long val){
    return json_integer(val);
}

cson_t jansson_impl_string(const char* val){
    return json_string(val);
}

cson_t jansson_impl_bool(char val){
    return json_boolean(val);
}

cson_t jansson_impl_real(double val){
    return json_real(val);
}

cson_t jansson_impl_array(){
    return json_array();
}

int jansson_impl_array_add(cson_t array, cson_t obj){
    return json_array_append_new((json_t*)array, (json_t*)obj);
}

int jansson_impl_object_set_new(cson_t rootObj, const char* field, cson_t obj){
    return json_object_set_new((json_t*)rootObj, field, (json_t*)obj);
}

cson_interface csomImpl = {
    jansson_impl_object_get,
    jansson_impl_typeof,
    jansson_impl_loadb,
    jansson_impl_decref,
    jansson_impl_string_value,
    jansson_impl_string_length,
    jansson_impl_integer_value,
    jansson_impl_real_value,
    jansson_impl_bool_value,
    jansson_impl_array_size,
    jansson_impl_array_get,
    jansson_impl_new,
    jansson_impl_to_string,
    jansson_impl_integer,
    jansson_impl_string,
    jansson_impl_bool,
    jansson_impl_real,
    jansson_impl_array,
    jansson_impl_array_add,
    jansson_impl_object_set_new
};
