/***********************************************************************
 *
 *	file: desconst.c
 *
 *	some DES constants
 *
 *	$Id: desconst.c 114 2007-04-10 02:22:06Z chapuni $
 *
 */

#include "desconst.h"

/* ���]�u PC1 */
int const tr_pc1[8][7] =
{
  { 8, 16, 24, 56, 52, 44, 36},
  { 7, 15, 23, 55, 51, 43, 35},
  { 6, 14, 22, 54, 50, 42, 34},
  { 5, 13, 21, 53, 49, 41, 33},
  { 4, 12, 20, 28, 48, 40, 32},
  { 3, 11, 19, 27, 47, 39, 31},
  { 2, 10, 18, 26, 46, 38, 30},
  { 1,  9, 17, 25, 45, 37, 29},
};

/* LR�ŏI�]�u */
int const tr_fp[64 + 2] =
{
  39,  7, 47, 15, 55, 23, 63, 31,
  38,  6, 46, 14, 54, 22, 62, 30,
  37,  5, 45, 13, 53, 21, 61, 29,
  36,  4, 44, 12, 52, 20, 60, 28,
  35,  3, 43, 11, 51, 19, 59, 27,
  34,  2, 42, 10, 50, 18, 58, 26,
  33,  1, 41,  9, 49, 17, 57, 25,
  32,  0, 40,  8, 48, 16, 56, 24,
  64, 64,
};

/* ���V�t�g�X�P�W���[��
   KEY::k[2] ���ґ�ɗp���Ď��o���̊ȑf����}�� */
int const ks_ls[28] =
{
  1, 1, 2, -1,
  2, -1, 2, -1,
  2, -1, 2, -1,
  2, -1, 1, 2,
  -1, 2, -1, 2,
  -1, 2, -1, 2,
  -1, 2, -1, 1 - 28,
};

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */