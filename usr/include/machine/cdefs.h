/*	$OpenBSD: cdefs.h,v 1.45 2025/05/13 15:16:43 millert Exp $	*/
/*	$NetBSD: cdefs.h,v 1.16 1996/04/03 20:46:39 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_ 1

#include <machine/cdefs.h>

/*
 * Macro to test if we're using a specific version of gcc or later.
 * GCC style: (__GNUC__ > ma) || (__GNUC__ == ma && __GNUC_MINOR__ >= mi)
 */
#ifndef __GNUC_PREREQ
#ifdef __GNUC__
#define __GNUC_PREREQ(maj, min) \
  ((__GNUC__ > (maj)) || (__GNUC__ == (maj) && __GNUC_MINOR__ >= (min)))
#else
#define __GNUC_PREREQ(maj, min) 0
#endif
#endif

/* For compatibility with OpenBSD's __GNUC_PREREQ__ naming */
#ifndef __GNUC_PREREQ__
#define __GNUC_PREREQ__ __GNUC_PREREQ
#endif

/*
 * Basic macro utilities
 */
#define __CONCAT(x, y) x ## y
#define __STRING(x)    #x

/*
 * Function prototype macro: ANSI C or C++ supports prototypes, else empty
 */
#if defined(__STDC__) || defined(__cplusplus)
#define __P(protos) protos
#else
#define __P(protos) ()
#endif

/*
 * Keywords and attributes
 */
#ifndef __const
#define __const const
#endif
#ifndef __signed
#define __signed signed
#endif
#ifndef __volatile
#define __volatile volatile
#endif

#if defined(__cplusplus) || defined(__PCC__)
#define __inline inline
#else
#if !defined(__GNUC__)
#define __inline
#endif
#endif

/*
 * Attribute macros with GCC version checks, fallback to empty
 */
#ifndef __attribute__
#if __GNUC_PREREQ(2,5)
#define __attribute__(x) __attribute__(x)
#else
#define __attribute__(x)
#endif
#endif

#if __GNUC_PREREQ(4,0)
#define __visible      __attribute__((__visibility__("default")))
#define __hidden       __attribute__((__visibility__("hidden")))
#else
#define __visible
#define __hidden
#endif

#if __GNUC_PREREQ(2,95)
#define __aligned(x)   __attribute__((__aligned__(x)))
#else
#define __aligned(x)
#endif

#if __GNUC_PREREQ(3,1)
#define __always_inline inline __attribute__((__always_inline__))
#else
#define __always_inline inline
#endif

#if __GNUC_PREREQ(2,96)
#define __pure         __attribute__((__pure__))
#define __const        __attribute__((__const__))
#else
#define __pure
#define __const
#endif

#if __GNUC_PREREQ(2,5)
#define __noreturn     __attribute__((__noreturn__))
#else
#define __noreturn
#endif

#if __GNUC_PREREQ(3,1)
#define __deprecated   __attribute__((__deprecated__))
#else
#define __deprecated
#endif

#if __GNUC_PREREQ(4,0)
#define __weak         __attribute__((__weak__))
#else
#define __weak
#endif

#if __GNUC_PREREQ(4,0)
#define __section(s)   __attribute__((__section__(#s)))
#else
#define __section(s)
#endif

#define __unused       __attribute__((__unused__))
#define __packed       __attribute__((__packed__))

/*
 * Branch prediction hints
 */
#if __GNUC_PREREQ(3,0)
#define __predict_true(exp)  __builtin_expect(!!(exp), 1)
#define __predict_false(exp) __builtin_expect(!!(exp), 0)
#define __likely(x)    __builtin_expect(!!(x), 1)
#define __unlikely(x)  __builtin_expect(!!(x), 0)
#else
#define __predict_true(exp)  (exp)
#define __predict_false(exp) (exp)
#define __likely(x)    (x)
#define __unlikely(x)  (x)
#endif

/*
 * Offsetof macro
 */
#ifndef __offsetof
#if __GNUC_PREREQ(4,1)
#define __offsetof(type, member) __builtin_offsetof(type, member)
#else
#define __offsetof(type, member) ((size_t) &((type *)0)->member)
#endif
#endif

/*
 * Static assert macro (C11 or GNUC >= 4.6 or clang)
 */
#if __GNUC_PREREQ(4,6) || defined(__clang__) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
#define __static_assert(expr, msg) _Static_assert(expr, msg)
#else
#define __static_assert(expr, msg) typedef char __CONCAT(__static_assert_, __LINE__)[(expr) ? 1 : -1]
#endif

/*
 * Byte order macros
 */
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __PDP_ENDIAN    3412

