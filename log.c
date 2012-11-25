/***********************************************************************
 *
 *	file: log.c
 *
 *	$Id: log.c 244 2010-03-16 13:54:51Z chapuni $
 *
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>

#if defined(WIN32)

#include <windows.h>
#include <process.h>

#elif defined(__GNUC__)

#include <sys/time.h>

#endif

#include "log.h"

static HANDLE mutex_log;
static FILE *ofp;

int
log_open(char const *name)
{
	if ((ofp = fopen(name, "at")) == NULL)
	{
		perror("log.txt");
		return errno;
	}

	setvbuf(ofp, NULL, _IONBF, BUFSIZ);	/* XXX MSVCRT ‚Å‚Í _IOLBF ‚ªŠú‘Ò’Ê‚è‚É“®ì‚µ‚È‚¢ */

	mutex_log = CreateMutex(NULL, FALSE, NULL);

	return 0;
}

static
int
log_printf(char const *fmt, ...)
{
  int r;
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stdout, fmt, ap);
  r = vfprintf(ofp, fmt, ap);
  va_end(ap);
  if (r > 0)
	return r;
  perror("log_printf");
  exit(errno);
}

void
log_print(int full, char const *hash, uint8_t const *key)
{
	struct timeb tb;
	struct tm *plt;

	/* ƒqƒbƒgŽž; –‚‰ü‘¢‚Æ‚¿‚å‚Á‚Æˆá‚¤ */
	ftime(&tb);
	plt = localtime(&tb.time);

	WaitForSingleObject(mutex_log, INFINITE);

	if (full)
	{
		log_printf("!%s #%s"
				   "\t%04d/%02d/%02d %02d:%02d:%02d.%03d"
				   "\t(%02X %02X %02X %02X %02X %02X %02X %02X/%02X)\n",
				   hash,
				   key,
				   plt->tm_year + 1900,
				   plt->tm_mon + 1,
				   plt->tm_mday,
				   plt->tm_hour,
				   plt->tm_min,
				   plt->tm_sec,
				   tb.millitm,
				   key[0], key[1], key[2], key[3],
				   key[4], key[5], key[6], key[7],
				   key[8]);
	}
	else
	{
		log_printf("!%s\t\t\t\t\t(%02X %02X %02X %02X %02X %02X %02X %02X)\n",
				   hash,
				   key[0], key[1], key[2], key[3],
				   key[4], key[5], key[6], key[7]);
	}
	ReleaseMutex(mutex_log);
}

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
