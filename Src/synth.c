/***********************************************************************
 *
 *	file: synth.c
 *
 *	Assemble comparison code from the tree.
 *
 *	Original source written by Chapuni (http://naniya.sourceforge.jp/, chapuni@users.sourceforge.jp)
 *	Modified by Mads Buvik Sandvei (https://github.com/madsbuvi/, madsbuvi@stud.ntnu.no) to
 *  also generate code for OpenCL.
 *
 */

#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "config.h"
#include "crypt64.h"
#include "debug.h"
#include "desconst.h"
#include "simplify.h"
#include "synth.h"
#include "util.h"
#include "wdict.h"

/* �A���m ad hoc */
#define SEQ	8

//cmp.cl
FILE *clfp;

#define work_area_size 128

static int greatest_work_area_used = 0;


static int g_tn = HIT_BOOL + 1;	/* hit[] �C���f�N�X */

/* �L�����N�^��LR�C���f�N�X */
static int n_lrq[11];
static struct ITREE **lrq[11];


// Variable declarations for opencl code
void cl_declare_regs(const char *base_name, int count){
	assert(base_name != NULL);
	if(!count)return;
	char buffer[2048];
	int len = sprintf(buffer, "uint32_t %s0",base_name);
	fwrite(buffer, 1, len, clfp);
	int i;
	for(i = 1; i < count; i++){
		len = sprintf(buffer, ", %s%d",base_name, i);
		fwrite(buffer, 1, len, clfp);
	}
	len = sprintf(buffer, "; \\\n",base_name, i);
	fwrite(buffer, 1, len, clfp);
}

cl_save_work_area(){
	int i = 0;
	char buffer[2048];
	for(; i <= greatest_work_area_used; i++){
		int len = sprintf(buffer, "gwork_area( %d ) = work%d; \\\n", i, i);
		fwrite(buffer, 1, len, clfp);
	}
}

/***************************************************************
 *
 *	�I�y�R�[�h��W�J
 *
 *	r	EAX, ECX, EDX, EBX
 *		SIB, EBP, ESI, EDI
 *
 */

/* ofs8(i) �������� ofs32(i) �𐶐� */
static
void
disp_rm(FILE *sfp,
		unsigned d,
		unsigned i,
		int32_t ofs)
{
  assert(i != 4);	/* SP �ł͂Ȃ� SIB �ɂȂ� */
  if (i != 5 && ofs == 0)
	{
	  /* �I�t�Z�b�g�ȗ�
		 00 ddd sss */
	  putc(0000
		   | ((d << 3) & 0070)
		   | (i & 0007),
		   sfp);
	}
  else if (-128 <= ofs && ofs <= 127)
	{
	  /* short ofs
		 01 ddd sss [ofs.b] */
	  putc(0100
		   | ((d << 3) & 0070)
		   | (i & 0007),
		   sfp);
	  putc(ofs, sfp);
	}
  else
	{
	  /* long ofs
		 10 ddd sss [ofs.l] */
	  putc(0200
		   | ((d << 3) & 0070)
		   | (i & 0007),
		   sfp);
	  fwrite(&ofs, sizeof(ofs), 1, sfp);
	}
}

#define PTR_H	1	/* CX */
#define PTR_LR	5	/* BP */
#define PTR_RL	7	/* DI */
#define PTR_T	6	/* SI */

#define OFS_H(n)	(sizeof(WS_T) * (n))
#define OFS_T(n)	(sizeof(WS_T) * ((n) - 16))

#ifdef USE_64	/* ALU 64 */

/*
 *	0x49 0xF7 11-010-ddd	not
 *	0x49 op   11-sss-ddd
 *
 *	0x4C op   rm
 *	0x4C 0x8B rm	load
 *	0x4C 0x89 rm	store
 *
 */

#define OP_MOV	0x8B
#define OP_STOR	0x89
#define OP_AND	0x23
#define	OP_OR	0x0B
#define OP_XOR	0x33

// d = d OP s;
static
void
reg_op_cl(unsigned op,
	unsigned d,
	unsigned s){
	
	assert(clfp != NULL);
	static char buffer[512];
	unsigned len = 0;
	
	switch(op){
		case OP_AND:len = sprintf(buffer, "x%d = (x%d)%s(x%d); \\\n", d, d, "&", s);break;
		case OP_ANDN:len = sprintf(buffer, "x%d = (~x%d)%s(x%d); \\\n", d, d, "&", s);break;
		case OP_OR:len = sprintf(buffer, "x%d = (x%d)%s(x%d); \\\n", d, d, "|", s);break;
		case OP_XOR:len = sprintf(buffer, "x%d = (x%d)%s(x%d); \\\n", d, d, "^", s);break;
		case OP_MOV:len = sprintf(buffer, "x%d = x%d; \\\n", d, s);break;
		default: printf("Missing opcode %d\n",op);
	}
	assert(len > 0);
	fwrite(buffer, 1, len, clfp);
}

static
void
reg_mem_cl(unsigned op,
		unsigned d,
		unsigned i,
		int ofs){
	
	assert(clfp != NULL);
	static char buffer[512];
	unsigned len = 0;
	
	ofs/=16;
	
	const char wa[128];
	if(ofs<work_area_size){
		sprintf(wa, "work%d",ofs);
	}else{
		sprintf(wa, "gwork_area( %d )",ofs);
	}
	
	switch(op){
	case OP_MOV:
		//d = i[ofs]
		switch(i){
			case PTR_LR:len = sprintf(buffer, "x%d = b%d; \\\n", d, (ofs)+16);break;
			case PTR_RL:len = sprintf(buffer, "x%d = b%d; \\\n", d, (ofs)+48);break;
			case PTR_H: len = sprintf(buffer, "x%d = %s; \\\n", d, wa); greatest_work_area_used = max(greatest_work_area_used,ofs); break;
			default: 	len = sprintf(buffer, "x%d = p%d[%d]; \\\n", d, i, ofs);break;
		}
		break;
	case OP_STOR:
		//i[ofs] = d
		switch(i){
			case PTR_LR:len = sprintf(buffer, "b%d = x%d; \\\n", ofs, d);break;
			case PTR_H: len = sprintf(buffer, "%s = x%d; \\\n", wa, d); greatest_work_area_used = max(greatest_work_area_used,ofs); break;
			default: 	len = sprintf(buffer, "p%d[%d] = x%d; \\\n", i, ofs, d);break;
		}
		break;
	case OP_AND:
		//d = d&i[ofs]
		switch(i){
			case PTR_LR:len = sprintf(buffer, "x%d = x%d&b%d; \\\n", d, d, ofs);break;
			case PTR_H: len = sprintf(buffer, "x%d = x%d&%s; \\\n", d, d, wa); greatest_work_area_used = max(greatest_work_area_used,ofs); break;
			default: 	len = sprintf(buffer, "x%d = x%d&p%d[%d]; \\\n", d, d, i, ofs);break;
		}
		break;
	case OP_OR:
		//d = d|i[ofs]
		switch(i){
			case PTR_LR:len = sprintf(buffer, "x%d = x%d|b%d; \\\n", d, d, ofs);break;
			case PTR_H: len = sprintf(buffer, "x%d = x%d|%s; \\\n", d, d, wa); greatest_work_area_used = max(greatest_work_area_used,ofs);break;
			default: 	len = sprintf(buffer, "x%d = x%d|p%d[%d]; \\\n", d, d, i, ofs);break;
		}
		break;
	case OP_XOR:
		//Exclusively used to implement NOT.
		//d = ~d
		len = sprintf(buffer, "x%d = ~x%d; \\\n", d, d);break;
		break;
	default:
		fprintf(stderr,"Runtime encountered an unexpected OPCODE %d \\\n",op);
		break;
	}
	assert(len > 0);
	fwrite(buffer, 1, len, clfp);
}

