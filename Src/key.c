/***********************************************************************
 *
 *	file: key.c
 *
 *	$Id: key.c 241 2010-03-15 14:28:18Z chapuni $
 *
 */

#include <assert.h>
#include <stdlib.h>

#include "cp932.h"
#include "desconst.h"
#include "key.h"
#include "translate.h"

#if USE_DT
#include "dt4.h"
#endif

/* Key expansion classes */
#define KCLS_DT0 64
#define KCLS_DT1 128
#define KCLS_K2 256

#if USE_DT
/* 鍵キメ用辞書インデクス */
struct DT *kd[8 + 8];

/* Dictionary index */
struct DT *dtidx[0x100 + 1];
#endif

/* 指定されたクラスと入っているキーから、classify を行う */
void
key_make_map(struct KS_KEY *key, int n)
{
  int i, j;
  unsigned c = key->map[n][key->key[n]];

#if USE_DT
  if (3 <= n && n < 7 && kd[n - 3])
	{
	  /* 辞書のケツの文字。後ろにナニヤラキャラクタが来る */
	  c = kd[n - 3]->c[0];
	  if ((0x81 <= c && c <= 0x9F)
		  || (0xE0 <= c && c <= 0xFC))
		c = KCLS_K2;
	  else
		c = (cp932[256 * key->key[n]]
			 | cp932[256 * (key->key[n] ^ 0x80)]);
#if DEBUG>=1
	  printf("*n=%d, key=%02X, cls=%04X\n",
			 n,
			 key[n],
			 c);
#endif
	}
  else if (2 <= n && n < 6 && kd[n - 2])
	{
	  return;
	}
  else if (1 <= n && n < 5 && kd[n - 1])
	{
	  return;
	}
#if USE_DT
  else if (1 <= n && n < 5 && !kd[n - 1]
		   //&& (c & KCLS_K2)
		   && (c & KCLS_DT1))
	{
	  /* 漢字2文字を拾っていきまつ */
#if DEBUG>=1
	  printf("(%d)%02X %02X(%02X:%02X:%02X:%02X)\n",
			 n, key->key[n - 1], key->key[n],
			 cp932[(256 * key->key[n - 1] + key->key[n])],
			 cp932[(256 * key->key[n - 1] + key->key[n]) ^ 0x0080],
			 cp932[(256 * key->key[n - 1] + key->key[n]) ^ 0x8000],
			 cp932[(256 * key->key[n - 1] + key->key[n]) ^ 0x8080]);
#endif
	  if (n != 1 && n != 2
		  && (cp932[(256 * key->key[n - 1] + key->key[n]) ^ 0x0080] & KCLS_DT1))
		key->key[n] ^= 0x80;
	  else if (n != 2 && n != 3
			   && (cp932[(256 * key->key[n - 1] + key->key[n]) ^ 0x8000] & KCLS_DT1))
		key->key[n - 1] ^= 0x80;
	  else if (n > 3 && (cp932[(256 * key->key[n - 1] + key->key[n]) ^ 0x8080] & KCLS_DT1))
		key->key[n - 1] ^= 0x80, key->key[n] ^= 0x80;
	  if (cp932[256 * key->key[n - 1] + key->key[n]] & KCLS_DT1)
		{
		  for (kd[n - 1] = dtidx[key->key[n - 1]];
			   kd[n - 1]->c[1] != key->key[n];
			   kd[n - 1]++)
			assert(kd[n - 1]->c[0] == key->key[n - 1]);
#if DEBUG>=1
		  printf("(%02X%02X:%02X%02X)%c%c%c%c\n",
				 kd[n - 1]->c[0],
				 kd[n - 1]->c[1],
				 kd[n - 1]->c[2],
				 kd[n - 1]->c[3],
				 kd[n - 1]->c[0],
				 kd[n - 1]->c[1],
				 kd[n - 1]->c[2],
				 kd[n - 1]->c[3]);
#endif
		  return;
		}
	}
  else if (n < 4 && (c & KCLS_DT0) && kd[n] == NULL)
	{
	  /* カタカナ埋め込みいきます */
	  assert(kd[n] == NULL);
#if DEBUG>=1
	  printf("n=%d, key=%02X\n", n, key->key[n]);
#endif
	  kd[n] = dtidx[key->key[n]];
	  if (!kd[n]
		  && !(n == 1 || n == 2)
		  && dtidx[key->key[n] ^ 0x80])
		{
		  key->key[n] ^= 0x80;
		  kd[n] = dtidx[key->key[n]];
		}
	  if (kd[n])
		return;
	}
#endif
  else
	{
	  kd[n] = NULL;
	}
#endif

  /* 最後の部分は class map を生成する必要ナシ */
  if (n >= 6)
	return;

  for (i = 0; i < 256; i++)
	{
	  unsigned bm = 0;
#if 1
	  if (c & KCLS_K1)
		{
		  if (cp932[256 * key->key[n] + i] & KCLS_K1)
			bm |= KCLS_K2 | (cp932[256 * key->key[n] + i] & KCLS_DT1);
		  if (cp932[256 * (key->key[n] ^ 0x80) + i] & KCLS_K1)
			bm |= KCLS_K2 | (cp932[256 * (key->key[n] ^ 0x80) + i] & KCLS_DT1);
#if 0
		  bm |= ((cp932[256 * key->key[n] + i] & KCLS_K1)
				 || (cp932[256 * (key->key[n] ^ 0x80) + i] & KCLS_K1)
				 ? KCLS_K2 : 0);
#endif
		}
	  if (c & (KCLS_AN | KCLS_KA | KCLS_K2))
		for (j = 0; j < 256; j++)
		{
		  bm |= cp932[256 * i + j] & (KCLS_AN | KCLS_KA | KCLS_K1
									  | KCLS_DT0);
#if 0
		  if (j >= 127 && !(n == 0 || n == 1))
			break;
#endif
		}
	  key->map[n + 1][i] = bm;
#endif
	  if (i >= 128 && !(n == 0 || n == 1))
		key->map[n + 1][i - 128] |= key->map[n + 1][i];
	}

  if (n < 6)
	key->map[n + 1][0x00] = key->map[n + 1][0x80] = 0;
  if (n == 6)
	key->map[7][0x00] |= KCLS_AN;
}

#if USE_DT
unsigned
dt_get(int kdn,
	   int xn,
	   int n,
	   int ch)
{
  int i;
#if DEBUG>=1
  printf("*dt_get(%d)%c%c%c%c(%02X%02X:%02X%02X)->ch=%d",
		 n,
		 kd[kdn]->c[0], kd[kdn]->c[1], kd[kdn]->c[2], kd[kdn]->c[3],
		 kd[kdn]->c[0], kd[kdn]->c[1], kd[kdn]->c[2], kd[kdn]->c[3],
		 ch);
#endif
  /* まずは数える */
  for (i = 0;
	   kd[kdn][i].c[xn] == kd[kdn]->c[xn];
	   i++)
	;
  assert(i > 0);
  kd[kdn] += ch % i;
#if DEBUG>=1
  printf("/%d\n dt_get:  %c%c%c%c(%02X%02X:%02X%02X)->ch=%d\n",
		 i,
		 kd[kdn]->c[0], kd[kdn]->c[1], kd[kdn]->c[2], kd[kdn]->c[3],
		 kd[kdn]->c[0], kd[kdn]->c[1], kd[kdn]->c[2], kd[kdn]->c[3],
		 ch);
#endif
  return kd[kdn]->c[n];
}
#endif

/* マップから文字を拾ってセット */
unsigned
key_set(struct KS_KEY *key, int n, unsigned ch)
{
  int cnt = 0, i;

#if USE_DT
  if (3 <= n && n < 7 && kd[n - 3])
	{
	  return dt_get(n - 3, 2, 3, ch);
	  return kd[n - 3]->c[3];
	}
  else if (2 <= n && n < 6 && kd[n - 2])
	{
	  return dt_get(n - 2, 1, 2, ch);
	  return kd[n - 2]->c[2];
	}
  else if (1 <= n && n < 5 && kd[n - 1])
	{
	  return dt_get(n - 1, 0, 1, ch);
	  return kd[n - 1]->c[1];
	}
#endif

#if DEBUG>=3
  if (cnt == 0)
	{
	  printf("n=%d, ch=%d, (n-1)=%02X\n", n, ch, key[n - 1]);
	  int j;
	  for (i = 0; i < 16; i++)
		{
		  printf("map[0x%02X] =", 16 * i);
		  for (j = 0; j < 16; j++)
			printf(" %03X", kcls[n].map[16 * i + j]);
		  printf("\n");
		}
	}
#endif
  for (i = 0; i < 256; i++)
	{
#if 1
	  /* 鳥屋っぽーい */
	  if (1 <= n && n <= 2 && '.' < i && i <= 'z')
		continue;
#endif
	  if (key->map[n][i])
		{
		  if (ch-- == 0)
			return i;
		  cnt++;
		}
	  if (n != 1 && n != 2 && i >= 127)
		break;
	}
  /* 見つからなかったのでもいっぺん */
  assert(cnt > 0);
  ch %= cnt;
  for (i = 0; i < 256; i++)
	if (key->map[n][i])
	  {
#if 1
		/* 鳥屋っぽーい */
		if (1 <= n && n <= 2 && '.' < i && i <= 'z')
		  continue;
#endif
		if (ch-- == 0)
		  return i;
	  }
  assert(!"not matched");
  return 0;
}

/* set the bitwise key */
void
key_set64(struct KEY *key64,
		  int n,
		  unsigned k,
		  unsigned vk,
		  unsigned sk)
{
  int i, j;
  if (!((vk | sk) & 0x7F))
	return;

  for (i = 0; i < 7; i++)
	{
	  if (n == 7 && i < N_STRIDE) continue;
	  if (sk & (1 << i))
		{
		  /* Set */
		  int o = tr_pc1[n][6 - i] - 1;
		  if (o < 28)
			{
			  assert(o >= 0);
			  for (j = 0; j < N_ALU; j++)
				key64->k[0][0][o].a[j]
				= key64->k[0][1][o].a[j]
				  = -!!(k & (1 << i));
			}
		  else
			{
			  assert(o >= 28);
			  assert(o < 56);
			  for (j = 0; j < N_ALU; j++)
				key64->k[1][0][o - 28].a[j]
				  = key64->k[1][1][o - 28].a[j]
				  = -!!(k & (1 << i));
			}
		}
	  else if (vk & (1 << i))
		{
		  /* Inversion */
		  int o = tr_pc1[n][6 - i] - 1;
		  if (o < 28)
			{
			  assert(o >= 0);
			  for (j = 0; j < N_ALU; j++)
			  key64->k[0][0][o].a[j]
				= key64->k[0][1][o].a[j]
				= ~key64->k[0][0][o].a[j];
			}
		  else
			{
			  assert(o >= 28);
			  assert(o < 56);
			  for (j = 0; j < N_ALU; j++)
				key64->k[1][0][o - 28].a[j]
				= key64->k[1][1][o - 28].a[j]
				= ~key64->k[1][0][o - 28].a[j];
			}
		}
	}
}

/* 指定されたクラスの開始値にリセット
   直前の文字のクラスに縛られる */
int
key_reset(struct KS_KEY *key, int n)
{
  if (n >= 8)
	return 1;
  if (n == 7)
	{
	  key->key[7] = 0;
	  return 1;
	}

  /* 0-2 文字目はランダムに決める
	 3 文字目以降は初期値に */
  if (n >= KEY_SHUFFLE_POS)
	key->key[n] = key_set(key, n, 0);
  else
	key->key[n] = key_set(key, n, rand());

#if DEBUG>=3
  printf("key[%d]=%02X ncls=%04X\n", n, key[n], kcls[n].map[key[n]]);
#endif

  /* セットされた文字を元に、次キャラの文字クラスを決める */
  key_make_map(key, n);

  return key_reset(key, n + 1);
}

