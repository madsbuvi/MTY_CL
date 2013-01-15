/*
 * Some DES constants. Copied from DES_std.c of John the Ripper password cracker,
 * Copyright (c) 1996-2001,2005 by Solar Designer
 */

#ifndef _JOHN_DES_STD_H
#define _JOHN_DES_STD_H
/*
 * Various DES tables exported for use by other implementations.
 */
extern unsigned char DES_E[48];
extern unsigned char DES_PC1[56];
extern unsigned char DES_ROT[16];
extern unsigned char DES_PC2[48];

#endif