// d = ~d;
static
void
reg_not_cl(unsigned d){
	

	assert(clfp != NULL);
	static char buffer[512];
	unsigned len = 0;
	
	len = sprintf(buffer, "x%d = ~x%d; \\\n", d, d);
	assert(len > 0);
	
	fwrite(buffer, 1, len, clfp);
}

static
void
reg_op(FILE *sfp,
	   unsigned op,
	   unsigned d,
	   unsigned s)
{
  reg_op_cl(op,d,s);
  putc(0x4D, sfp);
  putc(op & 0xFD, sfp);
  /* 11 sss ddd */
  putc(0300
	   | ((s << 3) & 0070)
	   | (d & 0007),
	   sfp);
}

static
void
reg_not(FILE *sfp,
		unsigned d)
{
  reg_not_cl(d);
  putc(0x47, sfp);
  putc(0xF7, sfp);
  /* 11 010 ddd */
  putc(0320
	   | (d & 0007),
	   sfp);
}

static
void
reg_mem(FILE *sfp,
		unsigned op,
		unsigned d,
		unsigned i,
		long ofs)
{
  reg_mem_cl(op,d,i,ofs);
  putc(0x4C, sfp);
  putc(op, sfp);
  disp_rm(sfp, d, i, ofs);
}




#else	/* MMX or XMM */

#define OP_MOV	0x6F
#define OP_STOR	0x7F	
#define OP_AND	0xDB
#define OP_ANDN	0xDF
#define	OP_OR	0xEB
#define OP_XOR	0xEF


// d = d OP s;
static
void
reg_op_cl(unsigned op,
	unsigned d,
	unsigned s){
	
	assert(clfp != NULL);
	static char buffer[512];
	unsigned len = 0;
	
	switch(op){
		case OP_AND:len = sprintf(buffer, "x%d = (x%d)%s(x%d); \\\n", d, d, "&", s);break;
		case OP_ANDN:len = sprintf(buffer, "x%d = (~x%d)%s(x%d); \\\n", d, d, "&", s);break;
		case OP_OR:len = sprintf(buffer, "x%d = (x%d)%s(x%d); \\\n", d, d, "|", s);break;
		case OP_XOR:len = sprintf(buffer, "x%d = (x%d)%s(x%d); \\\n", d, d, "^", s);break;
		case OP_MOV:len = sprintf(buffer, "x%d = x%d; \\\n", d, s);break;
		default: printf("Missing opcode %d\n",op);
	}
	assert(len > 0);
	fwrite(buffer, 1, len, clfp);
}

static
void
reg_mem_cl(unsigned op,
		unsigned d,
		unsigned i,
		int ofs){
	
	assert(clfp != NULL);
	static char buffer[512];
	unsigned len = 0;
	
	ofs/=16;
	
	const char wa[128];
	if(ofs<work_area_size){
		sprintf(wa, "work%d",ofs);
	}else{
		sprintf(wa, "gwork_area( %d )",ofs);
	}
	
	switch(op){
	case OP_MOV:
		//d = i[ofs]
		switch(i){
			case PTR_LR:len = sprintf(buffer, "x%d = b%d; \\\n", d, (ofs)+16);break;
			case PTR_RL:len = sprintf(buffer, "x%d = b%d; \\\n", d, (ofs)+48);break;
			case PTR_H: len = sprintf(buffer, "x%d = %s; \\\n", d, wa);break;
			default: 	len = sprintf(buffer, "x%d = p%d[%d]; \\\n", d, i, ofs);break;
		}
		break;
	case OP_STOR:
		//i[ofs] = d
		switch(i){
			case PTR_LR:len = sprintf(buffer, "b%d = x%d; \\\n", ofs, d);break;
			case PTR_H: len = sprintf(buffer, "%s = x%d; \\\n", wa, d);break;
			default: 	len = sprintf(buffer, "p%d[%d] = x%d; \\\n", i, ofs, d);break;
		}
		break;
	case OP_AND:
		//d = d&i[ofs]
		switch(i){
			case PTR_LR:len = sprintf(buffer, "x%d = x%d&b%d; \\\n", d, d, ofs);break;
			case PTR_H: len = sprintf(buffer, "x%d = x%d&%s; \\\n", d, d, wa);break;
			default: 	len = sprintf(buffer, "x%d = x%d&p%d[%d]; \\\n", d, d, i, ofs);break;
		}
		break;
	case OP_OR:
		//d = d|i[ofs]
		switch(i){
			case PTR_LR:len = sprintf(buffer, "x%d = x%d|b%d; \\\n", d, d, ofs);break;
			case PTR_H: len = sprintf(buffer, "x%d = x%d|%s; \\\n", d, d, wa);break;
			default: 	len = sprintf(buffer, "x%d = x%d|p%d[%d]; \\\n", d, d, i, ofs);break;
		}
		break;
	case OP_XOR:
		//Exclusively used to implement NOT.
		//d = ~d
		len = sprintf(buffer, "x%d = ~x%d; \\\n", d, d);break;
		break;
	default:
		fprintf(stderr,"Runtime encountered an unexpected OPCODE %d \\\n",op);
		break;
	}
	assert(len > 0);
	fwrite(buffer, 1, len, clfp);
}

// d = ~d;
static
void
reg_not_cl(unsigned d){
	

	assert(clfp != NULL);
	static char buffer[512];
	unsigned len = 0;
	
	len = sprintf(buffer, "x%d = ~x%d; \\\n", d, d);
	assert(len > 0);
	
	fwrite(buffer, 1, len, clfp);
}