/* One of the keys advances into the designed keyspace
   Returns true if it was able to safely increment */
int
key_inc(struct KS_KEY *key, int n, int e)
{
  if (n >= 8){
  /* Oops */
	return 0;
}
  else if (n == 7)
	{
	  /* The last byte */
	  uint8_t o_k = (key->key[7] + (1 << N_STRIDE)) & 0x7F;
	  if (!o_k)
		return 0;	/*  When not able to increment
						wait without advancing. */

	  /* advance */
	  key->key[7] = o_k;
	  return 1;
	}
  else if (n >= e - 1)
  {
	  /* do nothing */
  }
  else if (key_inc(key, n + 1, e)
		   /*
			&& key_inc(n + 1)
		   && key_inc(n + 1)
		   && key_inc(n + 1)*/
		   )
	return 1;

  /* Promise not to increment salt */
  if (n == 1 || n == 2)
	return 1;

#if DEBUG>=3
  printf("key_inc(n=%d,ck=%02X)\n", n, key->key[n]);
#endif

#if USE_DT
  /* 辞書語はインクリメントしていい約束にする */
  if (3 <= n && n < 7 && kd[n - 3])
	{
	  if ((key->key[n - 3] & 0x7F) == ((kd[n - 3] + 1)->c[0] & 0x7F)
		  && (key->key[n - 2] & 0x7F) == ((kd[n - 3] + 1)->c[1] & 0x7F)
		  && (key->key[n - 1] & 0x7F) == ((kd[n - 3] + 1)->c[2] & 0x7F))
		{
		  memcpy(&key->key[n - 3], &(++kd[n - 3])->c[0], 4);
#if DEBUG>=2
		  printf(">dt_get:%c%c%c%c(%02X%02X:%02X%02X)\n",
				 kd[n - 3]->c[0], kd[n - 3]->c[1], kd[n - 3]->c[2], kd[n - 3]->c[3],
				 kd[n - 3]->c[0], kd[n - 3]->c[1], kd[n - 3]->c[2], kd[n - 3]->c[3]);
#endif
		  return 1;
		}
	  else
		{
		  return 0;
		}
	}
  else if (2 <= n && n < 6 && kd[n - 2])
	{
	  if ((key->key[n - 2] & 0x7F) == ((kd[n - 2] + 1)->c[0] & 0x7F)
		  && (key->key[n - 1] & 0x7F) == ((kd[n - 2] + 1)->c[1] & 0x7F))
		{
		  memcpy(&key->key[n - 2], &(++kd[n - 2])->c[0], 4);
#if DEBUG>=2
		  printf(">dt_get:%c%c%c%c(%02X%02X:%02X%02X)\n",
				 kd[n - 2]->c[0], kd[n - 2]->c[1], kd[n - 2]->c[2], kd[n - 2]->c[3],
				 kd[n - 2]->c[0], kd[n - 2]->c[1], kd[n - 2]->c[2], kd[n - 2]->c[3]);
#endif
		  return 1;
		}
	  else
		{
		  return 0;
		}
	  if (kd[n - 2]->c[0] == key->key[n - 2])
		return 1;
	  else
		return 0;
	}
  else if (1 <= n && n < 5 && kd[n - 1])
	{
	  unsigned c2 = kd[n - 1]->c[0];
	  if ((0x81 <= c2 && c2 <= 0x9F)
		  || (0xE0 <= c2 && c2 <= 0xFC))
		{
		  kd[n - 1] = NULL;
#if 0
		  if (!(n == 1 && n == 2))
			key->key[n] &= 0x7F;
		  if (!(n == 2 && n == 3))
			key->key[n - 1] &= 0x7F;
#endif
		  key_make_map(n - 1);
		}
	  else if ((key->key[n - 1] & 0x7F) == ((kd[n - 1] + 1)->c[0] & 0x7F))
		{
		  memcpy(&key->key[n - 1], &(++kd[n - 1])->c[0], 4);
#if DEBUG>=2
		  printf(">dt_get:%c%c%c%c(%02X%02X:%02X%02X)\n",
				 kd[n - 1]->c[0], kd[n - 1]->c[1], kd[n - 1]->c[2], kd[n - 1]->c[3],
				 kd[n - 1]->c[0], kd[n - 1]->c[1], kd[n - 1]->c[2], kd[n - 1]->c[3]);
#endif
		  return 1;
		}
	  else
		{
		  return 0;
		}
#if 0
	  if (kd[n - 1]->c[0] == key->key[n - 1])
		return 1;
	  else
		return 0;
#endif
	}
  else if (n < 4 && kd[n])
	{
	  if (0 && kd[n]->c[0] == key->key[n])
		return 1;
	  kd[n] = NULL;
#if 0
	  if (!(n == 1 || n == 2))
		key->key[n] &= 0x7F;
#endif
	}
#endif

  /* 実際に増やしてみる */
  assert(n >= 3);
  for (;;)
	{
#if USE_DT
	  if (n <= 3
		  && !(key->key[n] & 0x80)
		  && kcls[n].map[key->key[n] ^ 0x80] & (KCLS_DT0))
		{
		  /* 半角カタカナの1バイト目 */
		  key->key[n] ^= 0x80;
		}
	  else
#endif
		{
		  key->key[n] = (key->key[n] & 0x7F) + 1;
		  if (key->key[n] >= 0x80)
			{
			  key->key[n] = 0xFF;	/* 次に突入させないため */
			  return 0;
			}
		}

	  if (key->map[n][key->key[n]])
		{
		  key_make_map(key, n);
		  key_reset(key, n + 1);
		  return 1;
		}
	}
}

/* 鍵を完全にリセットする
   Saltもセットし直す */
void
key_init(struct KS_KEY *key)
{
  int i, j;

#if USE_DT
  /* 辞書を、インデクスを作りながらマップにはめこんで逝く
	 辞書はコード順昇順に並んでるものとする */
  for (i = 0; i < dtcnt; i++)
	{
	  unsigned c = dt[i].c[0];

	  assert(dt[i].c[0]
			 && dt[i].c[1]
			 && dt[i].c[2]
			 && dt[i].c[3]);

	  /* BSD 鯖でしにそうな文字は残念ながら除外 */
	  assert((dt[i].c[0] & 0x7F)
			 && (dt[i].c[1] & 0x7F)
			 && (dt[i].c[2] & 0x7F)
			 && (dt[i].c[3] & 0x7F));

	  /* インデクス */
	  if (!dtidx[c])
		dtidx[c] = &dt[i];

	  if ((0x81 <= c && c <= 0x9F)
		  || (0xE0 <= c && c <= 0xFC))
		{
		  /* 全角なので、2バイトきまった時点で立てる */
		  cp932[256 * c + dt[i].c[1]] |= KCLS_DT1;
		}
	  else if (0xA1 <= c && c <= 0xDF)
		{
		  /* 半角カナ */
		  for (j = 0; j < 256; j++)
			cp932[256 * c + j] |= KCLS_DT0;
		}
	}
  /* ケツ、ちうか番人 */
  dtidx[0x100] = &dt[i];
#endif

  key->key[8] = 0;

  /* 初期マップを組む */
  for (i = 0; i < 256; i++)
	{
	  unsigned bm = 0;
	  key->map[0][i] = 0;
	  for (j = 0; j < 256; j++)
		bm |= cp932[256 * i + j];
	  key->map[0][i] = bm & (KCLS_AN | KCLS_KA | KCLS_K1
							 | KCLS_DT0
							 );
	  if (i >= 128)
		key->map[0][i - 128] |= key->map[0][i];
	}

  key_reset(key, 0);
}

