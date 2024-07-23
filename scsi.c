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
#include <stdio.h>
#include <mint/cookie.h>
#include "wi-fi.h"
#include "scsidrv/scsi.h"
#include "scsidrv/scsi3.h"

#define BLUESCSI_NETWORK_WIFI_CMD				0x1c
#define BLUESCSI_NETWORK_WIFI_CMD_SCAN			0x01
#define BLUESCSI_NETWORK_WIFI_CMD_COMPLETE		0x02
#define BLUESCSI_NETWORK_WIFI_CMD_SCAN_RESULTS	0x03
#define BLUESCSI_NETWORK_WIFI_CMD_INFO			0x04
#define BLUESCSI_NETWORK_WIFI_CMD_JOIN			0x05

static unsigned char scsi_data[2048];

typedef struct deviceInfo
{
	unsigned short busNo;
	unsigned short id;
	unsigned long maxLen;
	char busName[20];
	char deviceBusName[20];
} DEVICEINFO;

#define ILL_HANDLE ((tHandle)-1)


#define MAX_DEVICES 32
static DEVICEINFO deviceInfos[MAX_DEVICES];
static int devCount;
static tScsiCall *cookie_scsi;
static int HasVirtual;
static void *VirtBuffer;
static SENSE_DATA senseData;
static tSCSICmd scan_cmd;
static tHandle scsi_handle = ILL_HANDLE;


/*--- Defines ---*/

#ifndef SuperToUser
#define SuperToUser(stack) Super(stack)
#endif

#define SCSIDRV_BEGIN(stack) \
	stack=NULL; \
	if (Super((void *)1)==0) { \
		stack=(void *)Super(NULL); \
	}

#define SCSIDRV_END(stack) \
	if (stack!=NULL) { \
		SuperToUser(stack); \
	}

/*--- SCSIDRV function calls ---*/

#if defined(__GNUC__) && !defined(__MSHORT__)

/* necessary because gcc only saves d0-d1/a0-a1 on function calls, */
/* and scsidrv functions can modify d0-d2/a0-a1 */

#define SCSIDRV_CLOBBER_LIST	"d1", "d2", "a0", "a1", "memory", "cc"

#define SCSIDRVCALL_l_w(func_ptr,p1)	\
__extension__	\
({	\
	register long retvalue __asm__("d0");	\
	\
	__asm__ volatile (	\
		"movw	%2,-(%%sp)\n\t"	\
		"jbsr	%1@\n\t"	\
		"addql	#2,%%sp"	\
		: "=r"(retvalue)	\
		: "a"(func_ptr), "g"(p1)	\
		: SCSIDRV_CLOBBER_LIST	\
	);	\
	retvalue;	\
})

#define SCSIDRVCALL_l_l(func_ptr,p1)	\
__extension__	\
({	\
	register long retvalue __asm__("d0");	\
	\
	__asm__ volatile (	\
		"movl	%2,-(%%sp)\n\t"	\
		"jbsr	%1@\n\t"	\
		"addql	#4,%%sp"	\
		: "=r"(retvalue)	\
		: "a"(func_ptr), "g"(p1)	\
		: SCSIDRV_CLOBBER_LIST	\
	);	\
	retvalue;	\
})

#define SCSIDRVCALL_l_ww(func_ptr,p1,p2)	\
__extension__	\
({	\
	register long retvalue __asm__("d0");	\
	\
	__asm__ volatile (	\
		"movw	%3,-(%%sp)\n\t"	\
		"movw	%2,-(%%sp)\n\t"	\
		"jbsr	%1@\n\t"	\
		"addql	#4,%%sp"	\
		: "=r"(retvalue)	\
		: "a"(func_ptr), "g"(p1), "g"(p2)	\
		: SCSIDRV_CLOBBER_LIST	\
	);	\
	retvalue;	\
})

#define SCSIDRVCALL_l_wl(func_ptr,p1,p2)	\
__extension__	\
({	\
	register long retvalue __asm__("d0");	\
	\
	__asm__ volatile (	\
		"movl	%3,-(%%sp)\n\t"	\
		"movw	%2,-(%%sp)\n\t"	\
		"jbsr	%1@\n\t"	\
		"addql	#6,%%sp"	\
		: "=r"(retvalue)	\
		: "a"(func_ptr), "g"(p1), "g"(p2)	\
		: SCSIDRV_CLOBBER_LIST	\
	);	\
	retvalue;	\
})