static
void
reg_op(FILE *sfp,
	   unsigned op,
	   unsigned d,
	   unsigned s)
{
  reg_op_cl(op,d,s);
#ifndef USE_MMX
  putc(0x66, sfp);
#endif
  putc(0x0F, sfp);
  putc(op, sfp);
  /* 11 ddd sss */
  putc(0300
	   | ((d << 3) & 0070)
	   | (s & 0007),
	   sfp);
}



static
void
reg_mem(FILE *sfp,
		unsigned op,
		unsigned d,
		unsigned i,
		int ofs)
{
  reg_mem_cl(op,d,i,ofs);
#ifndef USE_MMX
  putc(0x66, sfp);
#endif
  putc(0x0F, sfp);
  putc(op, sfp);
  disp_rm(sfp, d, i, ofs);
  const char *opcode;

}


/* �c�O������������̒萔�Ɖ��Z */
static
void
reg_not(FILE *sfp,
		unsigned d)
{
  reg_mem(sfp,
		  OP_XOR,
		  d,
		  PTR_T,
		  OFS_T(T_INV));
}


#endif

static
void
reg_clr(FILE *sfp,
		unsigned d)
{
  reg_op(sfp, OP_XOR, d, d);
}



/***************************************************************
 *
 *	�m�[�h�쐬
 *
 */

#define ALLOC_LEAP	64	/* 2**n */

static
struct ITREE *
synth_mknod(enum NODE_ID id)
{
  struct ITREE *node;

  node = calloc(1, sizeof(struct ITREE));
  assert(node != NULL);
  node->opcode = id;
  node->tn = -1;
  node->n_alloc = sizeof(node->tops) / sizeof(*node->tops);
  node->ops = node->tops;
  return node;
}

struct ITREE *
synth_make_not(void)
{
  return synth_mknod(NI_NOT);
}

struct ITREE *
synth_make_and(void)
{
  return synth_mknod(NI_AND);
}

struct ITREE *
synth_make_or(void)
{
  return synth_mknod(NI_OR);
}

struct ITREE *
synth_make_xor(void)
{
  return synth_mknod(NI_XOR);
}

struct ITREE *
synth_add(struct ITREE *parent, ...)
{
  struct ITREE *node;
  va_list ap;
  va_start(ap, parent);
  while ((node = va_arg(ap, struct ITREE *)) != NULL)
	{
	  /* expand allocation */
	  if (parent->n_alloc == parent->n_ops)
		{
		  assert(IS_POWER2(ALLOC_LEAP));
		  parent->n_alloc = (parent->n_alloc + ALLOC_LEAP) & -ALLOC_LEAP;
		  assert(parent->n_alloc > parent->n_ops);
		  if (parent->ops == parent->tops)
			{
			  parent->ops = malloc(parent->n_alloc * sizeof(*parent->ops));
			  memcpy(parent->ops, parent->tops, sizeof(parent->tops));
			}
		  else
			parent->ops = realloc(parent->ops,
								  parent->n_alloc * sizeof(*parent->ops));
		}
	  parent->ops[parent->n_ops++] = node;
	  node->ref++;
	}
  va_end(ap);
  return parent;
}

/***************************************************************
 *
 *	�m�[�h���������
 *	�R�h���͉�����Ȃ�
 *
 */

static
void
synth_free(struct ITREE *node)
{
#if DEBUG>=2
  printf("free: node=%p opcode=%d ops=%d\n",
		 node,
		 node->opcode,
		 node->n_ops);
#endif

  if (node->ops != NULL
	  && node->ops != node->tops)
	free(node->ops);

  /* �������g���폜 */
  free(node);
}

/***************************************************************
 *
 *	�m�[�h��؂藣��
 *	ref == 0 �ɂȂ����烁����������폜���č폜����Ԃ�
 *
 */

int
synth_unlink(struct ITREE *node)
{
  int i;
  int n = 0;

  assert(node->ref > 0);
  if (--node->ref > 0)
	return 0;

  assert(node->dict == NULL);

  /* �}��؂藎�Ƃ� */
  for (i = 0; i < node->n_ops; i++)
	n += synth_unlink(node->ops[i]);

  /* LR �̏ꍇ�A���X�g����؂藣�� */
  if (node->opcode == NI_LR)
	{
	  for (i = 0; i < n_lrq[node->lrn]; i++)
	  if (lrq[node->lrn][i] == node)
		{
		  memmove(&lrq[node->lrn][i],
				  &lrq[node->lrn][i + 1],
				  (--n_lrq[node->lrn] - i) * sizeof(*lrq[0][0]));
		  goto deleted;
		}
	  assert(!"XXX orphan node");
	deleted:
	  ;
	}

  /* �������g���폜 */
  synth_free(node);

  return n + 1;
}

/***************************************************************
 *
 *	Register LR node
 *
 */

struct ITREE *
synth_make_lr(uint64_t bmp, int lrn)
{
  struct ITREE *node;
  int qa, qb;

  assert(1 <= lrn && lrn <= 10);
  assert(bmp != 0);

  /* End mask */
  if (lrn == 10)
	{
	  bmp &= 0x1111111111111111ULL;

	  /* Optimized for the following
		 Set all bits of the node to always true */
	  if (!(~bmp & 0x1111111111111111ULL))
		bmp = 0xFFFFFFFFFFFFFFFFULL;
	}

  assert(bmp != 0);

  /* Return if it's already in the cache
	 Because this version is binary search <something something>(lel) */
  for (qa = 0, qb = n_lrq[lrn];
	   qb > 0;
	   qb >>= 1)
	{
	  int q = qa + (qb >> 1);
	  assert(lrq[lrn] != NULL);
	  assert(lrq[lrn][q] != NULL);
	  assert(lrq[lrn][q]->opcode == NI_LR);
	  if (bmp == lrq[lrn][q]->bmp)
		return lrq[lrn][q];
	  else if (bmp > lrq[lrn][q]->bmp)
		{
		  qa = q + 1;
		  qb--;
		}
	}

#if DEBUG>=1
  if (n_lrq[lrn] == 0)
	printf("%2d:%3d/%3d=---------------- %08X%08X\n",
		   lrn,
		   qa, n_lrq[lrn],
		   (unsigned)(bmp >> 32),
		   (unsigned)bmp);

  if (qa < n_lrq[lrn])
	printf("%2d:%3d/%3d=%08X%08X %08X%08X\n",
		   lrn,
		   qa, n_lrq[lrn],
		   (unsigned)(lrq[lrn][qa]->bmp >> 32),
		   (unsigned)(lrq[lrn][qa]->bmp),
		   (unsigned)(bmp >> 32),
		   (unsigned)bmp);
#endif

  node = calloc(1, sizeof(struct ITREE));
  assert(node != NULL);
  node->opcode = NI_LR;
  node->tn = -1;
  node->lrn = lrn;
  node->bmp = bmp;

  /* Insert new node into the cache register */
  if (lrq[lrn] == NULL)
	{
	  assert(n_lrq[lrn] == 0);
	  lrq[lrn] = malloc(sizeof(*lrq[lrn]));
	  n_lrq[lrn]++;
	  assert(qa == 0);
	}
  else
	{
	  int n = n_lrq[lrn];
	  lrq[lrn] = realloc(lrq[lrn], (n + 1) * sizeof(*lrq[lrn]));
	  memmove(&lrq[lrn][qa + 1], &lrq[lrn][qa], (n - qa) * sizeof(*lrq[lrn]));
	  n_lrq[lrn] = n + 1;
	}
  assert(lrq[lrn] != NULL);
  assert(qa < n_lrq[lrn]);
  lrq[lrn][qa] = node;

  return node;
}

