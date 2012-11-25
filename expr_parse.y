/***********************************************************************
 *
 *	file: expr_parse.y
 *
 *	Carefree parser
 *
 */

%{

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "expr_parse.h"
#include "mtytypes.h"
#include "synth.h"
#include "wdict.h"

#if !defined(YYDEBUG) && DEBUG>=1
#define YYDEBUG 1
#endif

char *g_filename;
int g_lineno;

/* We do only a simple match */
struct ELEMS_ARY
{
  union
  {
	uint64_t rng;
	struct DICT_CHAR dict;
  } ch[10];
  unsigned rng	:10;	/* Set up if in range */
  unsigned len	: 4;
};

%}

%union {
  int a;
  struct ITREE *tree;
  struct ELEMS_ARY *elems;
  char *str;
  uint64_t bmp;
  struct DICT_CHAR dict;
}

%{

#define YY_DECL	static int yylex(void)

YY_DECL;
static void yyerror(char const *msg);
static void expr_error(char const *fmt, ...);
static unsigned cv64(int c);
static uint64_t mkrng(unsigned a, unsigned z);
static int mkdict(unsigned attr, unsigned e);
static int mkchx(int c, unsigned e);

static struct ITREE *g_node;

%}

%token			JUNK
%token			'^' '$' '?'
%token <dict>	DICT
%token <dict>	CHX CHXE
%token			OR
%token			CLS CLS_CMPL
%token <bmp>	RNGS

%type <a>		acc_op dol_op
%type <tree>	expr
%type <elems>	elems elem
%type <dict>	dict d_el
%type <bmp>		rngs

%%

file	: delims_op exprs delims_op
		| delims_op
		;

exprs	: exprs orz expr
		{
		  synth_add(g_node, $3, NULL);
		}
		| expr
		{
		  synth_add(g_node, $1, NULL);
		}
		;

delims_op
		: /*nil*/
		| orz
		;

orz		: orz OR
		| OR
		;

expr	: acc_op elems dol_op
		{
		  int i, j;
		  int ne = 0;	/* Number of terms made */

		  $$ = NULL;
		  for (i = 1; $2 && i <= 11 - $2->len; i++)
			{
			  struct ITREE *pc;

			  /* First constraint */
			  if ($1 && i > 1)
				continue;
			  /* The end of the constraints */
			  if (i == 11 - $2->len)
				{
				  /*
					Possible translations:
					The end is not possible?
					Could not reach the end?
					Could not finish it here?
				  */
				  if (($2->rng & (1 << ($2->len - 1))))
					{
					  if (!($2->ch[$2->len - 1].rng & 0x1111111111111111ULL))
						continue;
					}
				  else
					{
					  if (!$2->ch[$2->len - 1].dict.e)
						continue;
					}
				}
			  else if ($3)	/* End constraint $ */
				continue;

			  /* Expansion */
			  if (!($2->rng & 1)
				  && $2->ch[0].dict.len == $2->len
				  && $2->len >=4)
				{
				  /* Single dictionary */
				  pc = synth_make_dict(i,	/* Position */
									   &$2->ch[0].dict);
				  ne++;

				  /* If NULL is returned
					 The dictionary already exists
					 you don't want to add another
					 (Is that okay? Apparently so)*/
				  if (pc == NULL)
					continue;
				}
			  else for (j = 0, pc = synth_make_and();
						j < $2->len;
						j += (($2->rng & (1 << j))
							  ? 1
							  : $2->ch[j].dict.len))
				if ($2->rng & (1 << j))
				  {
					/* General purpose range */
					synth_add(pc,
							  synth_make_lr($2->ch[j].rng, i + j),
							  NULL);
					ne++;
				  }
				else
				  {
					/* Expand logical operations */
					int k;
					uint64_t m = $2->ch[j].dict.bmp;
					for (k = 0; k < $2->ch[j].dict.len; k++, m >>= 6)
					  {
						unsigned c = m & 0x3F;
						synth_add(pc,
								  synth_make_lr((mkrng(c, c)
												 | (($2->ch[j].dict.xp & (1 << k))
													? (c >= 014
													   ? mkrng(c + 26, c + 26)
													   : c == 0
													   ? mkrng(c, c + 1)
													   : 0)
													: 0)),
												i + j + k),
								  NULL);
						ne++;
					  }
				  }

			  if (!$$)
				$$ = synth_make_or();

			  synth_add($$, pc, NULL);
			}

		  if ($2 != NULL)
			{
			  free($2);	/* Elements not required */

			  if (!ne)
				{
				  $$ = NULL;
				  expr_error("An error occured during parsing");
				}
			}
		}
		;

acc_op	: /*nil*/				{ $$ = 0; }
		| '^'					{ $$ = 1; }
		;

dol_op	: /*nil*/				{ $$ = 0; }
		| '$'					{ $$ = 1; }
		;

elems	: elems elem
		{
		  if ($1 == NULL || $2 == NULL)
			{
			  /* fallthru */
			  if ($1 != NULL)
				free($1);
			  if ($2 != NULL)
				free($1);
			  $$ = NULL;
			}
		  else if ($1->len + $2->len <= 10)
			{
			  assert(0 < $1->len + $2->len);
			  $$->rng = $1->rng | ($2->rng << $1->len);
			  memcpy(&$$->ch[$$->len], &$2->ch[0], $2->len * sizeof($2->ch[0]));
			  $$->len += $2->len;
			  free($2);
			}
		  else
			{
			  expr_error("Expression has more than 10 characters.");
			  assert($1 != NULL);
			  assert($2 != NULL);
			  free($1);
			  free($2);
			  $$ = NULL;
			}
		}
		| elem
		;

