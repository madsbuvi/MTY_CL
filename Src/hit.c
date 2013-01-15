/***********************************************************************
 *
 *	file: hit.c
 *
 *	$Id: hit.c 244 2010-03-16 13:54:51Z chapuni $
 *
 */

#include <string.h>

#include "desconst.h"
#include "hit.h"
#include "log.h"
#include "synth.h"
#include "tr64.h"
#include "translate.h"
#include "wdict.h"

/***************************************************************
 *
 *	Key transpose
 *
 */

static
void tr8_32(uint32_t (*A)[4])
{
	int j, k;
	uint32_t m, t;
	m = 0xF0F0F0F0UL;
	for (j = 4; j != 0; j >>= 1, m = m ^ (m >> j))
	{
		for (k = 0; k < 64; k = (k + j + 1) & ~j)
		{
			t = (A[k][0] ^ (A[k + j][0] << j)) & m;
			A[k][0] ^= t;
			A[k + j][0] ^= (t >> j);
		}
	}
}

static
void trk8(struct PACKET_CRYPT64 const *pkt, uint8_t *k8)
{
	int i, j;

	for (i = 0; i < 8; i++)
	{
		memset(&k8[128 * i + 16 * 7],
			   ((i == 1 || i == 2) && pkt->uk.key[i] & 0x80 ? -1 : 0),
			   16);
		for (j = 0; j < 7; j++)
		{
			int o = tr_pc1[i][6 - j] - 1;
			memcpy(&k8[128 * i + 16 * j],
				   (o < 28
					? &pkt->key64.k[0][0][o]
					: &pkt->key64.k[1][0][o - 28]),
				   16);
		}
	}
	for (i = 0; i < 4; i++)
	{
		tr8_32((uint32_t (*)[4])&k8[4 * i]);
	}
}

/***************************************************************
 *
 *	Hit Determination
 *
 */

void
check_hit(struct PACKET_CRYPT64 const *pkt_c,
		  SLICE const *hit)
{
	int i, k, kk;

	int xhash_loaded = 0;
	uint64_t xhash[64];

	int xkey_loaded = 0;
	uint8_t xkey_buf[8][16][8];

	for (kk = 0; kk < N_ALU; kk++)
	{

		ALU_T t;
		if (!(kk & (N_ALU / N_Q - 1)))
			xhash_loaded = 0;

		t = hit[HIT_ANY].a[kk];
		if (!t)
			continue;

		for (k = 0; k < ALU_BITS; k++)
		{
			char hash[16];
			uint8_t buf[32];

			if (!(t & ((ALU_T)1 << k)))
				continue;

			/* Perform FP and transpose matrix */
			if (!xhash_loaded)
			{
				CALL_TR64(&pkt_c->param64.lr[0][0].q[kk / (N_ALU / N_Q)], xhash);
				xhash_loaded = 1;
			}
			
			int hitbool = (hit[HIT_BOOL].a[kk] & ((ALU_T)1 << k));
			int wdicthit = wdict_ishit(hit,
								 kk, k,
								 xhash[(ALU_BITS * kk + k) & 0x3F]);
			
			/* Examine the dictionary */
			if (!(hitbool
				  || wdicthit))
				continue;

			for (i = 1; i < 11; i++)
			{
				unsigned c = 0;
				c = (xhash[(ALU_BITS * kk + k) & 63] >> (6 * (i - 1))) & 0x3F;	/* XXX */
				hash[i - 1] = C64[c];
			}
			hash[10] = 0;
			if (!xkey_loaded)
			{
				trk8(pkt_c, &xkey_buf[0][0][0]);
				xkey_loaded = 1;
			}

			for (i = 0; i < 8; i++)
				buf[i] = xkey_buf[i][0][((ALU_BITS * kk + k) >> 3) | (((ALU_BITS * kk + k) & 7) << 4)];
			buf[8] = buf[9] = 0;
			if ((buf[0] & 0x7F) == 0)
			{
				/* do nothing */
			}
			else
			{
				
				log_print(translate(buf, 0, 1), hash, buf);
			}
		}
	}
}

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