/***************************************************************
 *
 *	固定キーの生成
 *
 *	一見 Big Endian に非対応のように見えるだろうが
 *	随所でに散らばっている kludge により
 *	ALU_T が 64 ビットである限り、これで問題なく動く。
 *
 */

void
key_init_sk(struct KEY *key)
{
  int i, j;
  int o;
  uint64_t m;

  for (i = 5, m = 0xFFFFFFFF00000000ULL;
	   i >= 0;
	   m ^= (m >> (1 << --i)))
	{
	  o = tr_pc1[7][6 - i] - 1;
#if DEBUG>=2
	  printf("%d:%d->%2d: %08X%08X\n",
			 N_Q, i, o,
			 (unsigned)(m >> 32),
			 (unsigned)m);
#endif
	  for (j = 0; j < N_Q; j++)
		if (o < 28)
		  key->k[0][0][o     ].q[j] = key->k[0][1][o     ].q[j] = m;
		else
		  key->k[1][0][o - 28].q[j] = key->k[1][1][o - 28].q[j] = m;
	}
#if N_STRIDE==7
  /* bit 6 は Little Endian として扱う */
  o = 0;
  assert(tr_pc1[7][0] - 1 == o);
  assert(N_Q == 2);
  key->k[0][0][o].q[0] = key->k[0][1][o].q[0] = 0x0000000000000000ULL;
  key->k[0][0][o].q[1] = key->k[0][1][o].q[1] = 0xFFFFFFFFFFFFFFFFULL;
#endif
}

/***************************************************************
 *
 *	Salt のセット
 *	オペランドのオフセットを書き換えて回ってるので注意
 *
 */

void
set_salt(CODE_T *code,
		 struct CRYPT64_DESC const *desc,
		 uint8_t const *k)
{
  int i, j;

  for (i = 0; i < 2; i++)
	{
	  unsigned s = k[1 + i] & 255;
	  if (s > 'z')
		s = 0;
	  else if (s >= 'a')
		s = s - 'a' + 2 + 10 + 26;
	  else if (s >= 'A')
		s = s - 'A' + 2 + 10;
	  else if (s >= '.')
		s = s - '.';
	  else
		s = 0;

#if DEBUG>=1
	  printf("Salt %d:%3o\n", i, s & 63);
#endif
	  for (j = 0; j < 6; j++)
		{
#if DEBUG>=2
		  //printf("Salt %d:%d %+3d:%+3d",
		  printf("Salt %d:%d %08lX:%08lX",
				 i, j,
				 LSALT(desc, code, 0, i, j,  0),
				 LSALT(desc, code, 0, i, j, 24));
#endif
		if (s & (1 << j))
		  {
			LSALT(desc, code, 0, i, j,  0) = sizeof(WS_T) * (((4 * i + j + 15) & 31) - 16);
			LSALT(desc, code, 0, i, j, 24) = sizeof(WS_T) * (((4 * i + j -  1) & 31) - 16);
		  }
		else
		  {
			LSALT(desc, code, 0, i, j,  0) = sizeof(WS_T) * (((4 * i + j -  1) & 31) - 16);
			LSALT(desc, code, 0, i, j, 24) = sizeof(WS_T) * (((4 * i + j + 15) & 31) - 16);
		  }
		LSALT(desc, code, 0, i, j, 12) = sizeof(WS_T) * (((4 * i + j +  7) & 31) - 16);
		LSALT(desc, code, 0, i, j, 36) = sizeof(WS_T) * (((4 * i + j + 23) & 31) - 16);
#if DEBUG>=2
		//printf(" => %+3d:%+3d\n",
		  printf(" => %08lX:%08lX\n",
				 LSALT(desc, code, 0, i, j,  0),
				 LSALT(desc, code, 0, i, j, 24));
#endif
		}
	}
}

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
