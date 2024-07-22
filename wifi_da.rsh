/*
 * GEM resource C output of wifi_da
 *
 * created by ORCS 2.18
 */

#if !defined(__GNUC__) || !defined(__mc68000__)
#include <portab.h>
#endif

#ifndef __STDC__
# ifdef __PUREC__
#  define __STDC__ 1
# endif
#endif

#ifdef OS_WINDOWS
#  include <portaes.h>
#  define SHORT _WORD
#  ifdef __WIN32__
#    define _WORD signed short
#  else
#    define _WORD signed int
 #   pragma option -zE_FARDATA
#  endif
#else
#  ifdef __TURBOC__
#    include <aes.h>
#    define CP (_WORD *)
#  endif
#endif

#ifdef OS_UNIX
#  include <portaes.h>
#  define SHORT _WORD
#else
#  ifdef __GNUC__
#    ifndef __PORTAES_H__
#      if __GNUC__ < 4
#        include <aesbind.h>
#        ifndef _WORD
#          define _WORD int
#        endif
#        define CP (char *)
#      else
#        include <mt_gem.h>
#        ifndef _WORD
#          define _WORD short
#        endif
#        define CP (short *)
#      endif
#      define CW (short *)
#    endif
#  endif
#endif


#ifdef __SOZOBONX__
#  include <xgemfast.h>
#else
#  ifdef SOZOBON
#    include <aes.h>
#  endif
#endif

#ifdef MEGAMAX
#  include <gembind.h>
#  include <gemdefs.h>
#  include <obdefs.h>
#  define _WORD int
#  define SHORT int
#endif

#ifndef _VOID
#  define _VOID void
#endif

#ifndef OS_NORMAL
#  define OS_NORMAL 0x0000
#endif
#ifndef OS_SELECTED
#  define OS_SELECTED 0x0001
#endif
#ifndef OS_CROSSED
#  define OS_CROSSED 0x0002
#endif
#ifndef OS_CHECKED
#  define OS_CHECKED 0x0004
#endif
#ifndef OS_DISABLED
#  define OS_DISABLED 0x0008
#endif
#ifndef OS_OUTLINED
#  define OS_OUTLINED 0x0010
#endif
#ifndef OS_SHADOWED
#  define OS_SHADOWED 0x0020
#endif
#ifndef OS_WHITEBAK
#  define OS_WHITEBAK 0x0040
#endif
#ifndef OS_DRAW3D
#  define OS_DRAW3D 0x0080
#endif

#ifndef OF_NONE
#  define OF_NONE 0x0000
#endif
#ifndef OF_SELECTABLE
#  define OF_SELECTABLE 0x0001
#endif
#ifndef OF_DEFAULT
#  define OF_DEFAULT 0x0002
#endif
#ifndef OF_EXIT
#  define OF_EXIT 0x0004
#endif
#ifndef OF_EDITABLE
#  define OF_EDITABLE 0x0008
#endif
#ifndef OF_RBUTTON
#  define OF_RBUTTON 0x0010
#endif
#ifndef OF_LASTOB
#  define OF_LASTOB 0x0020
#endif
#ifndef OF_TOUCHEXIT
#  define OF_TOUCHEXIT 0x0040
#endif
#ifndef OF_HIDETREE
#  define OF_HIDETREE 0x0080
#endif
#ifndef OF_INDIRECT
#  define OF_INDIRECT 0x0100
#endif
#ifndef OF_FL3DIND
#  define OF_FL3DIND 0x0200
#endif
#ifndef OF_FL3DBAK
#  define OF_FL3DBAK 0x0400
#endif
#ifndef OF_FL3DACT
#  define OF_FL3DACT 0x0600
#endif
#ifndef OF_MOVEABLE
#  define OF_MOVEABLE 0x0800
#endif
#ifndef OF_POPUP
#  define OF_POPUP 0x1000
#endif

#ifndef R_CICONBLK
#  define R_CICONBLK 17
#endif
#ifndef R_CICON
#  define R_CICON 18
#endif

#ifndef G_SWBUTTON
#  define G_SWBUTTON 34
#endif
#ifndef G_POPUP
#  define G_POPUP 35
#endif
#ifndef G_EDIT
#  define G_EDIT 37
#endif
#ifndef G_SHORTCUT
#  define G_SHORTCUT 38
#endif
#ifndef G_SLIST
#  define G_SLIST 39
#endif
#ifndef G_EXTBOX
#  define G_EXTBOX 40
#endif
#ifndef G_OBLINK
#  define G_OBLINK 41
#endif

