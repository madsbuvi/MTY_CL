/***********************************************************************
 *
 *	file: translate.c
 *
 *	キーバイト列を、 CP932 に従って可視文字列に変換する
 *	(たしか)鳥屋クライアントで使ってるものと同一。
 *
 */

#include <string.h>

#include "cp932.h"
#include "translate.h"

/***************************************************************
 *
 *	可視文字列に変換
 *	n = 0, flag !=0 で呼ぶこと
 *
 *	ptr で指される文字列は、変換に成功したら書き換えられる
 *
 *	成功したら flg, 失敗したら 0 を返す。
 *
 */

unsigned
translate(unsigned char *ptr,
		  int n,
		  unsigned flag)
{
  int r;
  unsigned char buf[32];
  unsigned s0 = (n == 1 || n == 2 ? 0x00 : 0x80);
  unsigned s1 = (n == 0 || n == 1 ? 0x00 : 0x80);
  unsigned c0 = ptr[n] << 8;
  unsigned c1 = ptr[n + 1];
  unsigned cs0 = c0 ^ (s0 << 8);
  unsigned cs1 = c1 ^ s1;

  if (n >= 8)
	return flag;

  if (n == 7)
	{
	  int i;
	  /* 最後の1文字 */
	  if (!(ptr[7] & 0x7F))
		return flag;

	  /* 半角空白の場合、錯誤予防のため
		 もう1文字足してあげる */
	  if (ptr[7] == 0x20)
		{
		  if ((0x21 <= ptr[6] && ptr[6] <= 0x7E)
			  || (0xA1 <= ptr[6] && ptr[6] <= 0xDF))
			ptr[8] = ptr[6];
		  else
			ptr[8] = '?';	/* てきとうな1文字 */
		  return flag;
		}

	  for (i = 0x00; i <= 0x7E; i++)
		{
		  if (cp932[c0 | i] & KCLS_K1)
			{
			  ptr[8] = i;
			  return flag;
			}
		  if (cp932[c0 | (0x80 + i)] & KCLS_K1)
			{
			  ptr[8] = 0x80 + i;
			  return flag;
			}
		}

	  ptr[7] ^= 0x80; c0 = ptr[7] << 8;
	  for (i = 0x00; i <= 0x7E; i++)
		{
		  if (cp932[c0 | i] & KCLS_K1)
			{
			  ptr[8] = i;
			  return flag;
			}
		  if (cp932[c0 | (0x80 + i)] & KCLS_K1)
			{
			  ptr[8] = 0x80 + i;
			  return flag;
			}
		}
	  ptr[7] ^= 0x80; c0 = ptr[7] << 8;
	}
  else if ((ptr[n] & 0x7F) == 0x00
		   && (ptr[n + 1] & 0x7F) != 0x00)
  {
	  /* BSD鯖では失敗 */
	  return 0;
  }
  else if (n == 6
		   && (ptr[6] | 0x80) == 0x81
		   && (ptr[7] & 0x7F) == 0x40)
	{
	  /* 末尾が0x8140(全角空白)になりそうなときは
		 てきとうな1文字をケツに足しといてあげる */
	  ptr[6] = 0x81;
	  ptr[7] = 0x40;
	  ptr[8] = 0x40;	/* @ */
	  return flag;
	}

  /* K1 */
  if (cp932[c0 | c1] & KCLS_K1)
	{
	  r = translate(ptr, n + 2, flag);
	  if (r)
		return r;
	}
  if (s0
	  && cp932[cs0 | c1] & KCLS_K1)
	{
	  memcpy(buf, ptr, sizeof(buf));
	  buf[n] ^= s0;
	  r = translate(buf, n + 2, flag);
	  if (r)
		{
		  memcpy(ptr, buf, sizeof(buf));
		  return r;
		}
	}
  if (s1
	  && cp932[c0 | cs1] & KCLS_K1)
	{
	  memcpy(buf, ptr, sizeof(buf));
	  buf[n + 1] ^= s1;
	  r = translate(buf, n + 2, flag);
	  if (r)
		{
		  memcpy(ptr, buf, sizeof(buf));
		  return r;
		}
	}
  if (s0 && s1
	  && cp932[cs0 | cs1] & KCLS_K1)
	{
	  memcpy(buf, ptr, sizeof(buf));
	  buf[n] ^= s0;
	  buf[n + 1] ^= s1;
	  r = translate(buf, n + 2, flag);
	  if (r)
		{
		  memcpy(ptr, buf, sizeof(buf));
		  return r;
		}
	}

  /* AN */
  if (cp932[c0] & (KCLS_AN | KCLS_KA))
	{
	  r = translate(ptr, n + 1, flag);
	  if (r)
		return r;
	}
  if (s0 && cp932[cs0] & (KCLS_AN | KCLS_KA))
	{
	  memcpy(buf, ptr, sizeof(buf));
	  buf[n] ^= s0;
	  r = translate(buf, n + 1, flag);
	  if (r)
		{
		  memcpy(ptr, buf, sizeof(buf));
		  return r;
		}
	}

  /* KG */
  /* KD */
  /* AD */

  return 0;
}

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