/***************************************************************
 *
 *	Put a word in the dictionary
 *
 */

struct ITREE *synth_make_dict(int pos,
							  struct DICT_CHAR const *word)
{
  struct WDICT *pd;
  struct ITREE *node = NULL;

  /* Put a word in the dictionary
	 if NULL is returned the dictionary is already made */
  pd = wdict_add(g_tn, pos, word);

  /* Create a blank dictionary */
  if (pd != NULL)
	{
	  node = synth_mknod(NI_AND);
	  assert(node != NULL);
	  node->dict = pd;
	  node->tn = g_tn++;
	  wdict_setnode(pd, node);
	  return node;
	}
  else
	{
	  /* If the dictionary was already made, just return NULL */
	  return NULL;
	}
}

/***************************************************************
 *
 *	�؂��_���v; for debug
 *
 */

static
void
synth_dump(int lv, struct ITREE const *node)
{
  int i;
  char const *t = &"\t\t\t\t\t\t\t\t\t\t"[10 - lv];
  static char const node_name[][4] = 
	{
	  "LR",	/* -256 */
	  "SEQ",	/* XXX */
	  "NOT",
	  "AND",
	  "OR",
	  "XOR",
	  "NAN",
	};

  if (!node)
	return;

  if (NI_LR <= node->opcode && node->opcode < NI_MAX)
	printf("%s%s[%s] (n=%d) ref=%d tn=%d %p\n%s{\n",
		   t,
		   (node->dict != NULL ? "[DIC]" : ""),
		   node_name[node->opcode - NI_LR],
		   node->n_ops, node->ref,
		   node->tn,
		   node,
		   t);
  else
	printf("%snode=%d (n=%d) ref=%d dict=%p %p\n%s{\n",
		   t,
		   node->opcode, node->n_ops, node->ref,
		   node->dict,
		   node,
		   t);

  if (node->dict != NULL)
	  wdict_dump(lv, node->dict);

  if (node->opcode == NI_LR)
	{
	  int cn = popcnt64(node->bmp);
	  printf("%s\t%2d:", 
			 t,
			 node->lrn);
	  if (cn > 14)
		printf(F016LLX"\t(%2d)\n",
			   A016LLX(node->bmp),
			   cn);
	  else
		{
		  putchar('[');
		  for (i = 0; i < 64; i++)
			if (node->bmp & (1ULL << i))
			  putchar(C64[i]);
		  printf("]\t(%2d)\n", cn);
		}
	}
  else for (i = 0; i < node->n_ops; i++)
	{
	  printf("%s%d:\n", t, i);
	  synth_dump(lv + 1, node->ops[i]);
	}
  printf("%s}\n", t);
}

/***************************************************************
 *
 *	�𒼂ȃ��W�X�^���蓖��
 *
 */

#define REG_RELOAD	0	/* �t���Ă��n�̍œK�͂ނ���L�Q */
#define N_INFLIGHT	1	/* Number of simultaneous operations */

#if defined(USE_64_XMM)
#define NREGS	16
#else
#define NREGS	8
#endif




static struct ITREE *a_regs[NREGS];
static int g_reggen, a_reggen[NREGS];

static
int
synth_reg_find(struct ITREE *node)
{
  int i;
  assert(node != NULL);
  assert(node->ref > 0);
  for (i = 0; i < NREGS; i++)
	if (a_regs[i]
		&& a_regs[i] == node)
	  {
		a_reggen[i] = g_reggen++;
		return i;
	  }
  return -1;
}



/* �������W�X�^��T��(��������) */
static
int
synth_reg_alloc_vacant(void)
{
  int i;
  
  for (i = 0; i < NREGS; i++)
	if (a_regs[i] == NULL
		|| a_regs[i]->ref == 0)
	  return i;

  return -1;
}

static
int
synth_reg_alloc(struct ITREE *node)
{
  int i;
  int g, r;

  /* ���łɊ��蓖�Ă��Ă���΂����Ԃ� */
  i = synth_reg_find(node);
  if (i >= 0)
	return i;
  /* �A�L���W�X�^��T�� */
  r = -1;
  g = INT_MAX;
  for (i = 0; i < NREGS; i++)
	if (a_regs[i] != NULL
		&& a_regs[i]->ref > 0)
	  {
		/* ��߂��Ă��� */
		if (g > a_reggen[i]
			&& a_regs[i]->tn >= 0)
		  {
			g = a_reggen[i];
			r = i;
		  }
	  }
	else
	  {
		a_regs[i] = node;
		a_reggen[i] = g_reggen++;
		return i;
	  }

  /* XXX �ǂꂩ�ЂƂ�Ҕ����āA�����ɂ����Ɋ��蓖�Ă���̂͏������� */
  assert(r >= 0);
  assert(a_regs[r]->tn >= 0);
  a_regs[r] = node;
  a_reggen[r] = g_reggen++;
  return r;
}

/***************************************************************
 *
 *	���ߗ�𐶐�
 *
 */

struct LRX
{
  struct ITREE *node;
  int mtn;
  int otn;
  int src;
  struct QMX *xp;
  unsigned mtm;
};

#define MTM_NOT	0x100U

static
int
cmp_lrx(void const *pa, void const *pb)
{
  struct LRX const *a = pa;
  struct LRX const *b = pb;
  int r;
  assert(a->node != NULL);
  assert(b->node != NULL);
  if ((r = a->mtn - b->mtn)
	  || (r = a->mtm - b->mtm)
	  || (r = (int)popcnt64(a->node->bmp) - (int)popcnt64(b->node->bmp))
	  || (r = (int)(a->node->bmp >> 32) - (int)(b->node->bmp >> 32)))
	return r;
  return (int)a->node->bmp - (int)b->node->bmp;
}

#if SEQ
#define LRR(r,pos) (((r) + 6 * (pos)) & (NREGS - 1))
#else
#define LRR(r,pos) (r)
#endif

