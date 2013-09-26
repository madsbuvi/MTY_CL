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

#define WRAP(a,n) (IS_POWER2(n) ? (a) & ((n) - 1) : (a) % (n))


#define IS_POWER2(n) (!((n) & ((n) - 1)))


static
unsigned
popcnt32(uint32_t x)
{
  x = x - ((x >> 1) & 0x55555555);
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  x = (x + (x >> 4)) & 0x0F0F0F0F;
  x = x + (x >> 8);
  x = x + (x >> 16);
  return x & 0x3F;
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
