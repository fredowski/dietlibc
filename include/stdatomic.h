#ifndef _STDATOMIC_H
#define _STDATOMIC_H

typedef enum {
  memory_order_relaxed = __ATOMIC_RELAXED,
  memory_order_consume = __ATOMIC_CONSUME,
  memory_order_acquire = __ATOMIC_ACQUIRE,
  memory_order_release = __ATOMIC_RELEASE,
  memory_order_acq_rel = __ATOMIC_ACQ_REL,
  memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

#define ATOMIC_BOOL_LOCK_FREE           __GCC_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE           __GCC_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_CHAR16_T_LOCK_FREE       __GCC_ATOMIC_CHAR16_T_LOCK_FREE
#define ATOMIC_CHAR32_T_LOCK_FREE       __GCC_ATOMIC_CHAR32_T_LOCK_FREE
#define ATOMIC_WCHAR_T_LOCK_FREE        __GCC_ATOMIC_WCHAR_T_LOCK_FREE
#define ATOMIC_SHORT_LOCK_FREE          __GCC_ATOMIC_SHORT_LOCK_FREE
#define ATOMIC_INT_LOCK_FREE            __GCC_ATOMIC_INT_LOCK_FREE
#define ATOMIC_LONG_LOCK_FREE           __GCC_ATOMIC_LONG_LOCK_FREE
#define ATOMIC_LLONG_LOCK_FREE          __GCC_ATOMIC_LLONG_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE        __GCC_ATOMIC_POINTER_LOCK_FREE

#define ATOMIC_FLAG_INIT        { 0 }
#define ATOMIC_VAR_INIT(VALUE)  (VALUE)

#define atomic_init(PTR, VAL)                           \
  atomic_store_explicit (PTR, VAL, __ATOMIC_RELAXED)

#define kill_dependency(Y)                      \
  __extension__                                 \
  ({                                            \
    __auto_type __kill_dependency_tmp = (Y);    \
    __kill_dependency_tmp;                      \
  })

typedef _Atomic _Bool atomic_bool;
typedef _Atomic char atomic_char;
typedef _Atomic signed char atomic_schar;
typedef _Atomic unsigned char atomic_uchar;
typedef _Atomic short atomic_short;
typedef _Atomic unsigned short atomic_ushort;
typedef _Atomic int atomic_int;
typedef _Atomic unsigned int atomic_uint;
typedef _Atomic long atomic_long;
typedef _Atomic unsigned long atomic_ulong;
typedef _Atomic long long atomic_llong;
typedef _Atomic unsigned long long atomic_ullong;
typedef _Atomic __CHAR16_TYPE__ atomic_char16_t;
typedef _Atomic __CHAR32_TYPE__ atomic_char32_t;
typedef _Atomic __WCHAR_TYPE__ atomic_wchar_t;
typedef _Atomic __INT_LEAST8_TYPE__ atomic_int_least8_t;
typedef _Atomic __UINT_LEAST8_TYPE__ atomic_uint_least8_t;
typedef _Atomic __INT_LEAST16_TYPE__ atomic_int_least16_t;
typedef _Atomic __UINT_LEAST16_TYPE__ atomic_uint_least16_t;
typedef _Atomic __INT_LEAST32_TYPE__ atomic_int_least32_t;
typedef _Atomic __UINT_LEAST32_TYPE__ atomic_uint_least32_t;
typedef _Atomic __INT_LEAST64_TYPE__ atomic_int_least64_t;
typedef _Atomic __UINT_LEAST64_TYPE__ atomic_uint_least64_t;
typedef _Atomic __INT_FAST8_TYPE__ atomic_int_fast8_t;
typedef _Atomic __UINT_FAST8_TYPE__ atomic_uint_fast8_t;
typedef _Atomic __INT_FAST16_TYPE__ atomic_int_fast16_t;
typedef _Atomic __UINT_FAST16_TYPE__ atomic_uint_fast16_t;
typedef _Atomic __INT_FAST32_TYPE__ atomic_int_fast32_t;
typedef _Atomic __UINT_FAST32_TYPE__ atomic_uint_fast32_t;
typedef _Atomic __INT_FAST64_TYPE__ atomic_int_fast64_t;
typedef _Atomic __UINT_FAST64_TYPE__ atomic_uint_fast64_t;
typedef _Atomic __INTPTR_TYPE__ atomic_intptr_t;
typedef _Atomic __UINTPTR_TYPE__ atomic_uintptr_t;
typedef _Atomic __SIZE_TYPE__ atomic_size_t;
typedef _Atomic __PTRDIFF_TYPE__ atomic_ptrdiff_t;
typedef _Atomic __INTMAX_TYPE__ atomic_intmax_t;
typedef _Atomic __UINTMAX_TYPE__ atomic_uintmax_t;

#define ATOMIC_VAR_INIT(VALUE)	(VALUE)

#define atomic_store_explicit(PTR, VAL, MO) __atomic_store_n(PTR, VAL, MO)
#define atomic_store(PTR, VAL) atomic_store_explicit(PTR, VAL, __ATOMIC_SEQ_CST)

#define atomic_load_explicit(PTR, MO) __atomic_load_n(PTR, MO)
#define atomic_load(PTR) atomic_load_explicit(PTR, __ATOMIC_SEQ_CST)

#define atomic_exchange_explicit(PTR, VAL, MO) __atomic_exchange_n(PTR, VAL, MO)
#define atomic_exchange(PTR, VAL) atomic_exchange_explicit(PTR, VAL, __ATOMIC_SEQ_CST)

#define atomic_compare_exchange_strong_explicit(PTR, VAL, DES, SUC, FAIL) __atomic_compare_exchange_n(PTR, VAL, DES, false, SUC, FAIL)
#define atomic_compare_exchange_weak_explicit(PTR, VAL, DES, SUC, FAIL) __atomic_compare_exchange_n(PTR, VAL, DES, true, SUC, FAIL)
#define atomic_compare_exchange_strong(PTR, VAL, DES) __atomic_compare_exchange_n(PTR, VAL, DES, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#define atomic_compare_exchange_weak(PTR, VAL, DES) __atomic_compare_exchange_n(PTR, VAL, DES, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

#define atomic_fetch_add_explicit(PTR, VAL, MO) __atomic_add_fetch(PTR, VAL, MO)
#define atomic_fetch_sub_explicit(PTR, VAL, MO) __atomic_sub_fetch(PTR, VAL, MO)
#define atomic_fetch_or_explicit(PTR, VAL, MO) __atomic_or_fetch(PTR, VAL, MO)
#define atomic_fetch_xor_explicit(PTR, VAL, MO) __atomic_xor_fetch(PTR, VAL, MO)
#define atomic_fetch_and_explicit(PTR, VAL, MO) __atomic_and_fetch(PTR, VAL, MO)

#define atomic_fetch_add(PTR, VAL) atomic_fetch_add_explicit(PTR, VAL, __ATOMIC_SEQ_CST)
#define atomic_fetch_sub(PTR, VAL) atomic_fetch_sub_explicit(PTR, VAL, __ATOMIC_SEQ_CST)
#define atomic_fetch_or(PTR, VAL) atomic_fetch_or_explicit(PTR, VAL, __ATOMIC_SEQ_CST)
#define atomic_fetch_xor(PTR, VAL) atomic_fetch_xor_explicit(PTR, VAL, __ATOMIC_SEQ_CST)
#define atomic_fetch_and(PTR, VAL) atomic_fetch_and_explicit(PTR, VAL, __ATOMIC_SEQ_CST)

typedef _Atomic struct
{
  _Bool __val;
} atomic_flag;

#define atomic_flag_test_and_set(PTR) __atomic_test_and_set ((PTR), __ATOMIC_SEQ_CST)
#define atomic_flag_test_and_set_explicit(PTR, MO) __atomic_test_and_set ((PTR), (MO))
#define atomic_flag_clear(PTR)  __atomic_clear ((PTR), __ATOMIC_SEQ_CST)
#define atomic_flag_clear_explicit(PTR, MO)   __atomic_clear ((PTR), (MO))

#define atomic_thread_fence(MO) __atomic_thread_fence (MO)
#define atomic_signal_fence(MO) __atomic_signal_fence  (MO)
#define atomic_is_lock_free(OBJ) __atomic_is_lock_free (sizeof (*(OBJ)), (OBJ))

#endif