static int g_seqn = -1;

static
void
synth_load_lr(FILE *sfp, int pos, int i)
{
  int o = tr_fp[6 * pos + (5 - i)];
  if (o < 32){
	reg_mem(sfp,
			OP_MOV,
			LRR(i, pos),
			PTR_LR,
			sizeof(WS_T) * (o - 16));
  }
  else if (o < 64){
	reg_mem(sfp,
			OP_MOV,
			LRR(i, pos),
			PTR_RL,
			sizeof(WS_T) * (o - 32 - 16));
	}
}


/* �A���m�c���[��g�ݏグ�� */
static
struct ITREE *
synth_mkseq(void)
{
  int i, j;
  struct ITREE *t0;
  struct ITREE *seqnode[9];

  if (g_seqn < 0)
	{
	  g_seqn = g_tn;
	  g_tn += 9;
	}

  for (i = 0; i < 9; i++)
	{
	  seqnode[i] = synth_mknod(NI_SEQ);
	  seqnode[i]->tn = g_seqn + i;
	}

  t0 = synth_mknod(NI_NAND);
  for (i = 0; i < 10 + 1 - SEQ; i++)
	{
	  struct ITREE *t1 = synth_make_or();
	  for (j = 0; j < SEQ - 1; j++)
		{
		  assert(i + j < 9);
		  synth_add(t1, seqnode[i + j], NULL);
		}
	  synth_add(t0, t1, NULL);
	}

  return t0;
}

