#ifdef __PUREC__
#include <tos.h>
#include <aes.h>
#include <vdi.h>
#else
#include <mint/mintbind.h>
#include <gem.h>
#endif

#if defined(__PUREC__) && defined(__STDIO)
#define OLD_PUREC
#endif
#ifndef OLD_PUREC
#include <mint/arch/nf_ops.h>
#endif

#ifndef _WORD
# ifdef __PUREC__
#  define _WORD int
# else
#  define _WORD short
# endif
#endif

#ifndef OS_NORMAL
#define OS_NORMAL       NORMAL
#define OS_SELECTED     SELECTED
#define OS_CROSSED      CROSSED
#define OS_CHECKED      CHECKED
#define OS_DISABLED     DISABLED
#define OS_OUTLINED     OUTLINED
#define OS_SHADOWED     SHADOWED
#define OS_WHITEBAK     0x40            /* TOS         */
#define OS_DRAW3D       DRAW3D            /* GEM 2.x     */
#define OF_NONE		 	NONE
#define OF_SELECTABLE	SELECTABLE
#define OF_DEFAULT		DEFAULT
#define OF_EXIT			EXIT
#define OF_EDITABLE		EDITABLE
#define OF_RBUTTON		RBUTTON
#define OF_LASTOB		LASTOB
#define OF_TOUCHEXIT	TOUCHEXIT
#define OF_HIDETREE		HIDETREE
#define OF_INDIRECT		INDIRECT
#define OF_FL3DMASK     0x0600
#define OF_FL3DNONE     0x0000
#define OF_FL3DIND      0x0200
#define OF_FL3DBAK      0x0400
#define OF_FL3DACT      0x0600
#define OF_SUBMENU      0x0800
#endif

#if !defined(__GNUC__) && !defined(__attribute__)
#define __attribute__(x)
#endif

#ifndef UNUSED
# define UNUSED(x) (void)(x)
#endif

#ifndef __CDECL
#define __CDECL cdecl
#endif

#if defined(__PUREC__) && !defined(__PORTAES_H__)
#define wind_get_grect(wh, what, gr) wind_get(wh, what, &(gr)->g_x, &(gr)->g_y, &(gr)->g_w, &(gr)->g_h)
#define wind_set_grect(wh, what, gr) wind_set(wh, what, (gr)->g_x, (gr)->g_y, (gr)->g_w, (gr)->g_h)
#define wind_set_int(wh, what, val) wind_set(wh, what, val, 0, 0, 0)
#define wind_get_int(wh, what, val) wind_get(wh, what, val)
#define wind_calc_grect(kind, what, in, out) wind_calc(kind, what, (in)->g_x, (in)->g_y, (in)->g_w, (in)->g_h, &(out)->g_x, &(out)->g_y, &(out)->g_w, &(out)->g_h)
#define form_center_grect(tree, out) form_center(tree, &(out)->g_x, &(out)->g_y, &(out)->g_w, &(out)->g_h)
#define form_dial_grect(flag, gr1, gr2) form_dial(flag, (gr1)->g_x, (gr1)->g_y, (gr1)->g_w, (gr1)->g_h, (gr2)->g_x, (gr2)->g_y, (gr2)->g_w, (gr2)->g_h)
#define wind_create_grect(kind, gr) wind_create(kind, (gr)->g_x, (gr)->g_y, (gr)->g_w, (gr)->g_h)
#define wind_open_grect(wh, gr) wind_open(wh, (gr)->g_x, (gr)->g_y, (gr)->g_w, (gr)->g_h)
#define objc_draw_grect(tree, obj, d, gr) objc_draw(tree, obj, d, (gr)->g_x, (gr)->g_y, (gr)->g_w, (gr)->g_h)
#define objc_change_grect(tree, obj, d, gr, s, r) objc_change(tree, obj, d, (gr)->g_x, (gr)->g_y, (gr)->g_w, (gr)->g_h, s, r)
#define wind_set_str(wh, what, s) wind_set(wh, what, s, NULL)
#define _AESversion _GemParBlk.global[0]
#define _AESnumapps _GemParBlk.global[1]
#define gl_apid _GemParBlk.global[2]
#define EVNT_TIME(lo) 0, lo

struct POPUP_INIT_args
{
	OBJECT *tree;
	_WORD scrollpos;
	_WORD nlines;
	void *param;
};

_WORD rc_intersect(const GRECT *src, GRECT *dst);
_WORD form_popup(OBJECT *tree, _WORD x, _WORD y);
_WORD xfrm_popup(
                OBJECT *tree, _WORD x, _WORD y,
                _WORD firstscrlob, _WORD lastscrlob,
                _WORD nlines,
                void __CDECL (*init)(struct POPUP_INIT_args),
                void *param, _WORD *lastscrlpos );     /* MagiC 5.03 */

#else
#define EVNT_TIME(lo) lo
#endif

#ifdef __GEMLIB__
extern short _app;
#endif

#ifndef WM_BOTTOMED
#define WM_BOTTOMED		33
#endif
#ifndef WM_M_BDROPPED
#define WM_M_BDROPPED	100
#endif

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC CLK_TCK
#endif

#ifndef G_SHORTCUT
#define G_SHORTCUT		38						/* MagiC 6 */
#endif
