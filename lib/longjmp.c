/* Copyright (C) 1991, 92, 94, 95, 97, 98 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* #include <stddef.h> */
#include <setjmp.h>
#include <signal.h>

extern void __longjmp(void* env, int val);

/*
extern void _longjmp_unwind (jmp_buf env, int val);
*/
/* Set the signal mask to the one specified in ENV, and jump
   to the position specified in ENV, causing the setjmp
   call there to return VAL, or 1 if VAL is 0.  */
void __siglongjmp (sigjmp_buf env, int val);
void __siglongjmp (sigjmp_buf env, int val)
{
  /* Perform any cleanups needed by the frames being unwound.  */
  /*
  _longjmp_unwind (env, val);
  */

  if (env[0].__mask_was_saved)
    /* Restore the saved signal mask.  */
    (void) sigprocmask (SIG_SETMASK, (sigset_t*)&env[0].__saved_mask, 0);

  /* Call the machine-dependent function to restore machine state.  */
  __longjmp (env[0].__jmpbuf, val ?: 1);
}

void siglongjmp (sigjmp_buf env, int val) __attribute__((weak,alias("__siglongjmp")));
void longjmp (sigjmp_buf env, int val) __attribute__((weak,alias("__siglongjmp")));
void _longjmp (sigjmp_buf env, int val) __attribute__((weak,alias("__siglongjmp")));
void __libc_longjmp (sigjmp_buf env, int val) __attribute__((weak,alias("__siglongjmp")));