static
int
synth_assemble_lr(FILE *sfp, int pos, struct ITREE **nodea, int noden)
{
  int i;
  uint64_t m;
  unsigned regs_loaded = 0;
  int rd;
  int recon;
  struct LRX *lrx;
  struct QMX (*qmx)[QMX_LEN + 1];
  int nts_total = 0;
  int nts_reduced = 0;
  assert(noden <= 10000);
  debug_eprintf(1, "ASSEMBLE! (noden=%d)\n", noden);
  recon = 0;

  /* XXX �C�x�� */
  for (i = 0; i < NREGS; i++)
	a_regs[i] = NULL;

#if SEQ > 0
  if (pos >= 1)	/* 11�A�Ή��ł͂Ȃ�! */
	{
	  /* �������� */
	  i = 0;
	  if (pos == 10)
		{
		  reg_op(sfp, OP_OR, LRR(1, pos - 1), LRR(0, pos - 1));
		  i = 2;
		}

	  /* adhoc �ȘA���m���o */
	  for (/*nil*/; i < 6; i++)
		{
		  synth_load_lr(sfp, pos, i);
		  regs_loaded |= (1 << i);
		  if (pos >= 2)
			{
			  reg_op(sfp, OP_XOR, LRR(i, pos - 1), LRR(i, pos));
			  if (i > 0)
				reg_op(sfp, OP_OR, LRR(i, pos - 1), LRR(i - 1, pos - 1));
			}
		}

	  /* �A���m�����ꏊ */
	  if (pos >= 2)
		{
		  assert(g_seqn >= 0);
		  reg_mem(sfp,
				  OP_STOR,
				  LRR(5, pos - 1),
				  PTR_H,
				  OFS_H(g_seqn + pos - 2));
		}
	}
#endif

  if (noden == 0)
	return 0;

  assert(nodea != NULL);

  m = (pos < 10
	   ? 0x0000000000000000ULL
	   : 0xEEEEEEEEEEEEEEEEULL);

  /* ���[�N�G���A */
  lrx = calloc(noden, sizeof(*lrx));
  assert(lrx != NULL);
  qmx = calloc(noden, sizeof(*qmx));
  assert(qmx != NULL);

  for (i = 0; i < noden; i++)
	{
	  int j, k;
	  unsigned opo, opa;
	  struct ITREE *node;
	  struct QMX *xp;
	  struct QMX const *px;

	  for (j = i; j < noden; j++)
		{
		  /* �ŏ��̃��[�v�Œʂ�͂� */
		  if (lrx[j].node == NULL)
			{
			  lrx[j].node = nodea[j];
			  lrx[j].xp = qmx[j];
			  lrx[j].mtn  = simplify_qm(lrx[j].xp,
										lrx[j].node->bmp,
										m);
			  lrx[j].otn = lrx[j].mtn;
			  lrx[j].mtm  = OP_MOV;
			  lrx[j].src = -256;	/* �����ă}�b�`���Ȃ� */
			}
		  if (recon && lrx[j].mtn == INT_MAX)
			{
			  lrx[j].mtn  = simplify_qm(lrx[j].xp,
										lrx[j].node->bmp,
										m);
			  lrx[j].otn = lrx[j].mtn;
			  assert(lrx[j].mtm == OP_MOV);
			}

		  /* �핢�̔핢�����߂� */
		  for (k = 0; k < i; k++)
			{
			  int ttn;
			  struct QMX txp[QMX_LEN + 1];

			  if (!recon && k < i - 2)	/* �v�Z����K�v�i�V */
				continue;

			  if ((lrx[k].node->bmp & lrx[j].node->bmp) == lrx[k].node->bmp
				  && (ttn = simplify_qm(txp,
										lrx[j].node->bmp,
										m | lrx[k].node->bmp)) < INT_MAX)
				{
				  /* OR */
				  if (k != i - 1)	/* ���O�̂��̂łȂ����1�i�R�X�g�� */
					ttn++;
				  if (lrx[j].mtn > ttn)
					{
					  lrx[j].mtn = ttn;
					  lrx[j].mtm = OP_OR;
					  lrx[j].src = k;
					  memcpy(lrx[j].xp, txp, sizeof(txp));
					}
				}

			  if ((lrx[k].node->bmp & lrx[j].node->bmp) == lrx[j].node->bmp
				  && (ttn = simplify_qm(txp,
										lrx[j].node->bmp,
										m | ~lrx[k].node->bmp)) < INT_MAX)
				{
				  /* AND */
				  ttn++;	/* ���[�h�̕��Q�� */
				  if (lrx[j].mtn > ttn)
					{
					  lrx[j].mtn = ttn;
					  lrx[j].mtm = OP_AND;
					  lrx[j].src = k;
					  memcpy(lrx[j].xp, txp, sizeof(txp));
					}
				}

			  if ((~lrx[k].node->bmp & lrx[j].node->bmp) == ~lrx[k].node->bmp
				  && (ttn = simplify_qm(txp,
										lrx[j].node->bmp,
										m | ~lrx[k].node->bmp)) < INT_MAX)
				{
				  /* NOT OR */
				  if (k != i - 1)	/* ���O�̂��̂łȂ����1�i�R�X�g�� */
					ttn++;
				  ttn += 1;	/* NOT */
				  if (lrx[j].mtn > ttn)
					{
					  lrx[j].mtn = ttn;
					  lrx[j].mtm = OP_OR | MTM_NOT;
					  lrx[j].src = k;
					  memcpy(lrx[j].xp, txp, sizeof(txp));
					}
				}

			  if ((~lrx[k].node->bmp & lrx[j].node->bmp) == lrx[j].node->bmp
				  && (ttn = simplify_qm(txp,
										lrx[j].node->bmp,
										m | lrx[k].node->bmp)) < INT_MAX)
				{
				  /* NOT AND (not ANDN) */
				  ttn += 3;	/* LOAD, NOT, AND */
				  if (lrx[j].mtn > ttn)
					{
					  lrx[j].mtn = ttn;
					  lrx[j].mtm = OP_AND | MTM_NOT;
					  lrx[j].src = k;
					  memcpy(lrx[j].xp, txp, sizeof(txp));
					}
				}
			}
		}

	  /* �������Ƀ\�[�g
		 �擪�ɗ������m��I�o���邱�ƂɂȂ� */
	  recon = 0;
	  qsort(&lrx[i], noden - i, sizeof(*lrx), cmp_lrx);

	  assert(lrx[i].mtn != INT_MAX);

#if DEBUG>=1
	  fprintf(stderr,
			  "%4d/%4d:n=%3d/%3d(mt=%c%02X:src=%4d) %08X%08X\n",
			  i, (noden - i),
			  lrx[i].mtn,
			  lrx[i].otn,
			  (lrx[i].mtm & MTM_NOT ? '!' : ' '),
			  lrx[i].mtm & 0xFF,
			  lrx[i].src,
			  (unsigned)(lrx[i].node->bmp >> 32),
			  (unsigned)lrx[i].node->bmp);
#endif
	  assert(lrx[i].mtn >= 0);

	  /* ���v */
	  nts_total += lrx[i].mtn;
	  if (lrx[i].mtm && lrx[i].otn < INT_MAX)
		nts_reduced += lrx[i].otn - lrx[i].mtn;

	  /* ���ߗ�̐��� */
	  node = lrx[i].node;
	  xp = lrx[i].xp;

	  /* ���[�N�G���A���蓖�� */
	  node->tn = g_tn++;

	  switch (lrx[i].mtm)
		{
		case OP_OR | MTM_NOT:
		case OP_OR:	rd = 6; opo = OP_OR;	break;
		default:	rd = 7; opo = OP_MOV;	break;
		}

	  /* ���o�̍��𗘗p���ĉ��Z����΂��� */
	  if (opo != OP_MOV && lrx[i].src != i - 1)
		reg_mem(sfp,
				OP_MOV,
				LRR(7, pos),
				PTR_H,
				OFS_H(lrx[lrx[i].src].node->tn));

	  if (lrx[i].mtm == (OP_OR | MTM_NOT))
		reg_not(sfp, LRR(7, pos));

	  if (!(node->bmp & ~m)
		  || !~(node->bmp | m))
		{
		  fprintf(stderr,
				  "�����������[ %08X%08X\n",
				  (unsigned)(node->bmp >> 32),
				  (unsigned)node->bmp);
		  assert(!!(node->bmp & ~m));
		  assert(!!~(node->bmp | m));
		}

	  /* ���k���ꂽ����W�J */
	  for (px = xp; px->m; px++)
		{
		  int j;
		  unsigned mi, pi;

		  opa = OP_MOV;

		  /* �⍀; !a!b..!f => !(a+b+...+f) */
		  mi = px->m & ~px->n;
		  for (j = 0; j < 6; j++)
			if (mi & (1 << j))
			  {
				if (!(regs_loaded & (1 << j)))
				  {
					synth_load_lr(sfp, pos, j);
					regs_loaded |= (1 << j);
				  }
				reg_op(sfp, opa, LRR(rd, pos), LRR(j, pos));
				opa = OP_OR;
			  }

#ifdef OP_ANDN
		  if (opa == OP_OR)
			opa = OP_ANDN;
#else
		  if (opa == OP_OR)
			{
			  reg_not(sfp, LRR(rd, pos));
			  opa = OP_AND;
			}
#endif
		  /* �c��̐ύ� */
		  pi = px->m & px->n;
		  for (j = 0; j < 6; j++)
			if (pi & (1 << j))
			  {
				if (!(regs_loaded & (1 << j)))
				  {
					synth_load_lr(sfp, pos, j);
					regs_loaded |= (1 << j);
				  }
				reg_op(sfp, opa, LRR(rd, pos), LRR(j, pos));
				opa = OP_AND;
			  }

		  /* �⍀�����������ꍇ�̏��� */
#ifdef OP_ANDN
		  if (opa == OP_ANDN)
			reg_not(sfp, LRR(rd, pos));
#endif

		  /* �a���ɒǉ� */
		  if (rd != 7)
			reg_op(sfp, opo, LRR(7, pos), LRR(rd, pos));
		  rd = 6;
		  opo = OP_OR;
		}

	  if (opo == OP_MOV)
		{
		  /* �Ϙa������������Ȃ������ꍇ */
		  assert(lrx[i].mtm != OP_MOV);
		  assert(lrx[i].mtm != OP_OR);
		  assert(lrx[i].src >= 0);
		  if (lrx[i].src != i - 1)
			reg_mem(sfp,
					OP_MOV,
					LRR(7, pos),
					PTR_H,
					OFS_H(lrx[lrx[i].src].node->tn));
		  if (lrx[i].mtm & MTM_NOT)
			reg_not(sfp, LRR(7, pos));
		}
	  else switch (lrx[i].mtm)	  /* ���o���Ƃ̉��Z */
		{
		case OP_AND | MTM_NOT:
		  reg_mem(sfp,
				  OP_MOV,
				  LRR(6, pos),
				  PTR_H,
				  OFS_H(lrx[lrx[i].src].node->tn));
		  reg_not(sfp, LRR(6, pos));
		  reg_op(sfp, OP_AND, LRR(7, pos), LRR(6, pos));
		  break;
		case OP_AND:
		  reg_mem(sfp,
				  lrx[i].mtm,
				  LRR(7, pos),
				  PTR_H,
				  OFS_H(lrx[lrx[i].src].node->tn));
		  break;
		}

	  /* ���ʂ̊i�[ */
	  reg_mem(sfp,
			  OP_STOR,
			  LRR(7, pos),
			  PTR_H,
			  OFS_H(node->tn));

	  a_regs[LRR(7, pos)] = node;	/* XXX �C�x�� */
	}
#if DEBUG>=1
  {
	static int tt, tn;
	tt += nts_total;
	tn += nts_reduced;
	fprintf(stderr,
			"term=%d (reduced %d) total:%d%%(%d reduced=%d)\n",
			nts_total, nts_reduced,
			100 * tt / (tt + tn),
			tt, tn);
  }
#endif

  free(lrx);
  free(qmx);

  return 0;
}

