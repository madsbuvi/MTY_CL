/***********************************************************************
 *
 *	file: scoreboard.h
 *
 *	$Id: scoreboard.h 114 2007-04-10 02:22:06Z chapuni $
 *
 */

#ifndef SCOREBOARD_H__
#define SCOREBOARD_H__

#include <stdio.h>

extern FILE *scoreboard_open(void);
extern void *scoreboard_map(FILE *fp);

#endif /* SCOREBOARD_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
