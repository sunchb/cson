/**
 * @file   cson_reflect.h
 * @author sun_chb@126.com
 */
#ifndef _REFLECT_H_
#define _REFLECT_H_

#include "stddef.h"
#include "cson_interface.h"

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
    const struct reflect_item_t*  reflect_tbl;      /**< must be specified when type is object or object array */
    size_t                  arraySize;              /**< size of per array item. must be specified when type is array */
    char*                   arrayCountField;        /**< field saving array size */
    int                     nullable;               /**< paser return failure when the field is not found and nullable equals to 0 */
} reflect_item_t;

extern const reflect_item_t integerReflectTbl[];
extern const reflect_item_t stringReflectTbl[];
extern const reflect_item_t boolReflectTbl[];
extern const reflect_item_t realReflectTbl[];

#define _offset(type, field)                                                (&(((type*)0)->field))
#define _size(type, field)                                                  (sizeof(((type*)0)->field))
#define _property(type, field, jtype, tbl, nullable)                        {#field, _offset(type, field), _size(type, field), jtype, tbl, 0, NULL, nullable}
#define _property_end()                                                     {NULL, 0, 0, CSON_NULL, NULL, 0, NULL, 1}

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
 * @param count: property to save the array size
 *
 */
#define _property_array(type, field, tbl, subType, count)                   {#field, _offset(type, field), _size(type, field), CSON_ARRAY, tbl, sizeof(subType), #count, 1}
#define _property_array_object(type, field, tbl, subType, count)            _property_array(type, field, tbl, subType, count)
#define _property_array_int(type, field, subType, count)                    _property_array(type, field, integerReflectTbl, subType, count)
#define _property_array_string(type, field, subType, count)                 _property_array(type, field, stringReflectTbl, subType, count)
#define _property_array_real(type, field, subType, count)                   _property_array(type, field, realReflectTbl, subType, count)
#define _property_array_bool(type, field, subType, count)                   _property_array(type, field, boolReflectTbl, subType, count)

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
#define _property_array_nonull(type, field, tbl, subType, count)            {#field, _offset(type, field), _size(type, field), CSON_ARRAY, tbl, sizeof(subType), #count, 0}
#define _property_array_object_nonull(type, field, tbl, subType, count)     _property_array_nonull(type, field, tbl, subType, count)
#define _property_array_int_nonull(type, field, subType, count)             _property_array_nonull(type, field, integerReflectTbl, subType, count)
#define _property_array_string_nonull(type, field, subType, count)          _property_array_nonull(type, field, stringReflectTbl, subType, count)
#define _property_array_real_nonull(type, field, subType, count)            _property_array_nonull(type, field, realReflectTbl, subType, count)
#define _property_array_bool_nonull(type, field, subType, count)            _property_array_nonull(type, field, boolReflectTbl, subType, count)
/**
 * @brief get the address of field by retrieve the property table.
 *
 * @param obj: object to be operated.
 * @param field:
 * @param tbl: property table of the type.
 *
 * @return address of field.
 */
void*   csonGetProperty(void* obj, const char* field, const reflect_item_t* tbl);

/**
 * @brief set the field of object to specified data.
 *
 * @param obj: object to be operated.
 * @param field:
 * @param data: pointer of specified data.
 * @param tbl: property table of the type.
 *
 * @return void.
 */
void    csonSetProperty(void* obj, const char* field, void* data, const reflect_item_t* tbl);

/**
 * @brief set the field of object to specified data without retrieve the property table.
 *
 * @param obj: object to be operated.
 * @param data: pointer of specified data.
 * @param tbl: property of field.
 *
 * @return void.
 */
void    csonSetPropertyFast(void* obj, const void* data, const reflect_item_t* tbl);

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
#endif