#ifndef CRYPT64_H__
#define CRYPT64_H__

#include "config.h"

#define N_I			(sizeof(WS_T) / sizeof(uint32_t))
#define N_Q			(sizeof(WS_T) / sizeof(uint64_t))
#define N_ALU		(sizeof(WS_T) / sizeof(ALU_T))

/* 1ビット分 */
typedef union SLICE
{
  uint32_t	i[N_I];		/* 32-bit initializer */
  ALU_T		a[N_ALU];	/* Size that is easy for C to handle (?) */
  uint64_t	q[N_Q];		/* 64-bit for transpose */
  WS_T		w;			/* Size to handle the encoding (?) */
} SLICE;

/* parameters taken by crypt64() */
struct PARAM
{
  SLICE lr[2][32];
  SLICE t[32];
  SLICE hit[19];	/* The magic number to clean the packet boundary (?) */
};

/* All 1s (wat? 16 is 10000) */
#define T_INV		16

/* LR key needs to be copied
   KEY::k[0]			LR
   KEY::k[1]			LR copy
   KEY::k[2][0][i].a[0]	increment in the next key */
struct KEY
{
  SLICE k[2][2][28];
  SLICE ks[28];
};

/* Packets for batch processing */
struct PACKET_CRYPT64
{
  union
  {
	uint8_t		key[8];		/* Key set to the end */
	SLICE		pad;
  } uk;
  struct KEY	key64;
  struct PARAM	param64;	/* The last element, PARAM::hit[], is variable length */
};

/* CRYPT64 コア記述子 */
struct CRYPT64_DESC
{
  CODE_T const *pro;		/* 手続き入り口 */
  CODE_T const *crypt;		/* CRYPTコアループ */
  CODE_T const *cmp_pro;	/* 比較部セットアップ */
  CODE_T const *cmp_ep;		/* 比較部あとしまつ(計時程度?) */
  CODE_T const *ep;			/* 手続き出口 */
  CODE_T const *ep_end;		/* しんがり */
  uint8_t n_units;			/* 計算単位 */
  uint8_t ofsw;				/* オフセットの大きさ(2 .. int16_t) */
  uint16_t n_ofs;			/* オフセット組数 */
  int16_t ofs[1][48];		/* Saltオフセット表(crypt基準) */
};

/* 実装によってはこれ以外にも記述子が存在している */
extern struct CRYPT64_DESC const crypt64_desc;

extern int *debug_variables;

/* CODE基準のSalt左辺値 */
#define LSALT(d,c,n,i,j,o) (*(SALTOFS_T *)((c) + ((d)->crypt - (d)->pro) + (d)->ofs[n][6 * (i) + (j) + (o)]))

/* 戻り値は 64bit にパックされている。
   下位32ビットにコアループ tick
   上位32ビットに比較器 tick */

#ifdef USE_REGPARM

/* REGPARM は、A, D, C の順である
   呼ばれ側は MSFASTCALL(C, D) 前提なので、A にはダミー値を。 */
/* crypt(key, lr) */
typedef uint64_t
	 (*CRYPT64_PP_T)(void *a, struct PARAM *lr, struct KEY const *k) __attribute__((regparm(3)));
#define CALL_CRYPT64(code,k,lr) (*(CRYPT64_PP_T)(code))(code, lr, k)

/* cmp(hit, lr) */
typedef uint32_t
	 (*CMP64_PP_T)(void *a, SLICE (*lr)[32], SLICE *hit) __attribute__((regparm(3)));
#define CALL_CMP64(code,hit,lr) (*(CMP64_PP_T)(code))(code, lr, hit)

#elif defined(__GNUC__)

#define CALL_CRYPT64(code,k,lr)	\
({uint32_t a, c, d;							\
asm volatile("call *%3"						\
			 : "=a"(a), "=c"(c), "=d"(d)	\
			 : "r"(code), "1"(k), "2"(lr) 	\
			 : "memory");					\
((uint64_t)d << 32) | a;})

#define CALL_CMP64(code,hit,lr)	\
({uint32_t cnt, c, d;						\
asm volatile("call *%3"						\
			 : "=a"(cnt), "=c"(c), "=d"(d)	\
			 : "r"(code), "1"(hit), "2"(lr)	\
			 : "memory");					\
cnt;})

#else /* __fastcall 前提 */

/* crypt(key, lr) */
typedef uint64_t (__fastcall *CRYPT64_PP_T)(struct KEY const *k, struct PARAM *lr);
#define CALL_CRYPT64(code,k,lr) (*(CRYPT64_PP_T)(code))(k, lr)

/* cmp(hit, lr) */
typedef uint32_t (__fastcall *CMP64_PP_T)(SLICE *hit, SLICE (*lr)[32]);
#define CALL_CMP64(code,hit,lr) (*(CMP64_PP_T)(code))(hit, lr)

#endif

#endif /* CRYPT64_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