#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    ifdef __PUREC__
#      define _WORD int
#    else
#      define _WORD short
#    endif
#  endif
#endif

#ifndef _UBYTE
#  define _UBYTE char
#endif

#ifndef _BOOL
#  define _BOOL int
#endif

#ifndef _LONG
#  ifdef LONG
#    define _LONG LONG
#  else
#    define _LONG long
#  endif
#endif

#ifndef _ULONG
#  ifdef ULONG
#    define _ULONG ULONG
#  else
#    define _ULONG unsigned long
#  endif
#endif

#ifndef _LONG_PTR
#  define _LONG_PTR _LONG
#endif

#ifndef C_UNION
#ifdef __PORTAES_H__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __GEMLIB__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __PUREC__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __ALCYON__
#  define C_UNION(x) x
#endif
#endif
#ifndef C_UNION
#  define C_UNION(x) (_LONG_PTR)(x)
#endif

#ifndef SHORT
#  define SHORT short
#endif

#ifndef CP
#  define CP (SHORT *)
#endif

#ifndef CW
#  define CW (_WORD *)
#endif


#undef RSC_STATIC_FILE
#define RSC_STATIC_FILE 1

#include "wifi_da.h"

#ifndef RSC_NAMED_FUNCTIONS
#  define RSC_NAMED_FUNCTIONS 0
#endif

#ifndef __ALCYON__
#undef defRSHInit
#undef defRSHInitBit
#undef defRSHInitStr
#ifndef RsArraySize
#define RsArraySize(array) (_WORD)(sizeof(array)/sizeof(array[0]))
#define RsPtrArraySize(type, array) (type *)array, RsArraySize(array)
#endif
#define defRSHInit( aa, bb ) RSHInit( aa, bb, RsPtrArraySize(OBJECT *, rs_trindex), RsArraySize(rs_object) )
#define defRSHInitBit( aa, bb ) RSHInitBit( aa, bb, RsPtrArraySize(BITBLK *, rs_frimg) )
#define defRSHInitStr( aa, bb ) RSHInitStr( aa, bb, RsPtrArraySize(_UBYTE *, rs_frstr) )
#endif

#ifdef __STDC__
#ifndef W_Cicon_Setpalette
extern _BOOL W_Cicon_Setpalette(_WORD *_palette);
#endif
#ifndef hrelease_objs
extern void hrelease_objs(OBJECT *_ob, _WORD _num_objs);
#endif
#ifndef hfix_objs
extern void *hfix_objs(RSHDR *_hdr, OBJECT *_ob, _WORD _num_objs);
#endif
#endif

#ifndef RLOCAL
#  if RSC_STATIC_FILE
#    ifdef LOCAL
#      define RLOCAL LOCAL
#    else
#      define RLOCAL static
#    endif
#  else
#    define RLOCAL
#  endif
#endif


#ifndef N_
#  define N_(x)
#endif


#if RSC_STATIC_FILE
#undef NUM_STRINGS
#undef NUM_BB
#undef NUM_IB
#undef NUM_CIB
#undef NUM_CIC
#undef NUM_TI
#undef NUM_FRSTR
#undef NUM_FRIMG
#undef NUM_OBS
#undef NUM_TREE
#undef NUM_UD
#define NUM_STRINGS 31
#define NUM_BB		8
#define NUM_IB		1
#define NUM_CIB     0
#define NUM_CIC     0
#define NUM_TI		3
#define NUM_FRSTR	8
#define NUM_FRIMG	7
#define NUM_OBS     21
#define NUM_TREE	3
#define NUM_UD		0
#endif


