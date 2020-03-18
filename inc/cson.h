/**
 * @file   cson.h
 * @author sun_chb@126.com
 */
#ifndef _CSON_H_
#define _CSON_H_

#include "stddef.h"

/**
 * @brief error code of parser.
 */
#define     ERR_NONE            (0)     /**< success */
#define     ERR_MEMORY          (-1)    /**< malloc failed */
#define     ERR_TYPE            (-2)    /**< type matching error */
#define     ERR_MISSING_FIELD   (-3)    /**< field not found */
#define     ERR_FORMAT          (-4)    /**< input json string format error */
#define     ERR_ARGS            (-5)    /**< args error */
#define     ERR_OVERFLOW        (-6)    /**< value overflow */

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

typedef size_t  cson_array_size_t;

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

/**
 * @brief the description of property in struct.
 *
 * @TODO: Try to simplify the struct
 */
typedef struct reflect_item_t {
    char*                   field;                  /**< field */
    size_t                  offset;                 /**< offset of property */
    size_t                  size;                   /**< size of property */
    cson_type               type;                   /**< corresponding json type */
    const struct reflect_item_t*  reflect_tbl;      /**< must be specified when type is object or array */
    size_t                  arrayItemSize;          /**< size of per array item. must be specified when type is array */
    int                     nullable;               /**< paser return failure when the field is not found and nullable equals to 0 */
} reflect_item_t;

extern const reflect_item_t integerReflectTbl[];
extern const reflect_item_t stringReflectTbl[];
extern const reflect_item_t boolReflectTbl[];
extern const reflect_item_t realReflectTbl[];

#define _offset(type, field)                                                (&(((type*)0)->field))
#define _size(type, field)                                                  (sizeof(((type*)0)->field))
#define _property(type, field, jtype, tbl, nullable)                        {#field, _offset(type, field), _size(type, field), jtype, tbl, 0, nullable}
#define _property_end()                                                     {NULL, 0, 0, CSON_NULL, NULL, 0, 1}

/**
 * @brief Declaring integer properties.
 *
 * @param type: type of struct
 * @param field: field name of properties
 *
 */
#define _property_int(type, field)                                          _property(type, field, CSON_INTEGER, integerReflectTbl, 1)

/**
 * @brief Declaring real properties.
 *
 * @param type: type of struct
 * @param field: field name of properties
 *
 */
#define _property_real(type, field)                                         _property(type, field, CSON_REAL, realReflectTbl, 1)

/**
 * @brief Declaring bool properties.
 *
 * @param type: type of struct
 * @param field: field name of properties
 *
 */
#define _property_bool(type, field)                                         _property(type, field, CSON_TRUE, boolReflectTbl, 1)

/**
 * @brief Declaring string properties.
 *
 * @param type: type of struct
 * @param field: field name of properties
 *
 */
#define _property_string(type, field)                                       _property(type, field, CSON_STRING, stringReflectTbl, 1)

/**
 * @brief Declaring struct properties.
 *
 * @param type: type of struct
 * @param field: field name of properties
 * @param tbl: property description table of sub-struct
 *
 */
#define _property_obj(type, field, tbl)                                     _property(type, field, CSON_OBJECT, tbl, 1)

/**
 * @brief Declaring array properties.
 *
 * @param type: type of struct
 * @param field: field name of properties
 * @param tbl: property description table of type of array
 * @param subType: type of array
 *
 */
#define _property_array(type, field, tbl, subType)                          {#field, _offset(type, field), _size(type, field), CSON_ARRAY, tbl, sizeof(subType), 1}
#define _property_array_object(type, field, tbl, subType)                   _property_array(type, field, tbl, subType)
#define _property_array_int(type, field, subType)                           _property_array(type, field, integerReflectTbl, subType)
#define _property_array_string(type, field, subType)                        _property_array(type, field, stringReflectTbl, subType)
#define _property_array_real(type, field, subType)                          _property_array(type, field, realReflectTbl, subType)
#define _property_array_bool(type, field, subType)                          _property_array(type, field, boolReflectTbl, subType)

/**
 * @brief nonull definitions. parser will stop and return error code when field not found which declared whit it.
 *
 * @param refer to comment of nullable definition
 */
#define _property_int_nonull(type, field)                                   _property(type, field, CSON_INTEGER, NULL, 0)
#define _property_real_nonull(type, field)                                  _property(type, field, CSON_REAL, NULL, 0)
#define _property_bool_nonull(type, field)                                  _property(type, field, CSON_TRUE, NULL, 0)
#define _property_string_nonull(type, field)                                _property(type, field, CSON_STRING, NULL, 0)
#define _property_obj_nonull(type, field, tbl)                              _property(type, field, CSON_OBJECT, tbl, 0)
#define _property_array_nonull(type, field, tbl, subType)                   {#field, _offset(type, field), _size(type, field), CSON_ARRAY, tbl, sizeof(subType), 0}
#define _property_array_object_nonull(type, field, tbl, subType)            _property_array_nonull(type, field, tbl, subType, count)
#define _property_array_int_nonull(type, field, subType)                    _property_array_nonull(type, field, integerReflectTbl, subType)
#define _property_array_string_nonull(type, field, subType)                 _property_array_nonull(type, field, stringReflectTbl, subType)
#define _property_array_real_nonull(type, field, subType)                   _property_array_nonull(type, field, realReflectTbl, subType)
#define _property_array_bool_nonull(type, field, subType)                   _property_array_nonull(type, field, boolReflectTbl, subType)

/**
 * @brief function type of csonLoopProperty.
 *
 * @param obj: pointer of property.
 * @param tbl: property of field.
 *
 * @return void*(reserved).
 */
typedef void* (*loop_func_t)(void* pData, const reflect_item_t* tbl);

/**
 * @brief loop all property and process property by @func
 *
 * @param obj: object to be operated.
 * @param tbl: property of field.
 * @param func: callback
 *
 * @return void.
 */
void    csonLoopProperty(void* obj, const reflect_item_t* tbl, loop_func_t func);

/**
 * @brief convert json string to struct object.
 *
 * @param jstr: json string
 * @param output: target object
 * @param tbl: property table of output.
 *
 * @return ERR_NONE on success, otherwise failed.
 */
int csonJsonStr2Struct(const char* jstr, void* output, const reflect_item_t* tbl);

/**
 * @brief convert struct object to json string.
 *
 * @param jstr: output json string
 * @param output: input struct object
 * @param tbl: property table of input.
 *
 * @return ERR_NONE on success, otherwise failed.
 */
int csonStruct2JsonStr(char** jstr, void* input, const reflect_item_t* tbl);

/**
 * @brief Iterative output properties of data
 *
 * @param pData: struct object
 * @param tbl: property table of input.
 *
 * @return void.
 */
void csonPrintProperty(void* pData, const reflect_item_t* tbl);

/**
 * @brief Iterative free pointer of data
 *
 * @param pData: struct object
 * @param tbl: property table of input.
 *
 * @return void.
 */
void csonFreePointer(void* list, const reflect_item_t* tbl);


cson_array_size_t csonGetArraySize(void* pArray);


cson_array_size_t csonGetArraySizeByField(void* pData, const char* field, const reflect_item_t* tbl);
#endif