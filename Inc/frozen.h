/*
 * Copyright (c) 2004-2013 Sergey Lyubka <valenok@gmail.com>
 * Copyright (c) 2013 Cesanta Software Limited
 * All rights reserved
 *
 * This library is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http: *www.gnu.org/licenses/>.
 *
 * You are free to use this library under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this library under a commercial
 * license, as set out in <http://cesanta.com/products.html>.
 */

#ifndef FROZEN_HEADER_INCLUDED
#define FROZEN_HEADER_INCLUDED

#include "globals.h"

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

//#include <stdarg.h>

    enum json_type {
        JSON_TYPE_EOF = 0,      /* End of parsed tokens marker */
        JSON_TYPE_STRING = 1,
        JSON_TYPE_NUMBER = 2,
        JSON_TYPE_OBJECT = 3,
        JSON_TYPE_TRUE = 4,
        JSON_TYPE_FALSE = 5,
        JSON_TYPE_NULL = 6,
        JSON_TYPE_ARRAY = 7
    };

    struct json_token {
        const char *ptr;        /* Points to the beginning of the token */
        card len;                /* Token length */
        card num_desc;           /* For arrays and object, total number of descendants */
        enum json_type type;    /* Type of the token, possible values above */
    };

/* Error codes */
#define JSON_STRING_INVALID           -1
#define JSON_STRING_INCOMPLETE        -2
#define JSON_TOKEN_ARRAY_TOO_SMALL    -3

    card parse_json(const char *json_string, card json_string_length, struct json_token *tokens_array, card size_of_tokens_array);
    struct json_token *parse_json2(const char *json_string, card string_length);
    struct json_token *find_json_token(struct json_token *toks, const char *path);

    card json_emit_long(char *buf, card buf_len,card value);
    card json_emit_double(char *buf, card buf_len, double value);
    card json_emit_quoted_str(char *buf, card buf_len, const char *str, card len);
    card json_emit_unquoted_str(char *buf, card buf_len, const char *str, card len);
    card json_emit(char *buf, card buf_len, const char *fmt, ...);
    card json_emit_va(char *buf, card buf_len, const char *fmt, va_list);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* FROZEN_HEADER_INCLUDED */
