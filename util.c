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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wi-fi.h"
#include "util.h"

/*
 * Define to enable malloc debugging which creates an allocation larger
 * than the requested size, then embeds the allocated size and 2 canary
 * bytes before and after the allocation.  On xfree(), the canary bytes
 * are verified and if any are not correct, the program panics.
 */
/* #define MALLOC_DEBUG 1 */

#define ERROR_STRING_SIZE	1024

enum
{
	STOP_ALERT,
	CAUTION_ALERT,
	NOTE_ALERT,
	APPICON_ALERT
};


/*
 * Util helper needed to be called at program startup, to pre-allocate
 * some things that we can't do during errors.
 */

void util_init(void)
{
}

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


/*
 * BSD err(3) and warn(3) functions
 */

static void vwarn(short alert_func, const char *format, va_list ap)
{
	char icon;
	char err_str[ERROR_STRING_SIZE];
	char alert_str[ERROR_STRING_SIZE];

	vsprintf(err_str, format, ap);

	switch (alert_func)
	{
	case CAUTION_ALERT:
		icon = '1';
		break;
	case NOTE_ALERT:
		icon = '1';
		break;
	default:
		icon = '3';
		break;
	}
	sprintf(alert_str, rs_str(AL_WARN), icon, err_str);
	form_alert(1, alert_str);
}


static void ExitToShell(void)
{
	_WORD message[8];

	if (!_app || multi())
	{
		if (gl_apid > 0)
			appl_exit();
		exit(1);
	}
	for (;;)
		evnt_mesag(message);
}


void panic(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vwarn(STOP_ALERT, format, args);
	va_end(args);

	ExitToShell();
}


void warn(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vwarn(CAUTION_ALERT, format, args);
	va_end(args);
}


void note(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vwarn(NOTE_ALERT, format, args);
	va_end(args);
}


short ask(const char *format, ...)
{
	char err_str[ERROR_STRING_SIZE];
	char alert_str[ERROR_STRING_SIZE];
	va_list args;
	
	va_start(args, format);
	vsprintf(err_str, format, args);
	va_end(args);
	sprintf(alert_str, rs_str(AL_ASK), err_str);
	return form_alert(1, alert_str) == 1;
}


void center_in_screen(OBJECT *tree, GRECT *gr)
{
	GRECT screen;
	
	wind_get_grect(0, WF_WORKXYWH, &screen);
	form_center_grect(tree, gr);
	tree[ROOT].ob_x = gr->g_x;
	tree[ROOT].ob_y = gr->g_y;
	gr->g_x -= 3;
	gr->g_y -= 3;
	gr->g_w = tree[ROOT].ob_width + 6;
	gr->g_h = tree[ROOT].ob_height + 6;
	if (gr->g_x < screen.g_x)
		gr->g_x = screen.g_x;
	if (gr->g_y < screen.g_y)
		gr->g_y = screen.g_y;
	if (gr->g_x + gr->g_w > screen.g_x + screen.g_w)
		gr->g_w = screen.g_x + screen.g_w - gr->g_x;
	if (gr->g_y + gr->g_h > screen.g_y + screen.g_h)
		gr->g_h = screen.g_y + screen.g_h - gr->g_y;
}


#if 0
/*
 * General Mac-specific GUI functions
 */

pascal bool ModalDialogFilter(DialogPtr dlg, EventRecord *event, short *hit)
{
	WindowPtr event_win;
	short event_in;
	char key;

	switch (event->what)
	{
	case mouseDown:
		event_in = FindWindow(event->where, &event_win);

		switch (event_in)
		{
		case inGoAway:
			if (TrackGoAway(dlg, event->where))
			{
				*hit = -1;
				return true;
			}
		}
		break;
	case keyDown:
		key = event->message & charCodeMask;

		if (event->modifiers & cmdKey)
		{
			switch (key)
			{
			case 'x':
				ZeroScrap();
				DlgCut(dlg);
				break;
			case 'c':
				ZeroScrap();
				DlgCopy(dlg);
				break;
			case 'v':
				if (TEFromScrap() == noErr)
					DlgPaste(dlg);
				break;
			}
			event->what = nullEvent;
			return false;
		} else if (key == 13 || key == 3)
		{
			/* OK button */
			*hit = OK;
			return true;
		}

		break;
	}

	return false;
}

static short _password_dialog_ditl_id = -1;
static char *_password_dialog_storage = NULL;
static size_t _password_dialog_storage_len = 0;