#define SCSIDRVCALL_l_wwl(func_ptr,p1,p2,p3)	\
__extension__	\
({	\
	register long retvalue __asm__("d0");	\
	\
	__asm__ volatile (	\
		"movl	%4,-(%%sp)\n\t"	\
		"movw	%3,-(%%sp)\n\t"	\
		"movw	%2,-(%%sp)\n\t"	\
		"jbsr	%1@\n\t"	\
		"addql	#8,%%sp"	\
		: "=r"(retvalue)	\
		: "a"(func_ptr), "g"(p1), "g"(p2), "g"(p3)	\
		: SCSIDRV_CLOBBER_LIST	\
	);	\
	retvalue;	\
})

#define SCSIDRVCALL_l_wll(func_ptr,p1,p2,p3)	\
__extension__	\
({	\
	register long retvalue __asm__("d0");	\
	\
	__asm__ volatile (	\
		"movl	%4,-(%%sp)\n\t"	\
		"movl	%3,-(%%sp)\n\t"	\
		"movw	%2,-(%%sp)\n\t"	\
		"jbsr	%1@\n\t"	\
		"lea	10(%%sp),%%sp"	\
		: "=r"(retvalue)	\
		: "a"(func_ptr), "g"(p1), "g"(p2), "g"(p3)	\
		: SCSIDRV_CLOBBER_LIST	\
	);	\
	retvalue;	\
})

#define SCSIDRVCALL_l_wlll(func_ptr,p1,p2,p3,p4)	\
__extension__	\
({	\
	register long retvalue __asm__("d0");	\
	\
	__asm__ volatile (	\
		"movl	%5,-(%%sp)\n\t"	\
		"movl	%4,-(%%sp)\n\t"	\
		"movl	%3,-(%%sp)\n\t"	\
		"movw	%2,-(%%sp)\n\t"	\
		"jbsr	%1@\n\t"	\
		"lea	14(%%sp),%%sp"	\
		: "=r"(retvalue)	\
		: "a"(func_ptr), "g"(p1), "g"(p2), "g"(p3), "g"(p4)	\
		: SCSIDRV_CLOBBER_LIST	\
	);	\
	retvalue;	\
})

#define SCSIDRVCALL_l_lww(func_ptr,p1,p2,p3)	\
__extension__	\
({	\
	register long retvalue __asm__("d0");	\
	\
	__asm__ volatile (	\
		"movw	%4,-(%%sp)\n\t"	\
		"movw	%3,-(%%sp)\n\t"	\
		"movl	%2,-(%%sp)\n\t"	\
		"jbsr	%1@\n\t"	\
		"addql	#8,%%sp"	\
		: "=r"(retvalue)	\
		: "a"(func_ptr), "g"(p1), "g"(p2), "g"(p3)	\
		: SCSIDRV_CLOBBER_LIST	\
	);	\
	retvalue;	\
})

#else

#define SCSIDRVCALL_l_w(func_ptr,p1) (*func_ptr)(p1)
#define SCSIDRVCALL_l_l(func_ptr,p1) (*func_ptr)(p1)
#define SCSIDRVCALL_l_ww(func_ptr,p1,p2) (*func_ptr)(p1, p2)
#define SCSIDRVCALL_l_wl(func_ptr,p1,p2) (*func_ptr)(p1, p2)
#define SCSIDRVCALL_l_wwl(func_ptr,p1,p2,p3) (*func_ptr)(p1, p2, p3)
#define SCSIDRVCALL_l_wll(func_ptr,p1,p2,p3) (*func_ptr)(p1, p2, p3)
#define SCSIDRVCALL_l_wlll(func_ptr,p1,p2,p3,p4) (*func_ptr)(p1, p2, p3, p4)
#define SCSIDRVCALL_l_lww(func_ptr,p1,p2,p3) (*func_ptr)(p1, p2, p3)

#endif


bool getCookie(long cookie, unsigned long *p_value)
{
	long *cookiejar = (long *) Setexc(0x5a0 / 4, (void (*)(void))-1);

	if (!cookiejar)
		return false;

	for (;;)
	{
		if (cookiejar[0] == cookie)
		{
			if (p_value)
				*p_value = cookiejar[1];
			return true;
		}
		if (cookiejar[0] == 0)
			break;
		cookiejar += 2;
	}

	return false;
}


