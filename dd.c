#include "wi-fi.h"
#include <string.h>

/*
 * We need the MiNT definitions here,
 * not the ones that are defined by PureC.
 * To be sure, undef the definitions that might come from the
 * original header.
 * Remember that the argument to a handler installed with
 * Psignal() is *long* (unlike with signal(), where it is int)
 */

#undef SIG_DFL
#undef SIG_IGN
#undef SIG_ERR
#undef SIG_SYS

#undef SIGPIPE
#define SIGPIPE		13		/* broken pipe */

#define       SIG_DFL	((__mint_sighandler_t)0L)
#define       SIG_IGN	((__mint_sighandler_t)1L)
#define       SIG_ERR	((__mint_sighandler_t)-1L)
#define       SIG_SYS	((__mint_sighandler_t)-2L)


#define PIPE_DRAGDROP "U:\\PIPE\\DRAGDROP.AA"
#define PIPE_EXT (sizeof(PIPE_DRAGDROP) - 3)

#ifndef __mint_sighandler_t_defined
#define __mint_sighandler_t_defined 1
typedef void __CDECL (*__mint_sighandler_t) (long signum);
#endif

static __mint_sighandler_t oldsig;

/* AP_DRAGDROP return codes */
#define DD_OK        0
#define DD_NAK       1
#define DD_EXT       2
#define DD_LEN       3
#define DD_TRASH     4
#define DD_PRINTER   5
#define DD_CLIPBOARD 6

#define DD_TIMEOUT	4000		/* Timeout in ms */

#define DD_NUMEXTS	8			/* Number of formats */
#define DD_EXTLEN   4
#define DD_EXTSIZE	(DD_NUMEXTS * DD_EXTLEN)
#define DD_NAMEMAX	128
#define DD_TIMEOUT	4000

#define DD_FNAME	"U:\\PIPE\\DRAGDROP.AA"
#define DD_NAMEMAX	128			/* Maximum length of a format name */
#define DD_HDRMIN	9			/* Minimum length of Drag&Drop headers */
#define DD_HDRMAX	( 8 + DD_NAMEMAX )	/* Maximum length */


/*** ********************************************************************** ***/
/*** ---------------------------------------------------------------------- ***/
/*** ********************************************************************** ***/

static int dd_reply(int fd, unsigned char ack)
{
	if (Fwrite(fd, 1L, &ack) != 1)
		return 0;
	return 1;
}

/*** ---------------------------------------------------------------------- ***/

static void dd_close(int fd)
{
	(void)Psignal(SIGPIPE, oldsig);
	Fclose(fd);
}

/*** ---------------------------------------------------------------------- ***/

static int dd_open_pipe(_WORD pipe_num)
{
	int fd;
	char pipename[sizeof(PIPE_DRAGDROP)];

	strcpy(pipename, PIPE_DRAGDROP);
	pipename[PIPE_EXT] = (pipe_num & 0xff00) >> 8;
	pipename[PIPE_EXT+1] = pipe_num & 0xff;
	if ((fd = (int)Fopen(pipename, 2)) >= 0)
		oldsig = (__mint_sighandler_t)Psignal(SIGPIPE, SIG_IGN);
	return fd;
}

/*** ---------------------------------------------------------------------- ***/

void dd_nak(const _WORD *message)
{
	int fd;
	
	if ((fd = dd_open_pipe(message[7])) >= 0)
	{
		dd_reply(fd, DD_NAK);
		dd_close(fd);
	}
}
