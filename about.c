/*
 * Copyright (c) 2024 Thorsten Otto
 * based on code
 * Copyright (c) 2023 joshua stein <jcs@jcs.org>
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

#include "wi-fi.h"
#include "util.h"

void wifi_about(void)
{
	OBJECT *tree;
	GRECT bounds;
	
	tree = rs_tree(ABOUT_DIALOG_ID);
	tree[ABOUT_OK].ob_state &= ~OS_SELECTED;

	center_in_screen(tree, &bounds);
	wind_update(BEG_UPDATE);
	wind_update(BEG_MCTRL);
	form_dial_grect(FMD_START, &bounds, &bounds);
	objc_draw_grect(tree, ROOT, MAX_DEPTH, &bounds);
	form_do(tree, 0);
	form_dial_grect(FMD_FINISH, &bounds, &bounds);
	wind_update(END_MCTRL);
	wind_update(END_UPDATE);
}
