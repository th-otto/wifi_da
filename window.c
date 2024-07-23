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

#include <stdio.h>
#include <string.h>
#include "wi-fi.h"
#include "popup.h"

static int nwifi_scan_networks;
static struct wifi_network_entry wifi_menu_networks[nitems(wifi_scan_networks)] = { 0 };

static char *ssid_menu[nitems(wifi_scan_networks) + 1];

static int nwifi_menu_networks;


void create_window(void)
{
	GRECT bounds;
	GRECT work;
	GRECT screen;
	OBJECT *tree;
	_WORD kind = NAME | CLOSER | MOVER;

	tree = rs_tree(WIN_ID);

	wind_get_grect(0, WF_WORKXYWH, &screen);
	main_win = wind_create_grect(kind, &screen);
	if (main_win <= 0)
	{
		DEBUG_LOG(("window creation failed\n"));
		return;
	}
	
	/* upper right corner, but leave space for desktop menu bar */
	bounds.g_x = screen.g_x + screen.g_w - tree[ROOT].ob_width - 70;
	bounds.g_y = screen.g_y + 20;
	bounds.g_w = tree[ROOT].ob_width;
	bounds.g_h = tree[ROOT].ob_height;
	wind_calc_grect(WC_WORK, kind, &bounds, &work);
	work.g_w = tree[ROOT].ob_width;
	work.g_h = tree[ROOT].ob_height;
	wind_calc_grect(WC_BORDER, kind, &work, &bounds);
	wind_set_str(main_win, WF_NAME, rs_str(WIN_TITLE));
	
	obj_set_ptext(tree, WIN_SCSI_ID, rs_str(STR_FINDING));
	tree[WIN_SIGNAL].ob_spec.bitblk = rs_image(SIGNAL_NO_DEVICE);

	if (wind_open_grect(main_win, &bounds) != 0)
	{
		DEBUG_LOG(("window open failed\n"));
	}
	
	if (wifi_scsi_id == WIFI_SCSI_ID_FINDING)
	{
		wifi_scsi_id = scsi_find_wifi();
	}

	if (wifi_scsi_id == WIFI_SCSI_ID_NONE)
	{
		da_state = STATE_IDLE;
		DEBUG_LOG(("no device found\n"));
		obj_set_ptext(tree, WIN_SCSI_ID, rs_str(scsidrv_get_version() == 0 ? NO_SCSI_DRVR : NO_DEVICE_TEXT));
		update_window(NULL);
	} else
	{
		DEBUG_LOG(("wifi id: %d\n", wifi_scsi_id));
		scsi_wifi_info(wifi_scsi_id, &wifi_cur_info);
		da_state = STATE_SCANNING;
		scsi_wifi_scan(wifi_scsi_id);
	}
}


static void redraw_window(_WORD win, OBJECT *tree, const GRECT *clip)
{
	GRECT work;
	GRECT gr;

	wind_get_grect(win, WF_WORKXYWH, &work);
	if (clip == NULL)
	{
		clip = &work;
	}
	tree[ROOT].ob_x = work.g_x;
	tree[ROOT].ob_y = work.g_y;
	wind_update(BEG_UPDATE);
	wind_get_grect(win, WF_FIRSTXYWH, &gr);
	while (gr.g_w > 0 && gr.g_h > 0)
	{
		if (rc_intersect(clip, &gr))
			objc_draw_grect(tree, ROOT, MAX_DEPTH, &gr);
		wind_get_grect(win, WF_NEXTXYWH, &gr);
	}
	wind_update(END_UPDATE);
}


void update_window(const GRECT *clip)
{
	OBJECT *tree = rs_tree(WIN_ID);
	_WORD icon;
	
	DEBUG_LOG(("redrawing window\n"));

	if (wifi_scsi_id == WIFI_SCSI_ID_NONE)
		icon = SIGNAL_NO_DEVICE;
	else if (wifi_scsi_id == WIFI_SCSI_ID_FINDING)
		icon = SIGNAL_FINDING_DEVICE;
	else if (wifi_cur_info.ssid[0] == '\0')
		icon = SIGNAL_NONE;
	else if (wifi_cur_info.rssi >= -60)
		icon = SIGNAL_4;
	else if (wifi_cur_info.rssi >= -70)
		icon = SIGNAL_3;
	else if (wifi_cur_info.rssi >= -75)
		icon = SIGNAL_2;
	else
		icon = SIGNAL_1;

	tree[WIN_SIGNAL].ob_spec.bitblk = rs_image(icon);

	redraw_window(main_win, tree, clip);
}


