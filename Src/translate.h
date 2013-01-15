/***********************************************************************
 *
 *	file: translate.h
 *
 */

#ifndef TRANSLATE_H__
#define TRANSLATE_H__

#define KCLS_AN		0x01	/* いちばん安全な文字	*/
#define KCLS_KA		0x02	/* 半角カナ				*/
#define KCLS_K1		0x04	/* 安全だと思われる漢字	*/
#define KCLS_KG		0x08	/* 不可視漢字			*/
#define KCLS_KD		0x10	/* 安全でない漢字		*/
#define KCLS_AD		0x20	/* 安全でない半角		*/

extern unsigned
translate(unsigned char *ptr,
		  int n,
		  unsigned flag);

#endif /* TRANSLATE_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
