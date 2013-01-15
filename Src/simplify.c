/***********************************************************************
 *
 *	file: simplify.c
 *
 *	Simplification of Boolean expression (Quine-Mucluskey's method)
 *
 *	$Id: simplify.c 185 2007-12-25 06:56:46Z chapuni $
 *
 */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "simplify.h"
#include "util.h"

/***************************************************************
 *
 *	�K�{�區���O�֗���悤�Ƀ\�[�g
 *
 */

static
int
cmp_minterm_cov(void const *va, void const *vb)
{
  uint64_t a = *(uint64_t const *)va;
  uint64_t b = *(uint64_t const *)vb;
  return popcnt64(a) - popcnt64(b);
}

/***************************************************************
 *
 *	�핢�\����ŏ��핢�����o��
 *
 *	cov[] �� 1 ���[�h��a���Ƃ݂Ȃ���
 *	�e�X 1 �r�b�g�ÂE���Ƃ����T�O�B
 *
 *	���łɏE��ꂽ�r�b�g(cand)���܂ލ��̓X�L�b�v�\�B
 *
 */

static 
uint64_t
extract_mincov(uint64_t const *cov,	/* �핢�\ */
			   int covn,			/* �핢�\�v�f�� */
			   uint64_t cand,		/* ���肵�Ă���핢��� */
			   int min)				/* �ŏ��L�^ */
{
  int candn;
  int candni;
  uint64_t c;
  uint64_t b;
  uint64_t r = ~0ULL;

  /* cand �̍����́A�r�b�g�����̂��� */
  candn = popcnt64(cand);

  /* cand �Ɋ܂܂�Ă���A���Ȃ킿���łɌ�₪���鍀��
	 ����ς݂Ƃ݂Ȃ���̂ŃX�L�b�v���� */
  do
	{
	  /* �\�̃P�c�܂łȂ߂������画�� */
	  if (covn == 0)
		  return (min > candn
				  ? cand
				  : r);
	}
  while (--covn, ((c = *cov++) & cand));

  /* �ȍ~�́A��₩�獀���ЂƂA
	 ���Ȃ킿�r�b�g���ЂƂ��������̂����� */
  candni = candn + 1;

  for (b = 1ULL; b; b <<= 1)
	{
	  int n;
	  uint64_t m;

	  if (!(c & b))
		continue;

	  /* �ŏ��L�^���X�V�ł��Ȃ��Ɣ��f�����ꍇ */
	  if (min <= candni)
		return r;

	  /* ���̏����͏Ȃ��Ă��\��Ȃ����A
		 �ċA�[�x��������Ƃ����a�炰�A�������ɂȂ��� */
	  if (covn == 0)
		return cand | b;

	  /* ��i���v�Z�����Ă݂�B
		 min ���X�V�ł��Ȃ������ꍇ�� ~0ULL ���Ԃ� */
	  m = extract_mincov(cov, covn, cand | b, min);
	  n = popcnt64(m);
	  if (min > n)
		{
		  /* �L�^�X�V */
		  min = n;
		  r = m;
		}
	}

  /* �L�^���X�V�ł��Ȃ������� ~0ULL ���Ԃ邱�ƂɂȂ� */
  return r;
}

/***************************************************************
 *
 *	Quine-McCluskey's method to simply the bmp
 *
 *	Returns the aproximate number of terms
 *	If it gives up it returns INT_MAX to imply prime
 *
 *	Returns the the sum of products from aox(m==0 terminated)
 *
 */

#define QMX_N_ELIM	0x80	/* eliminate sign bit in QMX::n */

