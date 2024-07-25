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

#include <string.h>
#include "wi-fi.h"
#include "popup.h"

static _WORD ed_win;
static _WORD ed_field;
static _WORD ed_idx;

static int nwifi_scan_networks;
static struct wifi_network_entry wifi_menu_networks[MAX_NETWORKS];

static char *ssid_menu[MAX_NETWORKS + 1];

static int nwifi_menu_networks;


#define BACKSPACE 0x0E08				/* backspace        */
#define SPACE 0x3920					/* ASCII <space>    */
#define UP 0x4800						/* up arrow     */
#define DOWN 0x5000						/* down arrow       */
#define LEFT 0x4B00						/* left arrow       */
#define RIGHT 0x4D00					/* right arrow      */
#define DELETE 0x537f					/* keypad delete    */
#define TAB 0x0F09						/* tab          */
#define BACKTAB 0x0F00					/* backtab      */
#define RETURN 0x1C0D					/* carriage return  */
#define ENTER 0x720D					/* enter key on keypad  */
#define ESCAPE 0x011B					/* escape       */

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
	bounds.g_x = screen.g_x + screen.g_w - tree[ROOT].ob_width - 20;
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

	if (wind_open_grect(main_win, &bounds) == 0)
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


void redraw_window(_WORD win, OBJECT *tree, const GRECT *clip)
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
		if (win == ed_win && ed_field > 0)
			objc_edit(tree, ed_field, 0, &ed_idx, ED_END);
		if (rc_intersect(clip, &gr))
			objc_draw_grect(tree, ROOT, MAX_DEPTH, &gr);
		if (win == ed_win && ed_field > 0)
			objc_edit(tree, ed_field, 0, &ed_idx, ED_INIT);
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
		nwifi_scan_networks = scsi_wifi_scan_results(wifi_scsi_id, wifi_scan_networks, MAX_NETWORKS);
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
	if (nwifi_menu_networks > MAX_NETWORKS)
	{
		nwifi_menu_networks = MAX_NETWORKS;
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


void destroy_pw_window(void)
{
	if (password_win > 0)
	{
		wind_close(password_win);
		wind_delete(password_win);
		password_win = -1;
	}
}


void destroy_main_window(void)
{
	if (main_win > 0)
	{
		wind_close(main_win);
		wind_delete(main_win);
		main_win = -1;
	}
}


void destroy_windows(void)
{
	destroy_pw_window();
	destroy_main_window();
}


static _WORD obj_deselect(OBJECT *tree, _WORD obj)
{
	_WORD dummy;
	_WORD inside;
	
	inside = graf_watchbox(tree, obj, tree[obj].ob_state ^ OS_SELECTED, tree[obj].ob_state);
	evnt_button(1, 1, 0, &dummy, &dummy, &dummy, &dummy);
	return inside;
}


static bool pw_dialog(struct wifi_join_request *wjr)
{
	OBJECT *tree = rs_tree(PASSWORD_DIALOG_ID);
	bool ok = false;
	GRECT bounds;
	GRECT work;
	GRECT screen;
	_WORD kind = NAME | CLOSER | MOVER;
	char password_buf[sizeof(wjr->key)];
	_WORD events;
	_WORD maxlen;
	_WORD mox, moy;
	_WORD key, kstate;
	_WORD message[8];
	_WORD dummy;

	wind_get_grect(0, WF_WORKXYWH, &screen);
	password_win = wind_create_grect(kind, &screen);
	if (password_win <= 0)
	{
		DEBUG_LOG(("window creation failed\n"));
		return false;
	}

	tree[ROOT].ob_spec.obspec.framesize = 0;
	tree[ROOT].ob_state &= ~OS_OUTLINED;
	form_center_grect(tree, &work);
	wind_calc_grect(WC_BORDER, kind, &work, &bounds);
	wind_set_str(password_win, WF_NAME, rs_str(PASSWORD_TITLE));
	
	obj_set_ptext(tree, PASSWORD_DIALOG_SSID, wjr->ssid);
	memset(password_buf, 0, sizeof(password_buf));
	maxlen = tree[PASSWORD_DIALOG_PASSWORD].ob_spec.tedinfo->te_txtlen;
	if (maxlen > (int)sizeof(password_buf))
		maxlen = (int)sizeof(password_buf);
	memset(tree[PASSWORD_DIALOG_PASSWORD].ob_spec.tedinfo->te_ptext, '\0', maxlen);
	tree[PASSWORD_DIALOG_CANCEL].ob_state &= ~OS_SELECTED;
	tree[PASSWORD_DIALOG_CONNECT].ob_state &= ~OS_SELECTED;
	
	ed_idx = 0;
	ed_field = PASSWORD_DIALOG_PASSWORD;
	ed_win = password_win;
	
	if (wind_open_grect(password_win, &bounds) == 0)
	{
		DEBUG_LOG(("window open failed\n"));
	}
	
	for (;;)
	{
		events = MU_MESAG | MU_BUTTON | MU_KEYBD;

		events = evnt_multi(events,
			1, 1, 1,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			message,
			EVNT_TIME(0),
			&mox, &moy, &dummy, &kstate, &key, &dummy);

		if (events & MU_MESAG)
		{
			handle_message(message);
		}

		if (password_win < 0)
			break;
		
		if (events & MU_KEYBD)
		{
			switch (key)
			{
			case RETURN:
			case ENTER:
				objc_change_grect(tree, PASSWORD_DIALOG_CONNECT, MAX_DEPTH, &work, tree[PASSWORD_DIALOG_CONNECT].ob_state | OS_SELECTED, true);
				ok = true;
				break;
			case BACKSPACE:
				if (ed_idx > 0)
				{
					memmove(password_buf + ed_idx - 1, password_buf + ed_idx, maxlen - ed_idx);
					password_buf[maxlen - 1] = '\0';
					objc_edit(tree, ed_field, key, &ed_idx, ED_CHAR);
				}
				break;
			case DELETE:
				memmove(password_buf + ed_idx, password_buf + ed_idx + 1, maxlen - ed_idx - 1);
				password_buf[maxlen - 1] = '\0';
				objc_edit(tree, ed_field, key, &ed_idx, ED_CHAR);
				break;
			case ESCAPE:
				memset(password_buf, 0, maxlen);
				objc_edit(tree, ed_field, key, &ed_idx, ED_CHAR);
				break;
			case LEFT:
			case RIGHT:
				objc_edit(tree, ed_field, key, &ed_idx, ED_CHAR);
				break;
			default:
				key &= 0xff;
				if (key >= ' ' && key <= '~' && ed_idx < maxlen - 1)
				{
					memmove(password_buf + ed_idx + 1, password_buf + ed_idx, maxlen - ed_idx - 1);
					password_buf[maxlen - 1] = '\0';
					password_buf[ed_idx] = key;
					objc_edit(tree, ed_field, '*', &ed_idx, ED_CHAR);
				}
				break;
			}
		}

		if (events & MU_BUTTON)
		{
			_WORD obj = objc_find(tree, ROOT, MAX_DEPTH, mox, moy);
			
			switch (obj)
			{
			case PASSWORD_DIALOG_CANCEL:
				if (obj_deselect(tree, obj))
					destroy_pw_window();
				break;
			case PASSWORD_DIALOG_CONNECT:
				if (obj_deselect(tree, obj))
				{
					ok = true;
					destroy_pw_window();
				}
				break;
			}
		}

		if (ok || password_win < 0)
			break;
	}
	
	destroy_pw_window();
	ed_win = -1;
	
	if (ok)
		strcpy(wjr->key, password_buf);
	return ok;
}


#define XXX 0

void window_mousedown(_WORD mox, _WORD moy)
{
	OBJECT *tree = rs_tree(WIN_ID);
	_WORD obj;
	GRECT pos;
	GRECT work;
	int n;
	int selitem;
	struct wifi_network_entry *net;
	struct wifi_join_request wjr;

	wind_get_grect(main_win, WF_WORKXYWH, &work);
	tree[ROOT].ob_x = work.g_x;
	tree[ROOT].ob_y = work.g_y;

#if XXX
	strcpy(wifi_menu_networks[0].ssid, "Hello123451234567890123456789012345678901234567890");
	strcpy(wifi_menu_networks[1].ssid, "World");
	strcpy(wifi_menu_networks[2].ssid, "Here we go");
	nwifi_menu_networks = 3;
	ssid_menu[0] = wifi_menu_networks[0].ssid;
	ssid_menu[1] = wifi_menu_networks[1].ssid;
	ssid_menu[2] = wifi_menu_networks[2].ssid;
	ssid_menu[3] = wifi_menu_networks[3].ssid;
#endif
	
#if !XXX
	if (wifi_scsi_id == WIFI_SCSI_ID_NONE)
		return;
#endif

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

#if XXX
	net->flags |= WIFI_NETWORK_FLAG_AUTH;
#endif

	if (net->flags & WIFI_NETWORK_FLAG_AUTH)
	{
		if (!pw_dialog(&wjr))
			return;
#if 0
		DEBUG_LOG(("pass is \"%s\"\n", wjr.key));
#endif
	}

	scsi_wifi_join(wifi_scsi_id, &wjr);

	/* force an update of the list */
	scsi_wifi_info(wifi_scsi_id, &wifi_cur_info);
	update_wifi_ssid_list(false);
}
