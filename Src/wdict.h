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

/* 語の属性 */
#define CA_U	0x0001U
#define CA_C	0x0002U
#define CA_A	0x0004U

/* 語を構成 */
struct DICT_CHAR
{
  uint64_t bmp;			/* エンコードされた語(LSBから) */
  unsigned xp	:10;	/* 大小拡張 */
  unsigned len	: 4;	/* 文字長 */
  unsigned e	: 1;	/* 末尾特定 */
  unsigned attr	: 3;	/* 語の属性 */
};

extern struct WDICT *wdict_add(int tn,
							   int pos,
							   struct DICT_CHAR const *word);

extern void wdict_setnode(struct WDICT *pd,
						  struct ITREE *node);
extern void wdict_setup_lr(void);

extern int wdict_ishit(SLICE const *hit,	/* 生成比較器がセットしたヒット配列[tn] */
					   int an, int ab,		/* SLICE::a[an].bit(ab) */
					   uint64_t bmp);		/* デコード済み文字列 */

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