void update_wifi_cur_info(void)
{
	struct wifi_network_entry old_info;

	if (wifi_scsi_id == WIFI_SCSI_ID_NONE)
		return;

	old_info = wifi_cur_info;

	scsi_wifi_info(wifi_scsi_id, &wifi_cur_info);

	if (memcmp(&old_info, &wifi_cur_info, sizeof(old_info)) != 0)
	{
		if (memcmp(&old_info.ssid, &wifi_cur_info.ssid, sizeof(old_info.ssid)) == 0)
		{
			/* just updating RSSI */
			DEBUG_LOG(("update_wifi_cur_info: just updating rssi to %d\n", wifi_cur_info.rssi));
			update_window(NULL);
		} else
		{
			DEBUG_LOG(("update_wifi_cur_info: updating ssid list\n"));
			update_wifi_ssid_list(false);
		}
	}
}


void update_wifi_ssid_list(bool update_networks)
{
	OBJECT *tree = rs_tree(WIN_ID);
	int n, m, mitem;

	if (wifi_scsi_id == WIFI_SCSI_ID_NONE)
		return;

	if (update_networks)
	{
		nwifi_scan_networks = scsi_wifi_scan_results(wifi_scsi_id, wifi_scan_networks, (int)nitems(wifi_scan_networks));
		DEBUG_LOG(("scsi_wifi_scan_results: %d\n", nwifi_scan_networks));

		if (nwifi_scan_networks == 0)
		{
			wifi_scan_networks[0] = wifi_cur_info;
			nwifi_scan_networks = 1;
		}
	}

	/* put the current network first, if any */
	wifi_menu_networks[0] = wifi_cur_info;

	nwifi_menu_networks = nwifi_scan_networks;
	if (nwifi_menu_networks > (int)nitems(wifi_menu_networks))
	{
		nwifi_menu_networks = (int)nitems(wifi_menu_networks);
		DEBUG_LOG(("capping nets to %d\n", nwifi_menu_networks));
	}

	/* add each additional network from the scan, excluding current */
	for (n = 0, m = 0; n < nwifi_menu_networks; n++)
	{
		if (strcmp(wifi_scan_networks[n].ssid, wifi_cur_info.ssid) == 0)
		{
			DEBUG_LOG(("skipping %s, is current\n", wifi_scan_networks[n].ssid));
			continue;
		}

		DEBUG_LOG(("net %d: %s\n", m, wifi_scan_networks[n].ssid));

		wifi_menu_networks[m++] = wifi_scan_networks[n];
	}
	
	if (m == 0)
		m = 1;
	nwifi_menu_networks = m;

	if (wifi_menu_networks[0].ssid[0] == '\0')
		strlcpy(wifi_menu_networks[0].ssid, rs_str(NO_NETWORK_TEXT), sizeof(wifi_menu_networks[0].ssid));

	for (n = 0, mitem = 0; n < nwifi_menu_networks; n++)
	{
		ssid_menu[mitem] = wifi_menu_networks[n].ssid;
		mitem++;
	}

	obj_set_ptext(tree, WIN_SCSI_ID, wifi_menu_networks[0].ssid);

	update_window(NULL);
}


void destroy_window(void)
{
	if (main_win > 0)
	{
		wind_close(main_win);
		wind_delete(main_win);
		main_win = -1;
	}
}

