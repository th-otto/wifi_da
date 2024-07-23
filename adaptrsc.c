/*
 * COPS (c) 1995 - 2003 Sven & Wilfried Behne
 *                 2004 F.Naumann & O.Skancke
 *
 * A XCONTROL compatible CPX manager.
 *
 * This file is part of COPS.
 *
 * COPS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * COPS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XaAES; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string.h>
#include <stdio.h>
#include <mint/cookie.h>

#include "wi-fi.h"

#include "adaptrsc.h"


unsigned long aes_flags;


/*----------------------------------------------------------------------------------------*/
/* Informationen ueber die AES-Funktionen zurueckliefern                                  */
/* Funktionsergebnis:   diverse Flags                                                     */
/*  font_id:        ID des AES-Fonts                                                      */
/*  font_height:    Hoehe des AES-Fonts (fuer vst_height())                               */
/*  hor_3d:         zusaetzlicher horizontaler beidseitiger Rand fuer 3D-Objekte          */
/*  ver_3d:         zusaetzlicher vertikaler beidseitiger Rand fuer 3D-Objekte            */
/*----------------------------------------------------------------------------------------*/
unsigned long get_aes_info(void)
{
	unsigned long magic;
	unsigned long flags;
	
	flags = 0;

	if (appl_find("?AGI") == 0)	/* appl_getinfo() vorhanden? */
		flags |= GAI_INFO;

	if (_AESversion >= 0x0401)	/* mindestens AES 4.01? */
		flags |= GAI_INFO;

	if (getCookie(C_MagX, &magic) && magic)			/* MagiC vorhanden? */
	{
		flags |= GAI_MAGIC | GAI_INFO;
	}

	if (flags & GAI_INFO)				/* ist appl_getinfo() vorhanden? */
	{
		_WORD ag1;
		_WORD ag2;
		_WORD ag3;
		_WORD ag4;

		if (appl_getinfo(2, &ag1, &ag2, &ag3, &ag4) && ag3)	/* Unterfunktion 2, Farben */
			flags |= GAI_CICN;

		if (appl_getinfo(7, &ag1, &ag2, &ag3, &ag4))		/* Unterfunktion 7 */
			flags |= ag1 & (GAI_WDLG|GAI_LBOX|GAI_FNTS|GAI_FSEL);

		if (appl_getinfo(AES_MOUSE, &ag1, &ag2, &ag3, &ag4) && (ag1 & 1))		/* graf_mouse(M_SAVE/M_RESTORE) supported? */
			flags |= GAI_MOUSE;

		if (appl_getinfo(9, &ag1, &ag2, &ag3, &ag4))		/* form_popup() supported? */
		{
			if (ag2 & 1)
			{
				flags |= GAI_POPUP;
				if (ag3 & 1)
				{
					/*
					 * XaAES has scrollable menus, but not scrollable popups :(
					 */
					/* FIXME: find a better way to detect whether xfrm_popup is supported */
					if (flags & GAI_MAGIC)
						flags |= GAI_SCROLLPOPUP;
				}
			}
		}

		if (appl_getinfo(12, &ag1, &ag2, &ag3, &ag4) && (ag1 & 8)) /* AP_TERM? */
			flags |= GAI_APTERM;

		if (appl_getinfo(14, &ag1, &ag2, &ag3, &ag4) && (ag1 & 1)) /* Flydials? */
			flags |= GAI_FLYDIAL;

		if (appl_getinfo(98, &ag1, &ag2, &ag3, &ag4))
		{
			if (ag1 & 0x01)
				flags |= GAI_OBJC_WEDIT;
		}
	}

	aes_flags = flags;
	return flags;
}
