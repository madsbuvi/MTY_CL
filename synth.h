/***********************************************************************
 *
 *	file: synth.h
 *
 *	$Id: synth.h 185 2007-12-25 06:56:46Z chapuni $
 *
 */

#ifndef SYNTH_H__
#define SYNTH_H__

#include <stdio.h>
#include "mtytypes.h"

struct WDICT;
struct DICT_CHAR;

/* Result storage
   PARAM::hit[n] */
#define HIT_ANY		0
#define HIT_BOOL	1

#define NOT_REACHED 0

enum NODE_ID
{
  NI_LR	= -256,
  NI_SEQ,	/* XXX */
  NI_NOT,
  NI_AND,
  NI_OR,
  NI_XOR,
  NI_NAND,
  NI_MAX,	/* sentinel */
};

struct ITREE
{
  int opcode;		/* NODE_ID or raw OP_XXX */
  int tn;			/* Position in the work area(t[tn]) */

  /* for OP_LR */
  uint64_t bmp;		/* set */
  int lrn;			/* character position(1..10) */

  /* for DICT */
  struct WDICT *dict;

  int ref;			/* number of times referenced */

  /* Operand */
  int n_ops;
  int n_alloc;	/* Ensure enough is allocated below (n_ops <= n_alloc) */
  struct ITREE **ops;
  struct ITREE *tops[2];
};

extern struct ITREE *synth_make_not(void);
extern struct ITREE *synth_make_and(void);
extern struct ITREE *synth_make_or(void);
extern struct ITREE *synth_make_xor(void);

extern int synth_unlink(struct ITREE *node);

extern struct ITREE *synth_make_lr(uint64_t bmp, int lrn);

extern struct ITREE *synth_make_dict(int pos,
									 struct DICT_CHAR const *word);

extern struct ITREE *synth_add(struct ITREE *parent, ...);	/* NULL terminated */

extern int synth_synthesize(FILE *sfp,
							struct ITREE *node);

#endif /* SYNTH_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
