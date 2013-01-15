#ifndef TR64_H__
#define TR64_H__

#include "mtytypes.h"

/* いろいろめんどくさいのでキャストして使え */
extern char transpose64[];

#ifdef USE_REGPARM

typedef void __attribute__((regparm(3)))
	(*TR64_PP_T)(void *a, uint64_t *d, uint64_t const *s);
#define CALL_TR64(s,d) (*(TR64_PP_T)transpose64)(code,d,s)

#elif defined(__GNUC__)

#define CALL_TR64(s,d)	\
{int d_c, d_d;									\
asm volatile("call *%2"							\
			 : "=c"(d_c), "=d"(d_d)				\
			 : "r"(transpose64), "0"(s), "1"(d)	\
			 : "memory");						\
}

#else /* USE_REGPARM */

typedef void (__fastcall *TR64_PP_T)(uint64_t const *s, uint64_t *d);
#define CALL_TR64(s,d) (*(TR64_PP_T)transpose64)(s,d)

#endif

#endif /* TR64_H__ */

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
