/***********************************************************************
 *
 *	file: debug.h
 *
 *	$Id: debug.h 168 2007-08-22 12:51:58Z chapuni $
 *
 */

#ifndef DEBUG_H__
#define DEBUG_H__

#include <stdio.h>

#ifndef DEBUG
#define DEBUG 0
#endif

/***************************************************************
 *
 *	uint64_t を%08X%08Xに喰わせるためのマクロ
 *
 */

#define F016LLX	"%08X%08X"
#define A016LLX(n) (unsigned)((uint64_t)(n) >> 32), (unsigned)(uint64_t)(n)

/***************************************************************
 *
 *	デバッグ出力
 *
 */

#if DEBUG>=1

#define debug_printf(lvl,...)	do {if (DEBUG>=(lvl)) printf(__VA_ARGS__);} while (0)
#define debug_eprintf(lvl,...)	do {if (DEBUG>=(lvl)) fprintf(stderr, __VA_ARGS__);} while (0)

#else

#define debug_printf(lvl,...)	do; while(0)
#define debug_eprintf(lvl,...)	do; while(0)

#endif

#endif /* DEBUG_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