static long scsidrv_in(tpSCSICmd params)
{
	void *oldstack;
	long result;
	unsigned long phystop;
	void *oldbuf;

	SCSIDRV_BEGIN(oldstack);
	phystop = *((unsigned long *)0x42e);
	if (HasVirtual && ((unsigned long)params->Buffer > phystop))
	{
		oldbuf = params->Buffer;
		params->Buffer = VirtBuffer; 
		result = SCSIDRVCALL_l_l(cookie_scsi->In, params);
		memcpy(oldbuf, params->Buffer, params->TransferLen);
		params->Buffer = oldbuf;
	} else
	{
		result = SCSIDRVCALL_l_l(cookie_scsi->In, params);
	}

	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_out(tpSCSICmd params)
{
	void *oldstack;
	long result;
	unsigned long phystop;
	void *oldbuf;

	SCSIDRV_BEGIN(oldstack);
	phystop = *((unsigned long *)0x42e);
	if (HasVirtual && ((unsigned long)params->Buffer > phystop))
	{
		oldbuf = params->Buffer;
		memcpy(VirtBuffer, oldbuf, params->TransferLen);
		params->Buffer = VirtBuffer;
		result = SCSIDRVCALL_l_l(cookie_scsi->Out, params);
		params->Buffer = oldbuf;
	} else
	{
		result = SCSIDRVCALL_l_l(cookie_scsi->Out, params);
	}
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_inquire_scsi(short what, tBusInfo *info)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wl(cookie_scsi->InquireSCSI, what, info);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_inquire_bus(short what, short busno, tDevInfo *device)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wwl(cookie_scsi->InquireBus, what, busno, device);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_check_dev(short busno, const DLONG *scsi_id, char *name, unsigned short *features)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wlll(cookie_scsi->CheckDev, busno, scsi_id, name, features);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_open(short busno, const DLONG *scsi_id, unsigned long *maxlen)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wll(cookie_scsi->Open, busno, scsi_id, maxlen);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_close(tHandle handle)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_l(cookie_scsi->Close, handle);
	SCSIDRV_END(oldstack);
	return result;
}


#if 0 /* not used here */
static long scsidrv_rescan_bus(short busno)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_w(cookie_scsi->RescanBus, busno);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_error(short *handle, short rwflag, short errno)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_lww(cookie_scsi->Error, handle, rwflag, errno);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_install(short bus, tpTargetHandler handler)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wl(cookie_scsi->Install, bus, handler);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_deinstall(short bus, tpTargetHandler handler)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wl(cookie_scsi->Deinstall, bus, handler);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_get_cmd(short bus, unsigned char *cmd)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wl(cookie_scsi->GetCmd, bus, cmd);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_send_data(short bus, unsigned char *buffer, unsigned long length)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wll(cookie_scsi->SendData, bus, buffer, length);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_get_data(short bus, void *buffer, unsigned long length)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wll(cookie_scsi->GetData, bus, buffer, length);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_send_status(short bus, unsigned short status)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wl(cookie_scsi->SendStatus, bus, status);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_send_msg(short bus, unsigned short msg)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_ww(cookie_scsi->SendMsg, bus, msg);
	SCSIDRV_END(oldstack);
	return result;
}


static long scsidrv_get_msg(short bus, unsigned short *msg)
{
	void *oldstack;
	long result;

	SCSIDRV_BEGIN(oldstack);
	result = SCSIDRVCALL_l_wl(cookie_scsi->GetMsg, bus, msg);
	SCSIDRV_END(oldstack);
	return result;
}
#endif