static
int
synth_assemble(FILE *sfp, struct ITREE *node)
{
  int i;
  int m;
  int rd[N_INFLIGHT];
  unsigned op[N_INFLIGHT];
  unsigned opx;
  int f_dict = 0;

  assert(node != NULL);
  assert(node->dict != NULL || node->tn < 0);
  assert(node->n_ops > 0);
  assert(node->ops != NULL);

  switch (node->opcode)
	{
	default: assert(!"XXX not supported");
	case NI_AND: opx = OP_AND; break;
	case NI_OR:  opx = OP_OR;  break;
	case NI_XOR: opx = OP_XOR; break;
	case NI_NAND:opx = OP_AND; break;
	}

  /* ���܂��Ȃ� */
  node->ref++;

  /* ���W�X�^���蓖�Ă̏����� */
  for (m = 0; m < N_INFLIGHT; m++)
	{
	  op[m] = OP_MOV;
	  rd[m] = -1;
	}

  /* �}�������W�߂� */
  m = 0;
  for (i = 0; i < node->n_ops; i++)
	{
	  struct ITREE *leaf = node->ops[i];
	  int rs;

	  assert(leaf != NULL);
	  assert(leaf->ref > 0);

	  /* �����Q�ɓ˓�����O�ɁA���ʂ� HIT_BOOL �ɏ������߂�
		 ���̂ւ�̏����͂����ւ�A�h�z�b�N
		 �Ȃ̂� INFLIGHT = 1 �ł��肢��! */
	  if (!f_dict
		  && leaf->dict != NULL)
		{
		  int td;
		  if (i > 0
			  && node->ops[i - 1]->dict == NULL)
			{
			  assert(rd[m] >= 0);
			  td = rd[m];
			}
		  else
			{
			  /* �v�N���A */
			  td = synth_reg_alloc_vacant();
			  assert(td >= 0);	/* �����ŋ󂢂ĂȂ��n�Y���Ȃ���ȁc */
			  reg_clr(sfp, td);
			}
		  reg_mem(sfp,
				  OP_STOR,
				  td,
				  PTR_H,
				  OFS_H(HIT_BOOL));
		  f_dict = 1;
		}

	  /* �}�𐶐�
		 DICT �͐�Ƀ��[�N������t�����Ă��� */
	  if (leaf->dict != NULL)
		synth_assemble(sfp, leaf);

	  rs = (leaf->tn >= 0
			? synth_reg_find(leaf)
			: synth_assemble(sfp, leaf));

	  if (op[m] == OP_MOV)
		{
		  assert(rd[m] < 0);
		  if (rs >= 0)
			{
			  assert(a_regs[rs] != NULL);
			  assert(a_regs[rs] == leaf);
			  if (a_regs[rs]->ref > 1)
				{
				  /* ���Q�Ƃ���Ă��郌�W�X�^�̓R�s�[ */
				  rd[m] = synth_reg_alloc(node);
				  assert(rd[m] >= 0);
				  reg_op(sfp, OP_MOV, rd[m], rs);
				}
			  else
				{
				  /* �Q�Ƃ���Ȃ��Ȃ����̂ŏ����� */
				  rd[m] = rs;
				  a_regs[rd[m]] = node;
				}
			}
		  else
			{
			  /* ���W�X�^�Ɋ��蓖�Ă��Ă��Ȃ��ꍇ���[�h */
			  assert(leaf->tn >= 0);
			  rs = synth_reg_alloc(leaf);
			  reg_mem(sfp,
					  op[m],
					  rs,
					  PTR_H,
					  OFS_H(leaf->tn));
#if REG_RELOAD
			  if (leaf->ref > 1)
				{
				  /* �R�s�[�Ƃ��� */
				  rd[m] = synth_reg_alloc(node);
				  reg_op(sfp, OP_MOV, rd[m], rs);
				}
			  else
#endif
				{
				  /* �������č\��Ȃ� */
				  a_regs[rd[m] = rs] = node;
				}
			}

		  /* ������� MOV �ł͂Ȃ� */
		  assert(rd[m] >= 0);
		  op[m] = opx;
		}
	  else
		{
		  assert(rs >= 0 || leaf->tn >= 0);

		  /* ���Ƃ̂��Ƃ��l����?? �������񃌃W�X�^���蓖�� */
#if REG_RELOAD
		  if (rs < 0 && leaf->ref > 1)
			{
			  rs = synth_reg_alloc(leaf);
			  reg_mem(sfp,
					  op[m],
					  rs,
					  PTR_H,
					  OFS_H(leaf->tn));
			}
#endif

		  if (rs >= 0)
			reg_op(sfp, op[m], rd[m], rs);
		  else
			reg_mem(sfp,
					op[m],
					rd[m],
					PTR_H,
					OFS_H(leaf->tn));
		}

	  /* �Q�Ɛ������炷 */
	  leaf->ref--;

	  /* �z�� */
	  if (++m >= N_INFLIGHT)
		m = 0;
	}

  /* �󂾂�����[���ɂ��� */
  if (op[0] == OP_MOV)
	{
	  rd[0] = synth_reg_alloc(node);
	  reg_clr(sfp, rd[0]);
	}
  else
	for (i = 1; i < N_INFLIGHT; i++)
	  {
		if (op[i] != OP_MOV)
		  reg_op(sfp, op[i], rd[0], rd[i]);
		if (rd[i] >= 0)
		  a_regs[rd[i]] = NULL;
	  }

  /* NAND ���� */
  if (node->opcode == NI_NAND)
	reg_not(sfp, rd[0]);

  /* �����͕K�����ʂ��c���K�v���� */
  if (node->dict != NULL)
	assert(node->tn >= 0);

  /* AND ���̋L�^ */
  if (node->opcode == NI_OR
	  && node->dict == NULL
	  && !f_dict)
	reg_mem(sfp,
			OP_STOR,
			rd[0],
			PTR_H,
			OFS_H(HIT_BOOL));

  /* �X�g�A */
  if (node->tn >= 0)
	reg_mem(sfp,
			OP_STOR,
			rd[0],
			PTR_H,
			OFS_H(node->tn));

  /* ���܂��Ȃ������ */
  node->ref--;

  return rd[0];
}

/***************************************************************
 *
 *	�؂��v�`�œK�����Ă݂�e�X�g
 *	�œK���ɂ����ʂ�Ԃ�
 *
 */

