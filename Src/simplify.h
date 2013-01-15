/***********************************************************************
 *
 *	file: simpilfy.h
 *
 *	$Id: simplify.h 187 2007-12-26 09:02:40Z chapuni $
 *
 */

#ifndef SIMPLIFY_H__
#define SIMPLIFY_H__

#include "mtytypes.h"

/* 64 mask */
#define QMX_BITS	6
#define QMX_LEN		(1U << QMX_BITS)
#define QMX_N_MASK	(QMX_LEN - 1)

struct QMX
{
  uint8_t m;	/* 6: Section */
  uint8_t n;	/* 6: 0 is the complement of section (?) */
};

extern int simplify_qm(struct QMX *aox,
					   uint64_t bmp,	/* Truth table */
					   uint64_t dc);	/* 1 bits are don't cares */

#endif /* SIMPLIFY_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