void window_mousedown(_WORD mox, _WORD moy)
{
	OBJECT *tree = rs_tree(WIN_ID);
	_WORD obj;
	GRECT pos;
	int n;
	int selitem;
	struct wifi_network_entry *net;
	struct wifi_join_request wjr;

	if (wifi_scsi_id == WIFI_SCSI_ID_NONE)
		return;

	obj = objc_find(tree, ROOT, MAX_DEPTH, mox, moy);
	if (obj != WIN_SCSI_ID)
		return;

	selitem = 0;
	for (n = 0; n < nwifi_menu_networks; n++)
	{
		if (strcmp(wifi_menu_networks[n].ssid, wifi_cur_info.ssid) == 0)
		{
			selitem = n;
			break;
		}
	}

	objc_offset(tree, WIN_SCSI_ID, &pos.g_x, &pos.g_y);
	pos.g_w = tree[WIN_SCSI_ID].ob_width;
	pos.g_h = tree[WIN_SCSI_ID].ob_height;
	selitem = do_popup(&pos, ssid_menu, nwifi_menu_networks, 2, selitem);
	if (selitem < 0)
		return;

	net = &wifi_menu_networks[selitem];

	memset(&wjr, 0, sizeof(wjr));
	strlcpy(wjr.ssid, net->ssid, sizeof(wjr.ssid));

	if (net->flags & WIFI_NETWORK_FLAG_AUTH)
	{
#if 0
		char password[64];

		obj_set_ptext(tree, PASSWORD_DIALOG_SSID, net->ssid);

		dg = GetNewDialog(OwnedResourceID(PASSWORD_DIALOG_ID), 0L, (WindowPtr) - 1L);
		center_in_screen(tree, &r);
		MoveWindow(dg, r.left, r.top, false);

		ShowWindow(dg);
		SetPort(dg);

		GetDItem(dg, PASSWORD_DIALOG_PASSWORD_STORAGE_ID, &itype, &ihandle, &irect);
		SetIText(ihandle, "\p");

		/* outline ok button */
		GetDItem(dg, ok, &itype, &ihandle, &irect);
		PenSize(3, 3);
		InsetRect(&irect, -4, -4);
		FrameRoundRect(&irect, 16, 16);
		PenNormal();

		for (;;)
		{
			ModalDialog(WiFiPasswordDialogFieldFilter, &hit);
			if (hit == ok || hit == cancel)
				break;
		}

		if (hit != ok)
		{
			DisposDialog(dg);
			goto menu_done;
		}

		GetDItem(dg, PASSWORD_DIALOG_PASSWORD_STORAGE_ID, &itype, &ihandle, &irect);
		GetIText(ihandle, &password);
		PtoCstr(password);
#if 0
		DEBUG_LOG(("pass is \"%s\"\n", password));
#endif
		strlcpy(wjr.key, password, sizeof(wjr.key));

		DisposDialog(dg);
#endif
	}

	scsi_wifi_join(wifi_scsi_id, &wjr);

	/* force an update of the list */
	scsi_wifi_info(wifi_scsi_id, &wifi_cur_info);
	update_wifi_ssid_list(false);
}

#if 0
pascal Boolean WiFiPasswordDialogFieldFilter(DialogPtr dlg, EventRecord *event, int *hit)
{
	Str255 password;
	DialogPeek dlgp;
	ControlHandle ihandle;
	Rect irect;
	int sel_start,
	 sel_end,
	 itype;
	char key;

	dlgp = (DialogPeek) dlg;

	switch (event->what)
	{
	case keyDown:
	case autoKey:
		sel_start = (*(dlgp->textH))->selStart;
		sel_end = (*(dlgp->textH))->selEnd;

		key = event->message & charCodeMask;
		if (event->modifiers & cmdKey)
		{
			/* TODO: implement DlgPaste for cmd+v? */
			event->what = nullEvent;
			return FALSE;
		}

		GetDItem(dlg, PASSWORD_DIALOG_PASSWORD_STORAGE_ID, &itype, &ihandle, &irect);
		GetIText(ihandle, &password);
		PtoCstr(password);

		if (key == 8)
		{
			/* backspace */
			if (sel_start == sel_end && sel_start > 0)
				memmove(password + sel_start - 1, password + sel_start, sizeof(password) - sel_start - 1);
			else if (sel_start != sel_end)
				memmove(password + sel_start, password + sel_end, sizeof(password) - sel_end - 1);
		} else if (sel_start >= sizeof(password))
		{
			event->what = nullEvent;
			return FALSE;
		} else if (key >= ' ' && key <= '~')
		{
			if (sel_start != sel_end)
				/* delete selection before making space for new char */
				memmove(password + sel_start, password + sel_end, sizeof(password) - sel_end - 1);
			memmove(password + sel_start + 1, password + sel_start, sizeof(password) - sel_start - 1);
			password[sel_start] = key;
			event->message = '¥';
		}
		password[(*(dlgp->textH))->teLength + 1] = '\0';
		CtoPstr(password);

		SetIText(ihandle, password);
		sel_start = 0;
		break;
	}

	return ModalDialogFilter(dlg, event, hit);
}
#endif