static
int
cmp_node(void const *pa, void const *pb)
{
  int r;
  struct ITREE *a = *(struct ITREE **)pa;
  struct ITREE *b = *(struct ITREE **)pb;
  if ((r = (!!a->dict) - (!!b->dict))
	  || (r = a->tn - b->tn)
	  || (r = b->opcode - a->opcode)
	  || (r = (char const *)pa - (char const *)pb))	/* ���������Ȃ����� */
	return r;

  assert(!"XXX notreached");
  return 0;
}

static
int
synth_optimize(struct ITREE *node)
{
  int neff = 0;
  int i, j;
  int sort_needed = 0;

  /* Let's optimize the branch destination! */
  for (i = 0; i < node->n_ops; i++)
	{
	  struct ITREE *child = node->ops[i];

	  /* Ahead, the challenge of optimizing a branch */
	  neff += synth_optimize(child);

	  /* Drop meaningless nodes */
	  if (child->opcode == NI_LR
		  && ((node->opcode == NI_AND && !~child->bmp)
			  || (node->opcode == NI_OR && !child->bmp)))
		{
		  debug_eprintf(1, "op=%d child="F016LLX" ~child="F016LLX" !~child=%d !!~child=%d\n",
						node->opcode,
						A016LLX(child->bmp),
						A016LLX(~child->bmp),
						!~child->bmp,
						!!~child->bmp);

		  neff += 1 + synth_unlink(child);
		  memmove(&node->ops[i],
				  &node->ops[i + 1],
				  (--node->n_ops - i) * sizeof(*node->ops));
		  i--; goto redo_i;
		}

	  /* If a node points to itself as a child it can be removed */
	  for (j = 0; j < i; j++)
		if (node->ops[j] == child)
		  {
			child->ref--;
			assert(child->ref >= 1);
			memmove(&node->ops[i],
					&node->ops[i + 1],
					(--node->n_ops - i) * sizeof(node->ops[i]));
			neff++;
			i--; goto redo_i;
		  }

	  /* identical operations can be fused */
	  if (child->opcode == node->opcode
		  && child->tn < 0
		  && child->ref == 1)
		{
		  assert(child->dict == NULL);
		  memmove(&node->ops[i],
				  &node->ops[i + 1],
				  (--node->n_ops - i) * sizeof(node->ops[i]));
		  if (node->n_alloc < node->n_ops + child->n_ops)
			{
			  node->n_alloc = 3 * (node->n_ops + child->n_ops) / 2;
			  if (node->ops == node->tops)
				{
				  node->ops = malloc(node->n_alloc * sizeof(node->ops[0]));
				  memcpy(node->ops, node->tops, sizeof(node->tops));
				}
			  else
				node->ops = realloc(node->ops,
									node->n_alloc * sizeof(node->ops[0]));
			}
		  memcpy(&node->ops[node->n_ops],
				 &child->ops[0],
				 child->n_ops * sizeof(child->ops[0]));
		  node->n_ops += child->n_ops;
		  synth_free(child);
		  neff++;
		  i--; goto redo_i;
		}

	  /* If a child node has only one child node,
		 Pull that node up and drop the intermediary */
	  if (child->n_ops == 1
		  && !(child->opcode == NI_NAND
			   || child->opcode == NI_XOR)
		  && child->ref == 1)
		{
		  assert(child->tn < 0);
		  assert(child->dict == NULL);
		  node->ops[i] = child->ops[0];
		  child->n_ops--;
		  neff += 1 + synth_unlink(child);
		  i--; goto redo_i;
		}

	  if (i > 0
		  && node->ops[i - 1]->dict != NULL
		  && node->ops[i]->dict == NULL)
		sort_needed++;

	redo_i:
	  ;
	}

  /* Sort so that AND->DICT */
  if (sort_needed)
	qsort(node->ops, node->n_ops, sizeof(*node->ops), cmp_node);

  return neff;
}

/***************************************************************
 *
 *	Generate a sequence of instructions from the tree
 *	Returns the results of generating the required work area size
 *
 */

int
synth_synthesize(FILE *sfp,
				 struct ITREE *node)
{
  clfp = fopen("cmp.cl","w");
  assert(clfp!=NULL);
  int i;
  int r;
  off_t olr, ot;

  /* Expand the dictionary */
  if (node){
	fprintf(stderr, "Setup...");
	wdict_setup_lr();
	fprintf(stderr, " done\n");
  }
  else{
	wdict_setup_lr();
  }

  if (node)
	{
#if SEQ > 0
	  fprintf(stderr, "SEQ setup... ");
	  synth_add(node, synth_mkseq(), NULL);
	  fprintf(stderr, " done\n");
#endif

	  fprintf(stderr, "Optimizing a little...");
#if DEBUG>=2
	  synth_dump(0, node);
#endif
	  r = synth_optimize(node);
	  fprintf(stderr, " done\n");
	  if (r > 0)
		fprintf(stderr, "There was %d waste.\n", r);
	}
  else
  {
	  fprintf(stderr,
			  "* Target.txt is required to make a search.\n"
			  "  Entered pure benchmark mode(%d bits)\n"
			  "  Only looking for the trips with %d identical characters\n",
			  1 << N_STRIDE, SEQ);
	  node = synth_mkseq();
	  fprintf(stderr, "Optimizing a little...");
	  r = synth_optimize(node);
	  fprintf(stderr, " done\n");
  }

#if DEBUG>=1
  synth_dump(0, node);
#endif

  /* LR Generate a sequence of instructions */
  olr = ftell(sfp);
  if (node)
	{
	  fprintf(stderr, "Generating comparison code...");
	  fprintf(clfp, "//LR assembly\n#define COMPARISON \\\n");
	  
	  cl_declare_regs("work",128);
	  
	  for (i = 0; i < 11; i++)
		synth_assemble_lr(sfp,i, lrq[i], n_lrq[i]);
	  fprintf(stderr, " done\n");
	}
#if DEBUG>=1
  ot = ftell(sfp);
  olr = ot - olr;
  fprintf(stderr, "0x%08X(%d): LR size\n", (unsigned)olr, (unsigned)olr);
#endif

  /* Generate the rest of the tree traversal instructions */
  if (node)
	{
	  fprintf(stderr, "Doing assembly\n");
	  fprintf(clfp, "\n//Assembly\n#define ASSEMBLY \\\n");
	  r = synth_assemble(sfp, node);
	}

  /* Store the final result */
  reg_mem(sfp,
		  OP_STOR,
		  r,
		  PTR_H,
		  OFS_H(HIT_ANY));
  
  cl_save_work_area();
  
#if DEBUG>=1
  ot = ftell(sfp) - ot;
  fprintf(stderr,
		  "0x%08X(%d): TR size\ng_tn=%d g_reggen=%d\n",
		  (unsigned)ot, (unsigned)ot,
		  g_tn, g_reggen);
#endif
  fclose(clfp);
  return g_tn;
}

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
