/***********************************************************************
 *
 *	file: translate.c
 *
 *	�L�[�o�C�g����A CP932 �ɏ]���ĉ�������ɕϊ�����
 *	(������)�����N���C�A���g�Ŏg���Ă���̂Ɠ���B
 *
 */

#include <string.h>

#include "cp932.h"
#include "translate.h"

/***************************************************************
 *
 *	��������ɕϊ�
 *	n = 0, flag !=0 �ŌĂԂ���
 *
 *	ptr �Ŏw����镶����́A�ϊ��ɐ��������珑����������
 *
 *	���������� flg, ���s������ 0 ��Ԃ��B
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
	  /* �Ō��1���� */
	  if (!(ptr[7] & 0x7F))
		return flag;

	  /* ���p�󔒂̏ꍇ�A����\�h�̂���
		 ����1���������Ă����� */
	  if (ptr[7] == 0x20)
		{
		  if ((0x21 <= ptr[6] && ptr[6] <= 0x7E)
			  || (0xA1 <= ptr[6] && ptr[6] <= 0xDF))
			ptr[8] = ptr[6];
		  else
			ptr[8] = '?';	/* �Ă��Ƃ���1���� */
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
	  /* BSD�I�ł͎��s */
	  return 0;
  }
  else if (n == 6
		   && (ptr[6] | 0x80) == 0x81
		   && (ptr[7] & 0x7F) == 0x40)
	{
	  /* ������0x8140(�S�p��)�ɂȂ肻���ȂƂ���
		 �Ă��Ƃ���1�������P�c�ɑ����Ƃ��Ă����� */
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
