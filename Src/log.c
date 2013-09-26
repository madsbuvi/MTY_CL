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
#include <locale.h>
#include <sys/time.h>
#endif

#include "log.h"
#include <assert.h>

#if defined(WIN32)

#elif defined(_POSIX_SOURCE)
#include <pthread.h>
#define HANDLE pthread_mutex_t
#define WaitForSingleObject(mutex, trash) pthread_mutex_lock(&mutex)
#define ReleaseMutex(mutex) pthread_mutex_unlock(&mutex);
#else
#error "Configuration not supported"

#endif

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
	assert(setvbuf(ofp, NULL, _IONBF, BUFSIZ)==0);	/* XXX MSVCRT ‚Å‚Í _IOLBF ‚ªŠú‘Ò’Ê‚è‚É“®ì‚µ‚È‚¢ */
#if defined(WIN32)
	mutex_log = CreateMutex(NULL, FALSE, NULL);
#elif defined(_POSIX_SOURCE)
	pthread_mutex_init(&mutex_log, NULL);
#else
#error "Configuration not supported"

#endif
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
  assert(ofp);
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
	
	assert(hash);


	if (full)
	{
		assert(key);
/*
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
*/
//On linux vfprintf targeting ofp would segfault, despite ofp being valid and the fact taht the preceding vfprintf succeeds show there are no problems with the arguments. Hrmpf.

		fprintf(stdout,"!%s #%s"
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
		fprintf(ofp,"!%s #%s"
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
		fprintf(stdout,"!%s\t\t\t\t\t(%02X %02X %02X %02X %02X %02X %02X %02X)\n",
				   hash,
				   key[0], key[1], key[2], key[3],
				   key[4], key[5], key[6], key[7]);
		fprintf(ofp,"!%s\t\t\t\t\t(%02X %02X %02X %02X %02X %02X %02X %02X)\n",
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