int
simplify_qm(struct QMX *aox,
			uint64_t bmp,	/* Truth table */
			uint64_t dc)	/* 1-bits are dontcares */
{
  int i, j;
  uint64_t m;
  int n_dcs;					/* count of dontcares*/

  /* �߂�l: �T�Z�o�͍��� */
  int n_estimated_terms;

  /* �ŏ��� */
  int n_minterms;
  struct QMX minterms[QMX_LEN];
  uint64_t minterm_cov[QMX_LEN];	/* �핢 */
  uint64_t covered_minterms;		/* �K�{�區���핢����ŏ��� */

  /* �區 */
  int n_pterms;
  struct QMX atmx[256 * QMX_LEN];	/* 64k �𒴂��Ȃ����炢���ڈ�? */
  int max_pterms = sizeof(atmx) / sizeof(struct QMX);
  struct QMX *pterms = atmx;

  /* �K�{�區����т��̔핢 */
  uint64_t essential_pterms;	/* �K�{�區 */
  int n_essential_pterms;		/* �K�{�區���� */

  /* �區�Z�o�p��, D/C ����񋓂��� */
  n_dcs = 0;
  for (i = 0; i < QMX_LEN; i++)
	if (dc & (1ULL << i))
	  {
		pterms[n_dcs  ].m = QMX_N_MASK;
		pterms[n_dcs++].n = i;
	  }

  /* �ŏ���(1 �̍�)��񋓂��� */
  bmp &= ~dc;
  n_minterms = 0;
  for (i = 0; i < QMX_LEN; i++)
	if (bmp & (1ULL << i))
	  {
		minterms[n_minterms  ].m = QMX_N_MASK;
		minterms[n_minterms++].n = i & QMX_N_MASK;
	  }

  debug_printf(2, F016LLX"/"F016LLX" n_minterms=%d n_dcs=%d\n",
			   A016LLX(bmp),
			   A016LLX(dc),
			   n_minterms, n_dcs);

#define C(i,s) (pterms[i].m & (1 << (s)) ? (pterms[i].n & (1 << (s)) ? '1' : '0') : '-')

  /* ���̈�����������Ŏ區�����߂� */
  memcpy(&pterms[n_dcs], minterms, n_minterms * sizeof(struct QMX));
  n_pterms = n_minterms + n_dcs;
  do
	{
	  int pterm_idx = n_pterms;

	  /* �������߂����̂��區�Ƃ��Ĕ����o�� */
	  for (i = 0; i < n_pterms; i++)
		for (j = i + 1; j < n_pterms; j++)
		  {
			int k;
			unsigned x;
			if (pterms[i].m != pterms[j].m)
			  continue;
			x = (pterms[i].n ^ pterms[j].n) & pterms[i].m;
			assert(x != 0);	/* �O�X�e�[�W�ɂď����ς݂̃n�Y */
			if (!IS_POWER2(x))
			  continue;
			/* �n�~���O������1�Ȃ̂ō����܂Ƃ߂�
			   �قȂ�r�b�g����}�X�N�𐶐� */
			x = QMX_N_MASK & ~x;
			/* ���ꍀ�͍Đ������Ȃ��悤��(�ߖ�) */
			for (k = n_pterms; k < pterm_idx; k++)
			  if (pterms[k].m == (pterms[i].m & x)
				  && pterms[k].n == (pterms[i].n & x))
				goto tmx_dup;
			/* �����P�c�ɓo�^ */
			pterms[pterm_idx  ].m = pterms[i].m & x;
			pterms[pterm_idx++].n = pterms[i].n & x;
			assert (pterm_idx <= max_pterms);
			if (pterm_idx == max_pterms)
			  {
				/* pterms �̊g��
				   ���[�J���̈���͂ݏo����q�[�v����擾 */
				assert(max_pterms  < INT_MAX / sizeof(struct QMX) / 2);
				max_pterms *= 2;
				if (pterms == atmx)
				  {
					pterms = malloc(max_pterms * sizeof(struct QMX));
					memcpy(pterms, atmx, sizeof(atmx));
					debug_printf(1, "allocated: %d->%d\n", pterm_idx, max_pterms);
				  }
				else
				  {
					pterms = realloc(pterms, max_pterms * sizeof(struct QMX));
					debug_printf(1, "extended: %d->%d\n", pterm_idx, max_pterms);
				  }
			  }
		  tmx_dup:
			/* �܂Ƃ߂�����ꂽ���ɍ폜��(�Ƃ肠����MSB) */
			pterms[i].n |= QMX_N_ELIM;
			pterms[j].n |= QMX_N_ELIM;
		  }

	  /* �����o���ꂽ���̂���d�����������Ă��� */
	  for (i = 0; i < pterm_idx; i++)
		for (j = i + 1; j < pterm_idx; j++)
		  if (pterms[i].m == pterms[j].m
			  && ((pterms[i].n ^ pterms[j].n) & QMX_N_MASK) == 0)
			{
			  pterms[i].n |= QMX_N_ELIM;
			  break;	/* next i */
			}

	  /* �폜���ꂽ���̂��̂Ă�
		 �����珇�s���ŋl�߂Ă������� */
	  for (i = j = 0; i < pterm_idx; i++)
		if (!(pterms[i].n & QMX_N_ELIM))
		  {
			if (i != j)
			  pterms[j] = pterms[i];
			j++;
		  }

	  debug_printf(2, "n_pterms=%d pterm_idx=%d j=%d\n", n_pterms, pterm_idx, j);
	  n_pterms = j;
	}
  while (i != j);

  /* �e�ŏ����ɂ����āA�區�ɂ��핢�\�𐶐�
	 �K�{�區�����o���A�K�{�區�핢���L�^ */
  memset(minterm_cov, 0, n_minterms * sizeof(*minterm_cov));
  assert(n_minterms <= 64);
  assert(n_pterms <= 64);
  essential_pterms = 0;
  covered_minterms = 0;
  for (i = 0; i < n_minterms; i++)
	{
	  int ep = -1;	/* �K�{�區�̏ꏊ */
	  for (j = 0; j < n_pterms; j++)
		if ((minterms[i].m & pterms[j].m) == pterms[j].m
			&& (minterms[i].n & pterms[j].m) == (pterms[j].n & pterms[j].m))
		  minterm_cov[i] |= 1ULL << (ep = j);

	  if (ep >= 0
		  && IS_POWER2(minterm_cov[i]))
		{
		  /* �K�{�區���핢������̂��}�[�L���O���� */
		  for (j = 0; j < n_minterms; j++)
			if ((minterms[j].m & pterms[ep].m) == pterms[ep].m
				&& (minterms[j].n & pterms[ep].m) == (pterms[ep].n & pterms[ep].m))
			  {
				essential_pterms |= 1ULL << ep;
				covered_minterms |= 1ULL << j;
			  }
		}
	}

  /* �區-�ŏ����}�g���N�X�̃_���v; for debug */
#if DEBUG>=1
  printf("             ");
  for (j = 0; j < n_minterms; j++)
	if (IS_POWER2(minterm_cov[j]))
	  printf("*");
	else if (covered_minterms & (1ULL << j))
	  printf("+");
	else
	  printf("#");
  printf("\n");

  for (i = 0; i < n_pterms; i++)
	{
	  printf("%3d: %c%c%c%c%c%c: ",
			 i,
			 C(i,5), C(i,4), C(i,3), C(i,2), C(i,1), C(i,0));
	  for (j = 0; j < n_minterms; j++)
		if (minterm_cov[j] & (1ULL << i))
		  {
			if (IS_POWER2(minterm_cov[j]))
			  {
				assert(covered_minterms & (1ULL << j));
				printf("*");
			  }
			else if (covered_minterms & (1ULL << j))
			  printf("+");
			else
			  printf("#");
		  }
		else
		  {
			printf("-");
		  }
	  printf("\n");
	}
#endif

  /* �區��64�𒴂��Ă��܂����ꍇ�A������߂�(�}�W�J��) */
  if (n_pterms > 64)
	return INT_MAX;

  /* �핢�\�̏d������菜�� */
  n_estimated_terms = 1;
  for (i = 0; i < n_minterms; i++)
	{
	  for (j = i + 1; j < n_minterms; j++)
		if (minterm_cov[i] == minterm_cov[j])
		  {
			minterm_cov[j] = minterm_cov[--n_minterms];
			j--; continue;	/* redo */
		  }
	  if (minterm_cov[i])
		n_estimated_terms *= popcnt64(minterm_cov[i]);
	}

  debug_printf(2,
			   "n_minterms=%d n_pterms=%d n_estimated_terms=%d\n",
			   n_minterms, n_pterms, n_estimated_terms);

#if DEBUG>=2
  for (i = 0; i < n_minterms; i++)
	if (minterm_cov[i])
	  printf("%2d: "F016LLX"\n", i, A016LLX(minterm_cov[i]));
#endif

  /* �r�b�g���̏��������̂���(==�K�{�區���O��)
	 �\�[�g���邱�Ƃɂ��A�v�Z�������Ɗy�ɂȂ�(�n�Y) */
  qsort(minterm_cov, n_minterms, sizeof(*minterm_cov), cmp_minterm_cov);

  /* �ŏ��핢�̒��o
	 (�ŏ��Ȃ̂�?)
	 �ς�����Ȃ�������S�區��I��������ԂƂȂ� */
  m = extract_mincov(minterm_cov,
					 n_minterms,
					 0ULL,
					 64);
#if DEBUG>=1
  debug_printf(1,
			   "ess:"F016LLX" cov:"F016LLX" all:"F016LLX"\n",
			   A016LLX(essential_pterms),
			   A016LLX(m & ~essential_pterms),
			   A016LLX(m));
assert((m & essential_pterms) == essential_pterms);
#endif
  essential_pterms = m;

  /* ���ʂ�W�J */
  n_estimated_terms = n_essential_pterms = popcnt64(essential_pterms);
  essential_pterms &= ((1ULL << n_pterms) - 1);
  j = 0;
  for (i = 0; i < n_pterms; i++)
	if (essential_pterms & (1ULL << i))
	  {
		n_estimated_terms += popcnt32(pterms[i].m);
		if (aox)
		  aox[j++] = pterms[i];

		debug_printf(2, "%c%c%c%c%c%c:%3d\n",
					 C(i,5), C(i,4), C(i,3), C(i,2), C(i,1), C(i,0),
					 n_estimated_terms);

		assert(j <= QMX_LEN);
	  }

  /* ���Ԃ�Ԑl */
  if (aox && j < QMX_LEN)
	aox[j].m = 0;

  debug_printf(1, "nterm=%d j=%d\n", n_estimated_terms, j);

#if DEBUG>=1
  if (aox)
	{
	  /* �������ꂽ���̌��Z */
	  static const uint64_t a6[] =
		{
		  0xAAAAAAAAAAAAAAAAULL,
		  0xCCCCCCCCCCCCCCCCULL,
		  0xF0F0F0F0F0F0F0F0ULL,
		  0xFF00FF00FF00FF00ULL,
		  0xFFFF0000FFFF0000ULL,
		  0xFFFFFFFF00000000ULL,
		};
	  uint64_t o = 0;
	  for (i = 0; aox[i].m; i++)
		{
		  uint64_t a = 0xFFFFFFFFFFFFFFFFULL;
		  for (j = 0; j < 6; j++)
			if (aox[i].m & (1 << j))
			  a &= (aox[i].n & (1 << j)
					? a6[j]
					: ~a6[j]);
		  o |= a;
		}
	  debug_eprintf(1, "bmp="F016LLX", o="F016LLX", dc="F016LLX"\n",
					A016LLX(bmp),
					A016LLX(o),
					A016LLX(dc));
	  assert(((bmp ^ o) & ~dc) == 0);
	}
#endif

  /* pterms ���g������Ă���ꍇ�̂��Ƃ��܂� */
  if (pterms != atmx)
	free(pterms);

  /* �S���W�J���Ă��܂����ꍇ */
  return (n_essential_pterms > n_pterms ? INT_MAX : n_estimated_terms);
}

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
