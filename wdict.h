/***********************************************************************
 *
 *	file: wdict.h
 *
 *	$Id: wdict.h 114 2007-04-10 02:22:06Z chapuni $
 *
 */

#ifndef WDICT_H__
#define WDICT_H__

#include "crypt64.h"

struct WDICT;	/* vague */

/* ��̑��� */
#define CA_U	0x0001U
#define CA_C	0x0002U
#define CA_A	0x0004U

/* ����\�� */
struct DICT_CHAR
{
  uint64_t bmp;			/* �G���R�[�h���ꂽ��(LSB����) */
  unsigned xp	:10;	/* �召�g�� */
  unsigned len	: 4;	/* ������ */
  unsigned e	: 1;	/* �������� */
  unsigned attr	: 3;	/* ��̑��� */
};

extern struct WDICT *wdict_add(int tn,
							   int pos,
							   struct DICT_CHAR const *word);

extern void wdict_setnode(struct WDICT *pd,
						  struct ITREE *node);
extern void wdict_setup_lr(void);

extern int wdict_ishit(SLICE const *hit,	/* ������r�킪�Z�b�g�����q�b�g�z��[tn] */
					   int an, int ab,		/* SLICE::a[an].bit(ab) */
					   uint64_t bmp);		/* �f�R�[�h�ςݕ����� */

extern void wdict_dump(int lv, struct WDICT const *dict);

extern void wdict_setup_gpu(void **dictpool, void **wdk_pool, void **wdw_pool,
							int32_t *dictpoolsize, int32_t *wdksize, int32_t *wdwsize,
							int32_t *ret_min_dictpool, int32_t *ret_n_dictpool);

#endif /* WDICT_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
