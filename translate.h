/***********************************************************************
 *
 *	file: translate.h
 *
 */

#ifndef TRANSLATE_H__
#define TRANSLATE_H__

#define KCLS_AN		0x01	/* �����΂���S�ȕ���	*/
#define KCLS_KA		0x02	/* ���p�J�i				*/
#define KCLS_K1		0x04	/* ���S���Ǝv���銿��	*/
#define KCLS_KG		0x08	/* �s������			*/
#define KCLS_KD		0x10	/* ���S�łȂ�����		*/
#define KCLS_AD		0x20	/* ���S�łȂ����p		*/

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
