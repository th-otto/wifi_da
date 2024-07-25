/*
 * Copyright (c) 2024 Thorsten Otto
 * based on code
 * Copyright (c) 2020-2022 joshua stein <jcs@jcs.org>
 * Copyright (c) 1998, 2015 Todd C. Miller <millert@openbsd.org>
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
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

#include <stdarg.h>
#include <string.h>
#include "wi-fi.h"
#include "util.h"

/* strangely, this is defined only in stdint.h, not limits.h */
#ifndef SIZE_MAX
#ifdef __SIZE_MAX__
#define SIZE_MAX __SIZE_MAX__
#else
#define SIZE_MAX 4294967295UL
#endif
#endif

/*
 * Define to enable malloc debugging which creates an allocation larger
 * than the requested size, then embeds the allocated size and 2 canary
 * bytes before and after the allocation.  On xfree(), the canary bytes
 * are verified and if any are not correct, the program panics.
 */
/* #define MALLOC_DEBUG 1 */


/*
 * Memory functions
 */

void *xmalloc(size_t size)
{
#ifdef MALLOC_DEBUG
	void *ptr;
	unsigned char *cptr;
#endif

	if (size == 0)
		panic("xmalloc: zero size");

#ifdef MALLOC_DEBUG
	ptr = malloc(size + 8);

	cptr = (unsigned char *) ptr;
	cptr[0] = 0xff;
	cptr[1] = 0xff;
	cptr[2] = (size >> 24) & 0xff;
	cptr[3] = (size >> 16) & 0xff;
	cptr[4] = (size >> 8) & 0xff;
	cptr[5] = size & 0xff;

	cptr[6 + size] = 0xff;
	cptr[6 + size + 1] = 0xff;

	return cptr + 6;
#else
	return malloc(size);
#endif
}

void xfree(void *ptrptr)
{
	void **addr = (void **) ptrptr;

#ifdef MALLOC_DEBUG
	size_t size;
	unsigned char *cptr;
#endif
	void *ptr;

	if (ptrptr == NULL)
		panic("xfree(NULL)");

	ptr = *addr;
	if (ptr == NULL)
		panic("xfree(&NULL) likely a double-free");

#ifdef MALLOC_DEBUG
	cptr = (unsigned char *) ptr - 6;
	if (cptr[0] != 0xff || cptr[1] != 0xff)
		panic("xfree() pre-buf canary dead");

	size = ((unsigned long) (cptr[2]) << 24) |
		((unsigned long) (cptr[3]) << 16) | ((unsigned long) (cptr[4]) << 8) | (unsigned long) (cptr[5]);

	if (cptr[6 + size] != 0xff || cptr[6 + size + 1] != 0xff)
		panic("xfree() post-buf canary dead");

	free(cptr);
#else
	free(ptr);
#endif

	*addr = NULL;
}


void *xmalloczero(size_t size)
{
	void *ptr;

	ptr = xmalloc(size);
	if (ptr != NULL)
		memset(ptr, 0, size);

	return ptr;
}


void *xcalloc(size_t nmemb, size_t size)
{
	if (nmemb > 0 && SIZE_MAX / nmemb < size)
		panic("xcalloc(%lu, %lu) overflow", nmemb, size);

	return xmalloczero(nmemb * size);
}
