#ifndef _STDCKDINT_H
#define _STDCKDINT_H

/* This is a C23 addition */

/* *result = a+b; return true on arithmetic overflow */
#define ckd_add(result, a, b) __builtin_add_overflow(a,b,result)
/* *result = a-b; return true on arithmetic overflow */
#define ckd_sub(result, a, b) __builtin_sub_overflow(a,b,result)
/* *result = a*b; return true on arithmetic overflow */
#define ckd_mul(result, a, b) __builtin_mul_overflow(a,b,result)

#endif
