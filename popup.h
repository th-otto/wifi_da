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

#ifndef _popup_h
#define _popup_h

_WORD do_popup(GRECT *button_rect, char **strs, _WORD no_strs, _WORD no_spaces, _WORD slct);
_WORD do_form_popup(OBJECT *tree,
	_WORD x, _WORD y,
	_WORD firstscrlob, _WORD lastscrlob, _WORD nlines,
	void *param,
	_WORD *offset);

#endif /* _popup_h */
