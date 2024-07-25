/*
 * Copyright (c) 2024 Thorsten Otto
 * based on code
 * Copyright (c) 2020-2022 joshua stein <jcs@jcs.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <limits.h>

#define multi() (_AESnumapps != 1)

void util_init(void);

void *xmalloc(size_t);
void xfree(void *ptrptr);
void *xmalloczero(size_t);
void *xcalloc(size_t, size_t);

__attribute__((format(printf, 1, 2)))
void panic(const char *format, ...);
__attribute__((format(printf, 1, 2)))
void warn(const char *format, ...);
__attribute__((format(printf, 1, 2)))
void note(const char *format, ...);
__attribute__((format(printf, 1, 2)))
int ask(const char *format, ...);

void center_in_screen(OBJECT *tree, GRECT *b);

const char *rs_str(_WORD id);
OBJECT *rs_tree(_WORD id);
BITBLK *rs_image(_WORD id);
void obj_set_ptext(OBJECT *tree, _WORD id, const char *str);
char *obj_get_ptext(OBJECT *obj);

void dd_nak(const _WORD *message);


size_t strlcpy(char *dst, const char *src, size_t dsize);

#endif