#if defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__)
#define __BYTE_ORDER __LITTLE_ENDIAN
#elif defined(__m68k__) || defined(__powerpc__) || defined(__sparc__)
#define __BYTE_ORDER __BIG_ENDIAN
#else
#error "Unknown architecture byte order"
#endif

/*
 * C++ linkage macros
 */
#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS   }
#define __THROW       throw()
#else
#define __BEGIN_DECLS
#define __END_DECLS
#define __THROW
#endif

/*
 * Feature test macros (X/Open, POSIX, BSD visibility)
 *
 * These macros define the visible features based on user macros:
 * _XOPEN_SOURCE, _POSIX_C_SOURCE, _ANSI_SOURCE, _ISOC99_SOURCE, etc.
 * Copied and adapted from OpenBSD cdefs.h.
 */

/* X/Open feature macros */
#ifdef _XOPEN_SOURCE
# if (_XOPEN_SOURCE - 0 >= 800)
#  define __XPG_VISIBLE 800
#  undef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 202405L
# elif (_XOPEN_SOURCE - 0 >= 700)
#  define __XPG_VISIBLE 700
#  undef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200809L
# elif (_XOPEN_SOURCE - 0 >= 600)
#  define __XPG_VISIBLE 600
#  undef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200112L
# elif (_XOPEN_SOURCE - 0 >= 520)
#  define __XPG_VISIBLE 520
#  undef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 199506L
# elif (_XOPEN_SOURCE - 0 >= 500)
#  define __XPG_VISIBLE 500
#  undef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 199506L
# elif (_XOPEN_SOURCE_EXTENDED - 0 == 1)
#  define __XPG_VISIBLE 420
# elif (_XOPEN_VERSION - 0 >= 4)
#  define __XPG_VISIBLE 400
# else
#  define __XPG_VISIBLE 300
# endif
#endif

/* POSIX feature macros */
#ifdef _POSIX_C_SOURCE
# if (_POSIX_C_SOURCE - 0 >= 202405)
#  define __POSIX_VISIBLE 202405
#  define __ISO_C_VISIBLE 2017
# elif (_POSIX_C_SOURCE - 0 >= 200809)
#  define __POSIX_VISIBLE 200809
#  define __ISO_C_VISIBLE 1999
# elif (_POSIX_C_SOURCE - 0 >= 200112)
#  define __POSIX_VISIBLE 200112
#  define __ISO_C_VISIBLE 1999
# elif (_POSIX_C_SOURCE - 0 >= 199506)
#  define __POSIX_VISIBLE 199506
#  define __ISO_C_VISIBLE 1990
# elif (_POSIX_C_SOURCE - 0 >= 199309)
#  define __POSIX_VISIBLE 199309
#  define __ISO_C_VISIBLE 1990
# elif (_POSIX_C_SOURCE - 0 >= 2)
#  define __POSIX_VISIBLE 199209
#  define __ISO_C_VISIBLE 1990
# else
#  define __POSIX_VISIBLE 199009
#  define __ISO_C_VISIBLE 1990
# endif
#elif defined(_POSIX_SOURCE)
# define __POSIX_VISIBLE 198808
# define __ISO_C_VISIBLE 0
#endif

/* ANSI only */
#if defined(_ANSI_SOURCE) && !defined(__POSIX_VISIBLE) && !defined(__XPG_VISIBLE)
#define __POSIX_VISIBLE 0
#define __XPG_VISIBLE 0
#define __ISO_C_VISIBLE 1990
#endif

/* ISO C standard macros override */
#if defined(_ISOC11_SOURCE) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || \
    (defined(__cplusplus) && __cplusplus >= 201703L)
#undef __ISO_C_VISIBLE
#define __ISO_C_VISIBLE 2011
#elif defined(_ISOC99_SOURCE) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
    (defined(__cplusplus) && __cplusplus >= 201103L)
#undef __ISO_C_VISIBLE
#define __ISO_C_VISIBLE 1999
#endif

/* BSD visibility */
#if !defined(_BSD_SOURCE) && \
    (defined(_ANSI_SOURCE) || defined(__XPG_VISIBLE) || defined(__POSIX_VISIBLE))
#define __BSD_VISIBLE 0
#endif

/* Default visibility */
#ifndef __XPG_VISIBLE
#define __XPG_VISIBLE 800
#endif
#ifndef __POSIX_VISIBLE
#define __POSIX_VISIBLE 202405
#endif
#ifndef __ISO_C_VISIBLE
#define __ISO_C_VISIBLE 2017
#endif
#ifndef __BSD_VISIBLE
#define __BSD_VISIBLE 1
#endif

#endif /* _SYS_CDEFS_H_ */
