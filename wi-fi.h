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

#ifndef __WIFI_H__
#define __WIFI_H__

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#ifdef __PUREC__
#include <tos.h>
#include <aes.h>
#include <vdi.h>
#else
#include <mint/mintbind.h>
#include <gem.h>
#endif

#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif

#include "util.h"
#include "wifi_da.h"

/* disable in production */
#define DEBUG_LOGGING		0

#if DEBUG_LOGGING
#include <mint/arch/nf_ops.h>
#endif

enum state
{
	STATE_CLOSED,
	STATE_INIT,
	STATE_FINDING,
	STATE_SCANNING,
	STATE_IDLE
};
extern int da_state;

struct wifi_network_entry
{
	char ssid[64];
	unsigned char bssid[6];
	char rssi;
	unsigned char channel;
	unsigned char flags;
#define WIFI_NETWORK_FLAG_AUTH 0x1
	char _padding;
};

struct wifi_join_request
{
	char ssid[64];
	char key[64];
	unsigned char channel;
	unsigned char _padding;
};

extern short _app;
extern _WORD main_win;
extern _WORD gl_wchar, gl_hchar;

extern int wifi_scsi_id;
enum
{
	WIFI_SCSI_ID_FINDING = -1,
	WIFI_SCSI_ID_NONE = -2
};
extern clock_t wifi_scan_started;
extern struct wifi_network_entry wifi_cur_info;
extern struct wifi_network_entry wifi_scan_networks[10];

void wifi_about(void);

#if DEBUG_LOGGING
#define DEBUG_LOG(args) nf_debugprintf args
#else
#define DEBUG_LOG(args)
#endif

void create_window(void);
void update_window(const GRECT *clip);
void update_wifi_cur_info(void);
void update_wifi_ssid_list(bool update_networks);
void destroy_window(void);
void window_mousedown(_WORD mox, _WORD moy);

bool scsidrv_init(bool reinit);
void scsidrv_exit(void);
unsigned short scsidrv_get_version(void);
int scsi_find_wifi(void);
bool scsi_wifi_scan(int lun);
bool scsi_wifi_scan_finished(int id);
void scsi_wifi_scan_cancel(int id);
bool scsi_wifi_info(int lun, struct wifi_network_entry *resp);
int scsi_wifi_scan_results(int scsi_id, struct wifi_network_entry *resp, int count);
bool scsi_wifi_join(int scsi_id, struct wifi_join_request *wjr);

#endif
