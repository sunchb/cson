/**
 * @file   cson.h
 * @author sun_chb@126.com
 */
#ifndef _CSON_H_
#define _CSON_H_

#include "cson_reflect.h"

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

#endif