void PasswordDialogFieldFilterSetup(short ditl_id, char *storage, size_t len)
{
	_password_dialog_ditl_id = ditl_id;
	_password_dialog_storage = storage;
	_password_dialog_storage_len = len;

	memset(_password_dialog_storage, 0, len);
}

pascal bool PasswordDialogFieldFilter(DialogPtr dlg, EventRecord *event, short *hit)
{
	DialogPeek dlgp;
	short sel_start,
	 sel_end;
	char key;

	dlgp = (DialogPeek) dlg;
	if (dlgp->editField == _password_dialog_ditl_id - 1)
	{
		sel_start = (*(dlgp->textH))->selStart;
		sel_end = (*(dlgp->textH))->selEnd;

		switch (event->what)
		{
		case keyDown:
		case autoKey:
			key = event->message & charCodeMask;
			if (event->modifiers & cmdKey)
			{
				/* TODO: implement DlgPaste for cmd+v? */
				event->what = nullEvent;
				return false;
			}

			if (key == 8)
			{
				/* backspace */
				if (sel_start == sel_end && sel_start > 0)
					memmove(_password_dialog_storage + sel_start - 1,
							_password_dialog_storage + sel_start, _password_dialog_storage_len - sel_start - 1);
				else if (sel_start != sel_end)
					memmove(_password_dialog_storage + sel_start,
							_password_dialog_storage + sel_end, _password_dialog_storage_len - sel_end - 1);
			} else if (sel_start >= _password_dialog_storage_len)
			{
				event->what = nullEvent;
				return false;
			} else if (key >= ' ' && key <= '~')
			{
				if (sel_start != sel_end)
					/* delete selection before making space for new char */
					memmove(_password_dialog_storage + sel_start,
							_password_dialog_storage + sel_end, _password_dialog_storage_len - sel_end - 1);
				memmove(_password_dialog_storage + sel_start + 1,
						_password_dialog_storage + sel_start, _password_dialog_storage_len - sel_start - 1);
				_password_dialog_storage[sel_start] = key;
				event->message = '¥';
			}
			_password_dialog_storage[(*(dlgp->textH))->teLength + 1] = '\0';
			sel_start = 0;
			break;
		}
	}

	return ModalDialogFilter(dlg, event, hit);
}

void PasswordDialogFieldFinish(void)
{
	_password_dialog_ditl_id = -1;
	_password_dialog_storage = NULL;
	_password_dialog_storage_len = 0;
}
#endif

/* C Extensions */

/*
 * Copy string src to buffer dst of size dsize.  At most dsize-1
 * chars will be copied.  Always NUL terminates (unless dsize == 0).
 * Returns strlen(src); if retval >= dsize, truncation occurred.
 */
size_t strlcpy(char *dst, const char *src, size_t dsize)
{
	const char *osrc = src;
	size_t nleft = dsize;

	/* Copy as many bytes as will fit. */
	if (nleft != 0)
	{
		while (--nleft != 0)
		{
			if ((*dst++ = *src++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src. */
	if (nleft == 0)
	{
		if (dsize != 0)
			*dst = '\0';				/* NUL-terminate dst */
		while (*src++)
			;
	}

	return (src - osrc - 1);			/* count does not include NUL */
}


/*** ---------------------------------------------------------------------- ***/

void obj_set_ptext(OBJECT *tree, _WORD obj, const char *str)
{
	_WORD len;
	TEDINFO *ted;
	_WORD maxlen;
	char *text;
	
	if (str == NULL)
		return;
	tree += obj;
	switch (tree->ob_type & 0xff)
	{
	case G_TEXT:
	case G_BOXTEXT:
	case G_FTEXT:
	case G_FBOXTEXT:
		ted = tree->ob_spec.tedinfo;
		text = ted->te_ptext;
		maxlen = ted->te_txtlen - 1;
		break;
	case G_STRING:
	case G_BUTTON:
	case G_TITLE:
		text = tree->ob_spec.free_string;
		maxlen = -1;
		break;
	default:
		return;
	}
	len = (_WORD)strlen(str);
	if (maxlen >= 0 && len > maxlen)
	{
		len = maxlen;
	}
	strncpy(text, str, len);
	text[len] = '\0';
}

/*** ---------------------------------------------------------------------- ***/

char *obj_get_ptext(OBJECT *obj)
{
	switch (obj->ob_type & 0xff)
	{
	case G_TEXT:
	case G_BOXTEXT:
	case G_FTEXT:
	case G_FBOXTEXT:
		return obj->ob_spec.tedinfo->te_ptext;
	case G_STRING:
	case G_BUTTON:
	case G_TITLE:
		return obj->ob_spec.free_string;
	default:
		return NULL;
	}
}


