/***********************************************************************
 *
 *	file: scoreboard.c
 *
 *	Framework for using the file to which the comparison code is written.
 *
 *	$Id: scoreboard.c 242 2010-03-15 15:14:06Z chapuni $
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#if defined(WIN32)

#include <windows.h>
#include <fcntl.h>
#include <io.h>

#elif defined(__GNUC__)

#include <sys/mman.h>
#include <unistd.h>

#endif

#include "config.h"
#include "scoreboard.h"

/***************************************************************
 *
 *	Open the scoreboard file
 *	Create a new file for the current version
 *
 */

FILE *scoreboard_open(void)
{
  char *fn = tempnam(NULL, "mtywk");

#ifdef WIN32
  FILE *fp;
  HANDLE h;
  h = CreateFile(fn,
				 GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE,
				 FILE_SHARE_DELETE
#if DEBUG>=1
				 | FILE_SHARE_READ | FILE_SHARE_WRITE
#endif
				 | 0,
				 NULL,
				 CREATE_ALWAYS,
				 FILE_ATTRIBUTE_NORMAL
#if DEBUG<1
				 | FILE_FLAG_DELETE_ON_CLOSE
#endif
				 | 0,
				 NULL);
  fp = _fdopen(_open_osfhandle((intptr_t)h, _O_RDWR), "w+b");
#else
  FILE *fp = fopen(fn, "w+b");
#endif

  assert(fp != NULL);
#if DEBUG>=1
  fprintf(stderr, "scoreboard: <%s>\n", fn);
#else
  unlink(fn);
#endif
  free(fn);	/* it was aquired by malloc(3) */
  return fp;
}

/***************************************************************
 *
 *	作業ファイルをメモリ上に実現する(W32)
 *
 */

void *scoreboard_map(FILE *fp)
{
  void *adr = NULL;
  off_t len;

#ifdef WIN32
  HANDLE h;
#endif

  /* イメージ長を得る */
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  assert(len > 0);

  fflush(fp);

#ifdef WIN32
  h = CreateFileMapping((HANDLE)_get_osfhandle(_fileno(fp)),
						NULL,
						PAGE_EXECUTE_READWRITE,
						0, 0,
						NULL);
#ifndef FILE_MAP_EXECUTE
#define FILE_MAP_EXECUTE SECTION_MAP_EXECUTE	/* XXX cygwin */
#endif
  if (h)
	adr = MapViewOfFile(h,
						FILE_MAP_EXECUTE | FILE_MAP_READ | FILE_MAP_WRITE,
						0, 0,
						0);
#else
  adr = mmap(NULL,
			 len,
			 PROT_EXEC | PROT_READ | PROT_WRITE,
			 MAP_SHARED,
			 fileno(fp),
			 0);
  if (adr == MAP_FAILED)
	adr = NULL;
#endif

  /* Check whether memory could be allocated */
  if (!adr)
	{
	  adr = malloc(len);
	  assert(adr != NULL);
	  rewind(fp);
	  fread(adr, 1, len, fp);
	}

#if DEBUG>=1
  fprintf(stderr,
		  "scoreboard: mapped to 0x%p, siz=0x%08X(%d)\n",
		  adr,
		  (unsigned)len, (unsigned)len);
#endif
  fprintf(stderr,
		  "A work area of %d allocated\n",
		  (unsigned)len);

  return adr;
}

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