static char wifi_da_string_0[] = "";
static char wifi_da_string_1[] = "Wi-Fi";
static char wifi_da_string_2[] = "v1.1";
static char wifi_da_string_3[] = "Desk Accessory for BlueSCSI";
static char wifi_da_string_4[] = "\275 2024, joshua stein";
static char wifi_da_string_5[] = "\275 2024, thorsten otto";
static char wifi_da_string_6[] = "I miss you, Carl";
static char wifi_da_string_7[] = "OK";
static char wifi_da_string_8[] = "Web: https://jcs.org";
static char wifi_da_string_9[] = "The Wi-Fi network";
static char wifi_da_string_10[] = "requires a password";
static char wifi_da_string_11[] = "Cancel";
static char wifi_da_string_12[] = "Connect";
static char wifi_da_string_13[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
static char wifi_da_string_14[] = "";
static char wifi_da_string_15[] = "";
static char wifi_da_string_16[] = "Password:";
static char wifi_da_string_17[] = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
static char wifi_da_string_18[] = "________________________________________________________________";
static char wifi_da_string_19[] = "X";
static char wifi_da_string_20[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxx";
static char wifi_da_string_21[] = "";
static char wifi_da_string_22[] = "";
static char wifi_da_string_23[] = "1.1 \275 2023-2024, joshua stein <jcs@jcs.org>";
static char wifi_da_string_24[] = "[2][%s][ Yes | No ]";
static char wifi_da_string_25[] = "[%c][%s][ OK ]";
static char wifi_da_string_26[] = "Wi-Fi";
static char wifi_da_string_27[] = "No SCSI Driver";
static char wifi_da_string_28[] = "No Device Found";
static char wifi_da_string_29[] = "(No Network)";
static char wifi_da_string_30[] = "Finding Wi-Fi...";


/* mask of ICON */
static _UBYTE wifi_da_IMAGE0[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF8, 0x00, 0x00, 0x7F, 0xFC, 0x00, 
0x01, 0xFF, 0xFF, 0x00, 0x03, 0xFF, 0xFF, 0x80, 0x07, 0xFF, 0xFF, 0xC0, 0x0F, 0xFF, 0xFF, 0xE0, 
0x1F, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0xC0, 0x03, 0xFF, 0xFF, 0x80, 
0x01, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x7F, 0xFC, 0x00, 0x00, 0x3F, 0xF8, 0x00, 
0x00, 0x1F, 0xF0, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x03, 0x80, 0x00, 
0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* data of ICON */
static _UBYTE wifi_da_IMAGE1[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x3F, 0xF8, 0x00, 0x00, 0x7F, 0xFC, 0x00, 0x01, 0xE0, 0x0F, 0x00, 0x03, 0x80, 0x03, 0x80, 
0x07, 0x0F, 0xE1, 0xC0, 0x00, 0x3F, 0xF8, 0x00, 0x00, 0x78, 0x3C, 0x00, 0x00, 0xE0, 0x0E, 0x00, 
0x00, 0xC7, 0xC6, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x1C, 0x70, 0x00, 0x00, 0x18, 0x30, 0x00, 
0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* data of WIN_SIGNAL */
static _UBYTE wifi_da_IMAGE2[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x1F, 0xF8, 0x30, 0x0C, 0x67, 0xE6, 0x4F, 0xF2, 
0x18, 0x18, 0x13, 0xC8, 0x07, 0xE0, 0x04, 0x20, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00};

/* data of SIGNAL_NO_DEVICE */
static _UBYTE wifi_da_IMAGE3[] = {
0x00, 0x00, 0x60, 0x06, 0x70, 0x0E, 0x3F, 0xFC, 0x1F, 0xF8, 0x3E, 0x7C, 0x67, 0xE6, 0x4F, 0xF2, 
0x1B, 0xD8, 0x17, 0xE8, 0x0F, 0xF0, 0x1C, 0x38, 0x39, 0x9C, 0x71, 0x8E, 0x60, 0x06, 0x00, 0x00};

/* data of SIGNAL_FINDING_DEVICE */
static _UBYTE wifi_da_IMAGE4[] = {
0x00, 0x00, 0x0F, 0xC0, 0x18, 0x60, 0x30, 0x10, 0x67, 0xC8, 0x48, 0x2C, 0x43, 0x8C, 0x44, 0x4C, 
0x41, 0x0C, 0x20, 0x18, 0x18, 0x78, 0x0F, 0xFC, 0x07, 0xFE, 0x00, 0x1F, 0x00, 0x0F, 0x00, 0x06};

/* data of SIGNAL_NONE */
static _UBYTE wifi_da_IMAGE5[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x50, 0x0A, 0xA8, 0x10, 0x04, 0x22, 0xA2, 0x45, 0x50, 
0x08, 0x08, 0x11, 0x40, 0x02, 0xA0, 0x04, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

/* data of SIGNAL_1 */
static _UBYTE wifi_da_IMAGE6[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x50, 0x0A, 0xA8, 0x10, 0x04, 0x22, 0xA2, 0x45, 0x50, 
0x08, 0x08, 0x11, 0x40, 0x02, 0xA0, 0x04, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00};

/* data of SIGNAL_2 */
static _UBYTE wifi_da_IMAGE7[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x50, 0x0A, 0xA8, 0x10, 0x04, 0x22, 0xA2, 0x45, 0x50, 
0x08, 0x08, 0x13, 0xC0, 0x07, 0xE0, 0x04, 0x20, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00};

/* data of SIGNAL_3 */
static _UBYTE wifi_da_IMAGE8[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x50, 0x0A, 0xA8, 0x10, 0x04, 0x27, 0xE2, 0x4F, 0xF0, 
0x18, 0x18, 0x13, 0xC8, 0x07, 0xE0, 0x04, 0x20, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00};

/* data of SIGNAL_4 */
static _UBYTE wifi_da_IMAGE9[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x1F, 0xF8, 0x30, 0x0C, 0x67, 0xE6, 0x4F, 0xF2, 
0x18, 0x18, 0x13, 0xC8, 0x07, 0xE0, 0x04, 0x20, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00};


static char *rs_frstr[NUM_FRSTR] = {
	wifi_da_string_23,
	wifi_da_string_24,
	wifi_da_string_25,
	wifi_da_string_26,
	wifi_da_string_27,
	wifi_da_string_28,
	wifi_da_string_29,
	wifi_da_string_30
};


static BITBLK rs_bitblk[NUM_BB] = {
	{ CP wifi_da_IMAGE2, 2, 16, 0, 0, 1 },
	{ CP wifi_da_IMAGE3, 2, 16, 0, 0, 1 },
	{ CP wifi_da_IMAGE4, 2, 16, 0, 0, 1 },
	{ CP wifi_da_IMAGE5, 2, 16, 0, 0, 1 },
	{ CP wifi_da_IMAGE6, 2, 16, 0, 0, 1 },
	{ CP wifi_da_IMAGE7, 2, 16, 0, 0, 1 },
	{ CP wifi_da_IMAGE8, 2, 16, 0, 0, 1 },
	{ CP wifi_da_IMAGE9, 2, 16, 0, 0, 1 }
};


static BITBLK *rs_frimg[NUM_FRIMG] = {
	&rs_bitblk[1],
	&rs_bitblk[2],
	&rs_bitblk[3],
	&rs_bitblk[4],
	&rs_bitblk[5],
	&rs_bitblk[6],
	&rs_bitblk[7]
};


static ICONBLK rs_iconblk[] = {
	{ CW wifi_da_IMAGE0, CW wifi_da_IMAGE1, wifi_da_string_0, 0x1000,21,5, 0,0,32,32, 0,24,0,8 }
};


static TEDINFO rs_tedinfo[NUM_TI] = {
	{ wifi_da_string_13, wifi_da_string_14, wifi_da_string_15, IBM, 0, TE_LEFT, 0x1100, 0x0, -1, 65,1 }, /* PASSWORD_DIALOG_SSID */
	{ wifi_da_string_17, wifi_da_string_18, wifi_da_string_19, IBM, 0, TE_LEFT, 0x1100, 0x0, -1, 65,65 }, /* PASSWORD_DIALOG_PASSWORD */
	{ wifi_da_string_20, wifi_da_string_21, wifi_da_string_22, IBM, 0, TE_CNTR, 0x1180, 0x0, -1, 29,1 } /* WIN_SCSI_ID */
};


static OBJECT rs_object[NUM_OBS] = {
/* ABOUT_DIALOG_ID */

	{ -1, 1, 9, G_BOX, OF_FL3DBAK, OS_OUTLINED, C_UNION(0x21100L), 0,0, 49,13 },
	{ 2, -1, -1, G_ICON, OF_NONE, OS_NORMAL, C_UNION(&rs_iconblk[0]), 2,1, 8192,8192 }, /* ICON */
	{ 3, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(wifi_da_string_1), 8,1, 6,2 },
	{ 4, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(wifi_da_string_2), 8,3, 4,1 }, /* VERSION */
	{ 5, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(wifi_da_string_3), 8,4, 27,1 },
	{ 6, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(wifi_da_string_4), 8,5, 27,1 },
	{ 7, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(wifi_da_string_5), 8,6, 27,1 },
	{ 8, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(wifi_da_string_6), 8,10, 27,1 },
	{ 9, -1, -1, G_BUTTON, 0x607, OS_NORMAL, C_UNION(wifi_da_string_7), 38,11, 8,1 }, /* ABOUT_OK */
	{ 0, -1, -1, G_STRING, OF_LASTOB, OS_NORMAL, C_UNION(wifi_da_string_8), 8,8, 27,1 },

/* PASSWORD_DIALOG_ID */

	{ -1, 1, 7, G_BOX, OF_FL3DBAK, OS_OUTLINED, C_UNION(0x21100L), 0,0, 68,10 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(wifi_da_string_9), 2,1, 17,1 },
	{ 3, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(wifi_da_string_10), 2,3, 19,1 },
	{ 4, -1, -1, G_BUTTON, 0x605, OS_NORMAL, C_UNION(wifi_da_string_11), 46,8, 8,1 }, /* PASSWORD_DIALOG_CANCEL */
	{ 5, -1, -1, G_BUTTON, 0x607, OS_NORMAL, C_UNION(wifi_da_string_12), 58,8, 8,1 }, /* PASSWORD_DIALOG_CONNECT */
	{ 6, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[0]), 2,2, 64,1 }, /* PASSWORD_DIALOG_SSID */
	{ 7, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(wifi_da_string_16), 2,5, 9,1 },
	{ 0, -1, -1, G_FTEXT, 0x28, OS_NORMAL, C_UNION(&rs_tedinfo[1]), 2,6, 64,1 }, /* PASSWORD_DIALOG_PASSWORD */

/* WIN_ID */

	{ -1, 1, 2, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 0,0, 34,2 },
	{ 2, -1, -1, G_IMAGE, OF_NONE, OS_NORMAL, C_UNION(&rs_bitblk[0]), 1,2048, 4096,4096 }, /* WIN_SIGNAL */
	{ 0, -1, -1, G_BOXTEXT, OF_LASTOB, OS_NORMAL, C_UNION(&rs_tedinfo[2]), 4,2048, 29,1 } /* WIN_SCSI_ID */
};


static OBJECT *rs_trindex[NUM_TREE] = {
	&rs_object[0], /* ABOUT_DIALOG_ID */
	&rs_object[10], /* PASSWORD_DIALOG_ID */
	&rs_object[18] /* WIN_ID */
};





#if RSC_STATIC_FILE

#if RSC_NAMED_FUNCTIONS
#ifdef __STDC__
_WORD wifi_da_rsc_load(_WORD wchar, _WORD hchar)
#else
_WORD wifi_da_rsc_load(wchar, hchar)
_WORD wchar;
_WORD wchar;
#endif
{
#ifndef RSC_HAS_PALETTE
#  define RSC_HAS_PALETTE 0
#endif
#ifndef RSC_USE_PALETTE
#  define RSC_USE_PALETTE 0
#endif
#if RSC_HAS_PALETTE || RSC_USE_PALETTE
	W_Cicon_Setpalette(&rgb_palette[0][0]);
#endif
#if NUM_OBS != 0
	{
		_WORD Obj;
		OBJECT *tree;
		for (Obj = 0, tree = rs_object; Obj < NUM_OBS; Obj++, tree++)
		{
			tree->ob_x = wchar * (tree->ob_x & 0xff) + (tree->ob_x >> 8);
			tree->ob_y = hchar * (tree->ob_y & 0xff) + (tree->ob_y >> 8);
			tree->ob_width = wchar * (tree->ob_width & 0xff) + (tree->ob_width >> 8);
			tree->ob_height = hchar * (tree->ob_height & 0xff) + (tree->ob_height >> 8);
		}
		hfix_objs(NULL, rs_object, NUM_OBS);
	}
#endif
	return 1;
}


#ifdef __STDC__
_WORD wifi_da_rsc_gaddr(_WORD type, _WORD idx, void *gaddr)
#else
_WORD wifi_da_rsc_gaddr(type, idx, gaddr)
_WORD type;
_WORD idx;
void *gaddr;
#endif
{
	switch (type)
	{
#if NUM_TREE != 0
	case R_TREE:
		if (idx < 0 || idx >= NUM_TREE)
			return 0;
		*((OBJECT **)gaddr) = rs_trindex[idx];
		break;
#endif
#if NUM_OBS != 0
	case R_OBJECT:
		if (idx < 0 || idx >= NUM_OBS)
			return 0;
		*((OBJECT **)gaddr) = &rs_object[idx];
		break;
#endif
#if NUM_TI != 0
	case R_TEDINFO:
		if (idx < 0 || idx >= NUM_TI)
			return 0;
		*((TEDINFO **)gaddr) = &rs_tedinfo[idx];
		break;
#endif
#if NUM_IB != 0
	case R_ICONBLK:
		if (idx < 0 || idx >= NUM_IB)
			return 0;
		*((ICONBLK **)gaddr) = &rs_iconblk[idx];
		break;
#endif
#if NUM_BB != 0
	case R_BITBLK:
		if (idx < 0 || idx >= NUM_BB)
			return 0;
		*((BITBLK **)gaddr) = &rs_bitblk[idx];
		break;
#endif
#if NUM_FRSTR != 0
	case R_STRING:
		if (idx < 0 || idx >= NUM_FRSTR)
			return 0;
		*((char **)gaddr) = (char *)(rs_frstr[idx]);
		break;
#endif
#if NUM_FRIMG != 0
	case R_IMAGEDATA:
		if (idx < 0 || idx >= NUM_FRIMG)
			return 0;
		*((BITBLK **)gaddr) = rs_frimg[idx];
		break;
#endif
#if NUM_OBS != 0
	case R_OBSPEC:
		if (idx < 0 || idx >= NUM_OBS)
			return 0;
		*((_LONG_PTR **)gaddr) = &rs_object[idx].ob_spec.index;
		break;
#endif
#if NUM_TI != 0
	case R_TEPTEXT:
		if (idx < 0 || idx >= NUM_TI)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_tedinfo[idx].te_ptext);
		break;
#endif
#if NUM_TI != 0
	case R_TEPTMPLT:
		if (idx < 0 || idx >= NUM_TI)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_tedinfo[idx].te_ptmplt);
		break;
#endif
#if NUM_TI != 0
	case R_TEPVALID:
		if (idx < 0 || idx >= NUM_TI)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_tedinfo[idx].te_pvalid);
		break;
#endif
#if NUM_IB != 0
	case R_IBPMASK:
		if (idx < 0 || idx >= NUM_IB)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_iconblk[idx].ib_pmask);
		break;
#endif
#if NUM_IB != 0
	case R_IBPDATA:
		if (idx < 0 || idx >= NUM_IB)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_iconblk[idx].ib_pdata);
		break;
#endif
#if NUM_IB != 0
	case R_IBPTEXT:
		if (idx < 0 || idx >= NUM_IB)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_iconblk[idx].ib_ptext);
		break;