static int findDevices(void)
{
	tBusInfo busInfo;
	long busResult;

	devCount = 0;
	busResult = scsidrv_inquire_scsi(cInqFirst, &busInfo);
	while (busResult == 0)
	{
		if (busInfo.BusNo == 1 || busInfo.BusNo == 31)
		{
			tDevInfo devInfo;
			long result;
	
			result = scsidrv_inquire_bus(cInqFirst, busInfo.BusNo, &devInfo);
			while (result == 0)
			{
				char deviceBusName[20];
				unsigned short features;
	
				scsidrv_check_dev(busInfo.BusNo, &devInfo.SCSIId, deviceBusName, &features);
				
				if (devCount < MAX_DEVICES)
				{
					deviceInfos[devCount].busNo = busInfo.BusNo;
					deviceInfos[devCount].id = devInfo.SCSIId.lo;
					deviceInfos[devCount].maxLen = busInfo.MaxLen;
					strcpy(deviceInfos[devCount].busName, busInfo.BusName);
					strcpy(deviceInfos[devCount].deviceBusName, deviceBusName);
					devCount++;
				}
	
				result = scsidrv_inquire_bus(cInqNext, busInfo.BusNo, &devInfo);
			}

			DEBUG_LOG(("  ID: %d, Name: '%s', Maximum transfer length: %lu ($%lX)\n",
				busInfo.BusNo, busInfo.BusName, busInfo.MaxLen, busInfo.MaxLen));
		}
		busResult = scsidrv_inquire_scsi(cInqNext, &busInfo);
	}

	return devCount;
}


static bool scsi_wifi_info_internal(tHandle handle)
{
	tCmd6 cdb;
	tSCSICmd cmd;
	unsigned short size;

	cdb.Command = BLUESCSI_NETWORK_WIFI_CMD;
	cdb.LunAdr = BLUESCSI_NETWORK_WIFI_CMD_INFO;
	size = sizeof(struct wifi_network_entry) + 2;
	cdb.Adr = (size >> 8) & 0xff;
	cdb.Len = size & 0xff;
	cdb.Flags = 0;

	cmd.Handle = handle;
	cmd.Cmd = (char *)&cdb;
	cmd.CmdLen = sizeof(cdb);
	cmd.Buffer = scsi_data;
	cmd.TransferLen = size;
	cmd.SenseBuffer = (void *) &senseData;
	cmd.Timeout = 5 * 200;
	cmd.Flags = 0;

	return scsidrv_in(&cmd) >= 0;
}


int scsi_find_wifi(void)
{
	INQUIRY_DATA inquiryData;
	long status;
	tCmd6 cdb;
	int i;
	int id;
	unsigned long maxLen;
	tSCSICmd cmd;
	long handle;
	DLONG scsiId;

	if (cookie_scsi == 0)
		return WIFI_SCSI_ID_NONE;

	for (id = 0; devCount; id++)
	{
		scsiId.hi = 0;
		scsiId.lo = deviceInfos[id].id;
				
		handle = scsidrv_open(deviceInfos[id].busNo, &scsiId, &maxLen);
	
		if (handle < 0)
		{
			DEBUG_LOG(("    ERROR: No handle\n"));
		} else
		{
			for (i = 0; i <= 1; i++)
			{
				cdb.Command = INQUIRY; 				/* inquiry */
				cdb.LunAdr = 0;
				cdb.Adr = 0;
				cdb.Len = 5;						/* length */
				cdb.Flags = 0;
	
				cmd.Handle = (tHandle)handle;
				cmd.Cmd = (char *)&cdb;
				cmd.CmdLen = sizeof(cdb);
				cmd.Buffer = &inquiryData;
				cmd.TransferLen = 5;
				cmd.SenseBuffer = (void *) &senseData;
				cmd.Timeout = 5 * 200;
				cmd.Flags = 0;

				status = scsidrv_in(&cmd);
				if (status < 0)
				{
					DEBUG_LOG(("scsi[%d]: inquiry failed\n", id));
					break;
				}
				if (i == 0)
				{
					/* re-inquire with full length */
					cdb.Len += inquiryData.additionalLength;
					cmd.TransferLen += inquiryData.additionalLength;
					continue;
				}
	
				inquiryData.vendor[sizeof(inquiryData.vendor) - 1] = '\0';
				inquiryData.product[sizeof(inquiryData.product) - 1] = '\0';
				DEBUG_LOG(("scsi[%d]: v \"%s\", p \"%s\"", id, inquiryData.vendor, inquiryData.product));
	
				if (memcmp(inquiryData.vendor, "Dayna", 5) != 0 || memcmp(inquiryData.product, "SCSI/Link", 9) != 0)
					break;
	
				if (scsi_wifi_info_internal(cmd.Handle))
				{
					scsi_handle = cmd.Handle;
					return id;
				}

				DEBUG_LOG(("scsi[%d]: matched vendor/product but no info", id));
			}
			scsidrv_close(cmd.Handle);
		}
	}

	return WIFI_SCSI_ID_NONE;
}


#if 0 /* not used here */
static void SetCmd6(tCmd6 *cdb, int LogicalUnit, unsigned char Cmd, unsigned long BlockAdr, unsigned short TransferLen)
{
	cdb->Command = Cmd;
	cdb->LunAdr = LogicalUnit + (BlockAdr >> 16) & 0x1F;
	cdb->Adr = BlockAdr;
	cdb->Len = TransferLen;
	cdb->Flags = 0;
}
#endif



bool scsi_wifi_scan(int id)
{
	tCmd6 cdb;
	unsigned char data;

	if (!cookie_scsi)
		return false;

	if (scan_cmd.Handle != ILL_HANDLE)
	{
		DEBUG_LOG(("scsi_wifi_scan: scan already in progress\n"));
		return false;
	}
	
	/* this will happen in the timer handler, when no device was found */
	if (id < 0 || scsi_handle == ILL_HANDLE)
		return false;

	memset(wifi_scan_networks, 0, sizeof(wifi_scan_networks));

	cdb.Command = BLUESCSI_NETWORK_WIFI_CMD;
	cdb.LunAdr = BLUESCSI_NETWORK_WIFI_CMD_SCAN;
	cdb.Adr = 0;
	cdb.Len = 1;
	cdb.Flags = 0;

	scan_cmd.Handle = scsi_handle;
	scan_cmd.Cmd = (char *)&cdb;
	scan_cmd.CmdLen = sizeof(cdb);
	scan_cmd.Buffer = &data;
	scan_cmd.TransferLen = 1;
	scan_cmd.SenseBuffer = (void *) &senseData;
	scan_cmd.Timeout = 5 * 200;
	scan_cmd.Flags = 0;

	wifi_scan_started = clock();

	if (scsidrv_in(&scan_cmd) < 0)
	{
		warn("wifi_scan failed");
	}
	return true;
}


void scsi_wifi_scan_cancel(int id)
{
	if (!cookie_scsi)
		return;
	if (scan_cmd.Handle == ILL_HANDLE)
	{
		DEBUG_LOG(("scsi_wifi_scan_cancel: no scan in progress\n"));
		return;
	}
	scan_cmd.Handle = ILL_HANDLE;
	if (id < 0)
		return;
}


bool scsi_wifi_scan_finished(int id)
{
	tCmd6 cdb;
	unsigned char data;

	if (!cookie_scsi)
		return false;
	/* this will happen in the timer handler, when no device was found */
	if (id < 0)
		return true;

	if (scan_cmd.Handle == ILL_HANDLE)
	{
		DEBUG_LOG(("scsi_wifi_scan_finished: no scan in progress\n"));
		return true;
	}
	
	cdb.Command = BLUESCSI_NETWORK_WIFI_CMD;
	cdb.LunAdr = BLUESCSI_NETWORK_WIFI_CMD_COMPLETE;
	cdb.Adr = 0;
	cdb.Len = 1;
	cdb.Flags = 0;

	scan_cmd.Cmd = (char *)&cdb;
	scan_cmd.CmdLen = sizeof(cdb);
	scan_cmd.Buffer = &data;
	scan_cmd.TransferLen = 1;
	scan_cmd.SenseBuffer = (void *) &senseData;
	scan_cmd.Timeout = 5 * 200;
	scan_cmd.Flags = 0;

	if (scsidrv_in(&scan_cmd) < 0)
	{
		warn("wifi_scan_finished failed");
		return true;
	}

	if (data == 1)
		return true;

	return false;
}


