/***********************************************************************
 *
 *	file: desconst.h
 *
 *	$Id: desconst.h 114 2007-04-10 02:22:06Z chapuni $
 *
 */

#ifndef DESCONST_H__
#define DESCONST_H__

/* 鍵転置 PC1 */
extern const int tr_pc1[8][7];

/* LR最終転置 */
extern const int tr_fp[64 + 2];

/* 鍵シフトスケジュール
   KEY::k[2] を贅沢に用いて取り出しの簡素化を図る */
extern const int ks_ls[28];


#endif /* DESCONST_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
