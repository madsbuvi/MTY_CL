/***********************************************************************
 *
 *	file: cp932.c
 *
 *	Kanji class table
 *
 */

#include "cp932.h"

unsigned char cp932[0x10000] = {
#include "cp932.inc"
};

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
