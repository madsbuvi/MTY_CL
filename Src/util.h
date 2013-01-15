/***********************************************************************
 *
 *	file: util.h
 *
 *	$Id: util.h 175 2007-08-27 04:25:10Z chapuni $
 *
 */

#ifndef UTIL_H__
#define UTIL_H__

#include "mtytypes.h"

/***************************************************************
 *
 *	頻出するようなマクロ
 *
 */

/* n での剰余系 */
#define WRAP(a,n) (IS_POWER2(n) ? (a) & ((n) - 1) : (a) % (n))

/***************************************************************
 *
 *	便利なワリには直感的ではないビット判定
 *		- Hacker's Delight より引用
 *
 */

#define IS_POWER2(n) (!((n) & ((n) - 1)))

/***************************************************************
 *
 *	ビット数え上げ - Hacker's Delight より引用
 *
 */

static
unsigned
popcnt32(uint32_t x)
{
  x = x - ((x >> 1) & 0x55555555);
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  x = (x + (x >> 4)) & 0x0F0F0F0F;
  x = x + (x >> 8);
  x = x + (x >> 16);
  return x & 0x3F;	/* 32 が返ることもあるので 0x1F ではダメ */
}

static
unsigned
popcnt64(uint64_t x)
{
  return popcnt32(x) + popcnt32(x >> 32);
}

#endif /* UTIL_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