#endif
#if NUM_BB != 0
	case R_BIPDATA:
		if (idx < 0 || idx >= NUM_BB)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_bitblk[idx].bi_pdata);
		break;
#endif
#if NUM_FRSTR != 0
	case R_FRSTR:
		if (idx < 0 || idx >= NUM_FRSTR)
			return 0;
		*((char ***)gaddr) = (char **)(&rs_frstr[idx]);
		break;
#endif
#if NUM_FRIMG != 0
	case R_FRIMG:
		if (idx < 0 || idx >= NUM_FRIMG)
			return 0;
		*((BITBLK ***)gaddr) = &rs_frimg[idx];
		break;
#endif
	default:
		return 0;
	}
	return 1;
}


#ifdef __STDC__
_WORD wifi_da_rsc_free(void)
#else
_WORD wifi_da_rsc_free()
#endif
{
#if NUM_OBS != 0
	hrelease_objs(rs_object, NUM_OBS);
#endif
	return 1;
}

#endif /* RSC_NAMED_FUNCTIONS */

#else /* !RSC_STATIC_FILE */
#if 0
_WORD rs_numstrings = 31;
_WORD rs_numfrstr = 8;

_WORD rs_nuser = 0;
_WORD rs_numimages = 10;
_WORD rs_numbb = 8;
_WORD rs_numfrimg = 7;
_WORD rs_numib = 1;
_WORD rs_numcib = 0;
_WORD rs_numti = 3;
_WORD rs_numobs = 21;
_WORD rs_numtree = 3;

char rs_name[] = "wifi_da.rsc";

_WORD _rsc_format = 2; /* RSC_FORM_SOURCE2 */
#endif
#endif /* RSC_STATIC_FILE */