elem	: dict
		{
		  int i;
		  if ($1.len > 0)
			{
			  $$ = calloc(1, sizeof(struct ELEMS_ARY));
			  $$->len = i = $1.len;
			  $$->rng = 0;
			  while (--i >= 0)
				$$->ch[i].dict = $1;
			}
		  else
			$$ = NULL;
		}
		| CLS rngs
		{
		  $$ = calloc(1, sizeof(struct ELEMS_ARY));
		  $$->len = 1;
		  $$->rng = 1;
		  $$->ch[0].rng = $2;
		}
		| CLS_CMPL rngs
		{
		  $$ = calloc(1, sizeof(struct ELEMS_ARY));
		  $$->len = 1;
		  $$->rng = 1;
		  $$->ch[0].rng = ~$2;
		}
		| '?'
		{
		  $$ = calloc(1, sizeof(struct ELEMS_ARY));
		  $$->len = 1;
		  $$->rng = 1;
		  /* (Remove later) Placeholder that matches everything */
		  $$->ch[0].rng = (uint64_t)-1;
		}
		| JUNK
		{ $$ = NULL; }	/* The error has already been reported */
		;

rngs	: rngs RNGS				{ $$ = $1 | $2; }
		| RNGS					{ $$ = $1; }
		;

dict	: dict d_el
		{
		  unsigned len1 = $1.len;
		  if (len1 == 0 || $2.len == 0)
			{
			  /* fallthru */
			}
		  else if (len1 + $2.len <= 9 + $2.e)
			{
			  /* merge $1 into $2
				 $2.e is inherited */
			  $$ = $2;
			  $$.len += len1;
			  $$.xp = ($$.xp << len1) | $1.xp;
			  $$.bmp = ($$.bmp << (6 * len1)) | $1.bmp;
			  $$.attr |= $1.attr;
			}
		  else
			{
			  expr_error("There is a word that is too long.");
			  $$.len = 0;
			}
		}
		| d_el
		;

d_el	: DICT
		| CHX
		| CHXE
		;

%%

/***************************************************************
 *
 *	文字を内部コード(0～63)に変換。
 *
 *	スキャナ部から呼ばれるため、
 *	エラー処理は省いても構わない…ハズ。
 *
 */

static
unsigned
cv64(int c)
{
  if ('.' <= c && c <= '/')
	return c - '.';
  else if ('0' <= c && c <= '9')
	return c - '0' + 2;
  else if ('A' <= c && c <= 'Z')
	return c - 'A' + 2 + 10;
  else if ('a' <= c && c <= 'z')
	return c - 'a' + 2 + 10 + 26;

  expr_error("<%02X>これはバグです。", c & 0xFF);
  assert(!"char range is impossible");
  return -1;
}

/***************************************************************
 *
 *	makes a bitmap that corresponds to [a-z]
 *	if the range is (in numeric values) 21-25
 *  then bits 21-25 are set high
 */

static
uint64_t
mkrng(unsigned a, unsigned z)
{
  assert(a < 64);
  if (z >= 64)
	return mkrng(a, a);
  if (a > z)
	return mkrng(z, a);
  /* シフトカウントを +1 すると、
	 1 << 64 を 1 << 0 と計算してしまうケースがあるため
	 外側で別途シフトするのが吉 */
  return (2 * ((uint64_t)1 << (z - a)) - 1) << a;
}

/***************************************************************
 *
 *	Scanner
 *
 */

#include "expr_scan.c"

int yywrap(void)
{
  return (yyin == NULL
		  || feof(yyin));
}

static
void yyerror(char const *msg)
{
  expr_error("Could not handle internal error <%s>", msg);
}

/***************************************************************
 *
 *	エラーっぽいものを報告
 *
 */

static
void
expr_error(char const *fmt, ...)
{
  va_list ap;
  fprintf(stderr, "%s:%d:", g_filename, g_lineno);
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fputc('\n', stderr);
}

/***************************************************************
 *
 *	DICT_CHAR generate(yytext, yyleng, yylval)
 *
 */

static
int
mkdict(unsigned attr, unsigned e)
{
  int i;
  yylval.dict.xp = 0;
  yylval.dict.len = yyleng;
  yylval.dict.e = e;
  yylval.dict.attr = attr;
  yylval.dict.bmp = 0;
  for (i = yyleng - 1; i >= 0; i--)
	yylval.dict.bmp = (yylval.dict.bmp << 6) | cv64(yytext[i]);
  return DICT;
}

static
int
mkchx(int c, unsigned e)
{
  yylval.dict.len = 1;
  yylval.dict.e = e;
  yylval.dict.attr = CA_C;
  if (c == '.' || c == '/')
	yylval.dict.bmp = 000;	/* dot representation */
  else
	yylval.dict.bmp = cv64(c & ~0x20);
  yylval.dict.xp = 0x01U;
  return DICT;
}

/***************************************************************
 *
 *	Expand into a tree
 *
 */

struct ITREE *
expr_parse(char const *filename)
{
  int r;

#if defined(YYDEBUG) && DEBUG>=2
  yydebug = 1;
#endif

#if DEBUG>=1
  fprintf(stderr, "target:<%s>\n", filename);
#endif

  yyin = fopen(filename, "rt");
  if (!yyin)
	{
	  perror(filename);
	  return NULL;
	}
  if (g_filename)
	free(g_filename);
  g_filename = strdup(filename);
  g_lineno = 1;

  fprintf(stderr, "Reading file <%s>...", filename);

  YY_NEW_FILE;
  g_node = synth_make_or();
  g_node->ref++;
  r = yyparse();
  assert(r == 0);

  if (g_node->n_ops == 0)
	{
	  synth_unlink(g_node);
	  g_node = NULL;
	}

  fclose(yyin);
  yyin = NULL;
  printf(stderr, " done.\n");
  return g_node;
}

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