int scsi_wifi_scan_results(int id, struct wifi_network_entry *resp, int count)
{
	tCmd6 cdb;
	size_t size;
	int net_count;

	if (!cookie_scsi)
		return false;
	/* this will happen in the timer handler, when no device was found */
	if (id < 0)
		return true;

	if (scan_cmd.Handle == ILL_HANDLE)
	{
		DEBUG_LOG(("scsi_wifi_results: no scan in progress\n"));
		return true;
	}
	
	cdb.Command = BLUESCSI_NETWORK_WIFI_CMD;
	cdb.LunAdr = BLUESCSI_NETWORK_WIFI_CMD_SCAN_RESULTS;
	cdb.Adr = (sizeof(scsi_data) >> 8) & 0xff;
	cdb.Len = sizeof(scsi_data) & 0xff;
	cdb.Flags = 0;

	scan_cmd.Cmd = (char *)&cdb;
	scan_cmd.CmdLen = sizeof(cdb);
	scan_cmd.Buffer = scsi_data;
	scan_cmd.TransferLen = sizeof(scsi_data);
	scan_cmd.SenseBuffer = (void *) &senseData;
	scan_cmd.Timeout = 5 * 200;
	scan_cmd.Flags = 0;

	if (scsidrv_in(&scan_cmd) < 0)
	{
		warn("scsi_wifi_scan_results failed");
		net_count = 0;
	} else
	{
		size = (scsi_data[0] << 8) | scsi_data[1];
		net_count = (int)(size / sizeof(struct wifi_network_entry));
		if (net_count > count)
			net_count = count;
	
		memcpy(resp, scsi_data + 2, sizeof(struct wifi_network_entry) * count);
	}

	scan_cmd.Handle = ILL_HANDLE;

	return net_count;
}


bool scsi_wifi_info(int id, struct wifi_network_entry *resp)
{
	bool result;
	
	if (!cookie_scsi)
		return false;
	if (id < 0 || scsi_handle == ILL_HANDLE)
		return false;

	result = scsi_wifi_info_internal(scsi_handle);
	if (result)
	{
		/* skip returned size */
		memcpy(resp, scsi_data + 2, sizeof(struct wifi_network_entry));
	}

	return result;
}


bool scsi_wifi_join(int id, struct wifi_join_request *wjr)
{
	tCmd6 cdb;
	unsigned short size;
	tSCSICmd cmd;
	long status;
	
	if (!cookie_scsi)
		return false;
	if (id < 0 || scsi_handle == ILL_HANDLE)
		return false;

	cdb.Command = BLUESCSI_NETWORK_WIFI_CMD;
	cdb.LunAdr = BLUESCSI_NETWORK_WIFI_CMD_JOIN;
	size = sizeof(*wjr);
	cdb.Adr = (size >> 8) & 0xff;
	cdb.Len = size & 0xff;
	cdb.Flags = 0;

	cmd.Handle = scsi_handle;
	cmd.Cmd = (char *)&cdb;
	cmd.CmdLen = sizeof(cdb);
	cmd.Buffer = wjr;
	cmd.TransferLen = size;
	cmd.SenseBuffer = (void *) &senseData;
	cmd.Timeout = 5 * 200;
	cmd.Flags = 0;

	status = scsidrv_out(&cmd);
	if (status < 0)
	{
		warn("scsi_wifi_join failed");
	}
	return status >= 0;
}


unsigned short scsidrv_get_version(void)
{
	if (cookie_scsi)
		return cookie_scsi->Version;
	return 0;
}


bool scsidrv_init(bool reinit)
{
	unsigned long pmmu;
	
	scan_cmd.Handle = ILL_HANDLE;
	HasVirtual = false;
	if (!getCookie(C_SCSI, (unsigned long *)&cookie_scsi))
	{
		DEBUG_LOG(("No SCSI driver found\n"));
		return false;
	}
	if (getCookie(C_PMMU, &pmmu) && pmmu != 0)
		HasVirtual = true;

	if (VirtBuffer == NULL && HasVirtual)
	{
		VirtBuffer = (void *)Mxalloc(64L * 1024L, 0);
		if ((long)VirtBuffer <= 0)
		{
			DEBUG_LOG(("Allocating VirtBuffer failed\n"));
			VirtBuffer = 0;
			HasVirtual = false;
		}
	}
	if (reinit || devCount == 0)
	{
		if (scsi_handle != ILL_HANDLE)
		{
			scsidrv_close(scsi_handle);
			scsi_handle = ILL_HANDLE;
		}
		findDevices();
	}
	return true;
}


void scsidrv_exit(void)
{
	devCount = 0;
	if (scsi_handle != ILL_HANDLE)
	{
		scsidrv_close(scsi_handle);
		scsi_handle = ILL_HANDLE;
	}
	scan_cmd.Handle = ILL_HANDLE;
	if (VirtBuffer != NULL)
	{
		Mfree(VirtBuffer);
		VirtBuffer = NULL;
	}
}
