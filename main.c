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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "wi-fi.h"
#include "adaptrsc.h"

#include "wifi_da.rsh"

int da_state = STATE_CLOSED;
int wifi_scsi_id = WIFI_SCSI_ID_FINDING;
clock_t wifi_scan_started;
struct wifi_network_entry wifi_cur_info;
struct wifi_network_entry wifi_scan_networks[10];

_WORD main_win;
_WORD password_win;
_WORD aes_handle;
_WORD gl_wchar, gl_hchar;

/*
 * This is called each time the DA is selected from the AES menu,
 * even if we're already open.
 */
static void da_open(void)
{
	if (main_win > 0)
	{
		wind_set_int(main_win, WF_TOP, 0);
	} else
	{
		if (da_state == STATE_CLOSED)
			da_state = STATE_INIT;
		create_window();
	}
}


static void da_close(void)
{
	/* window handle is already gone when we get here */
	main_win = -1;
	password_win = -1;
	da_state = STATE_CLOSED;
}


void handle_message(_WORD *message)
{
	switch (message[0])
	{
	case WM_REDRAW:
		if (main_win > 0 && main_win == message[3])
			update_window((const GRECT *)&message[4]);
		if (password_win > 0 && password_win == message[3])
			redraw_window(password_win, rs_tree(PASSWORD_DIALOG_ID), (const GRECT *)&message[4]);
		break;

	case AC_CLOSE:
		da_close();
		break;

	case AC_OPEN:
		da_open();
		break;
	
	case WM_CLOSED:
		if (password_win == message[3])
			destroy_pw_window();
		if (main_win == message[3])
			destroy_windows();
		break;

	case AP_TERM:
		destroy_windows();
		break;

	case WM_MOVED:
	case WM_SIZED:
		if (main_win > 0 && main_win == message[3])
			wind_set_grect(main_win, WF_CURRXYWH, (const GRECT *)&message[4]);
		if (password_win > 0 && password_win == message[3])
			wind_set_grect(password_win, WF_CURRXYWH, (const GRECT *)&message[4]);
		break;

	case WM_TOPPED:
	case WM_NEWTOP:
		if (main_win > 0 && main_win == message[3])
		{
			if (password_win > 0)
				wind_set_int(password_win, WF_TOP, 0);
			else
				wind_set_int(main_win, WF_TOP, 0);
		}
		if (password_win > 0 && password_win == message[3])
			wind_set_int(password_win, WF_TOP, 0);
		break;

	case WM_BOTTOMED:
	case WM_M_BDROPPED:
		if (main_win > 0 && main_win == message[3])
			wind_set_int(main_win, WF_BOTTOM, 0);
		/* ignored on password window */
		break;

	/* just to avoid that the sender has to wait for timeout */	
	case AP_DRAGDROP:
		dd_nak(message);
		break;
	}
}


static void handle_key(_WORD key, _WORD kstate)
{
	_WORD top;
	
	(void)kstate;
	wind_get_int(0, WF_TOP, &top);
	if (top == main_win)
	{
		switch (key & 0xff00)
		{
		case 0x6200: /* Help */
			wifi_about();
			key = 0;
			break;
		}
		switch (key & 0xff)
		{
		case 0x1b:
		case 0x11:
			destroy_main_window();
			break;
		}
	}
}


static void handle_timer(void)
{
	static clock_t last_update = 0;
	static clock_t last_scan = 0;
	clock_t Ticks;

	if (main_win <= 0)
		return;

	Ticks = clock();

	switch (da_state)
	{
	case STATE_SCANNING:
		/* check every second */
		if ((Ticks - last_update) >= ((clock_t)CLOCKS_PER_SEC * 1))
		{
			DEBUG_LOG(("%ld - handle_timer - checking for scan completion\n", (long)Ticks));
			last_update = Ticks;
			if (!scsi_wifi_scan_finished(wifi_scsi_id))
			{
				DEBUG_LOG(("%ld - handle_timer - scan not done\n", (long)Ticks));
				if (Ticks - wifi_scan_started <= ((clock_t)CLOCKS_PER_SEC * 5))
					break;
				DEBUG_LOG(("Wi-Fi scan failed to finish, canceling\n"));
				scsi_wifi_scan_cancel(wifi_scsi_id);
			}
			graf_mouse(ARROW, NULL);
			last_scan = Ticks;
			da_state = STATE_IDLE;
			DEBUG_LOG(("%ld - handle_timer - scan done, updating list\n", (long)Ticks));
			update_wifi_ssid_list(true);
		}
		break;

	case STATE_IDLE:
		/* refresh SSID/RSSI every 5 seconds, scan every 30 */
		if ((Ticks - last_update) >= ((clock_t)CLOCKS_PER_SEC * 5))
		{
			last_update = Ticks;
	
			if ((Ticks - last_scan) > ((clock_t)CLOCKS_PER_SEC * 30))
			{
				da_state = STATE_SCANNING;
				graf_mouse(BUSYBEE, NULL);
				DEBUG_LOG(("%ld - handle_timer - idle, scanning\n", (long)Ticks));
				scsi_wifi_scan(wifi_scsi_id);
			} else
			{
				DEBUG_LOG(("%ld - handle_timer - idle, updating info\n", (long)Ticks));
				update_wifi_cur_info();
			}
		}
		break;
	}
}

const char *rs_str(_WORD id)
{
	return rs_frstr[id];
}


OBJECT *rs_tree(_WORD id)
{
	return rs_trindex[id];
}


BITBLK *rs_image(_WORD id)
{
	return rs_frimg[id];
}



int main(void)
{
	_WORD events;
	_WORD mox, moy;
	_WORD key, kstate;
	_WORD message[8];
	_WORD i;
	bool done;
	_WORD dummy;

	if (appl_init() < 0)
		return 1;
	aes_handle = graf_handle(&gl_wchar, &gl_hchar, &dummy, &dummy);
	for (i = 0; i < NUM_OBS; i++)
		rsrc_obfix(rs_object, i);

	aes_flags = get_aes_info();
	util_init();
	scsidrv_init(false);

	if (multi())
		shel_write(9, 1, 0, "", "");

	done = false;
	if (_app)
	{
		da_open();
		if (main_win <= 0)
			done = true;
	}

	graf_mouse(ARROW, NULL);

	while (!done)
	{
		events = MU_MESAG;
		if (main_win > 0)
			events |= MU_TIMER | MU_BUTTON | MU_KEYBD;
		
		events = evnt_multi(events,
			2, 1, 1,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			message,
			1000,
			&mox, &moy, &dummy, &kstate, &key, &dummy);
		
		if (events & MU_MESAG)
		{
			handle_message(message);
		}

		if (events & MU_BUTTON)
		{
			window_mousedown(mox, moy);
		}
		
		if (events & MU_KEYBD)
		{
			handle_key(key, kstate);
		}
		
		if (main_win < 0 && (_app || multi()))
			break;

		if (events & MU_TIMER)
			handle_timer();
	}
	
	destroy_windows();
	scsidrv_exit();
	appl_exit();
	
	return 0;
}
