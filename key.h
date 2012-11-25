/***********************************************************************
 *
 *	file: key.h
 *
 *	$Id: key.h 241 2010-03-15 14:28:18Z chapuni $
 *
 */

#ifndef KEY_H__
#define KEY_H__

#include "crypt64.h"

/* Œ®ƒNƒ‰ƒX */
struct KS_KEY
{
	uint8_t key[8 + 8];
	unsigned short map[8 + 8][256];
};

extern void key_make_map(struct KS_KEY *key, int n);
extern unsigned key_set(struct KS_KEY *key, int n, unsigned ch);
extern void key_set64(struct KEY *key64,
					  int n,
					  unsigned k,
					  unsigned vk,
					  unsigned sk);
extern int key_reset(struct KS_KEY *key, int n);
extern int key_inc(struct KS_KEY *key, int n, int e);
extern void key_init(struct KS_KEY *key);
extern void key_init_sk(struct KEY *key);
extern void set_salt(CODE_T *code,
					 struct CRYPT64_DESC const *desc,
					 uint8_t const *k);

#endif /* KEY_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
