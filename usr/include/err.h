/*	$OpenBSD: err.h,v 1.13 2015/08/31 02:53:56 guenther Exp $	*/
/*	$NetBSD: err.h,v 1.11 1994/10/26 00:55:52 cgd Exp $	*/
/*  $Unics: err.h,v 1.13 2025/06/21 12:28:54 $   */

/*-
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *
 *	@(#)err.h	8.1 (Berkeley) 6/2/93
 */

#ifndef _ERR_H_
#define	_ERR_H_

#include <sys/cdefs.h>
#include <machine/_types.h>		/* for __va_list */

__BEGIN_DECLS

__dead void	err(int, const char *, ...)
			__attribute__((__format__ (printf, 2, 3)));
__dead void	verr(int, const char *, __va_list)
			__attribute__((__format__ (printf, 2, 0)));
__dead void	errc(int, int, const char *, ...)
			__attribute__((__format__ (printf, 3, 4)));
__dead void	verrc(int, int, const char *, __va_list)
			__attribute__((__format__ (printf, 3, 0)));
__dead void	errx(int, const char *, ...)
			__attribute__((__format__ (printf, 2, 3)));
__dead void	verrx(int, const char *, __va_list)
			__attribute__((__format__ (printf, 2, 0)));
void		warn(const char *, ...)
			__attribute__((__format__ (printf, 1, 2)));
void		vwarn(const char *, __va_list)
			__attribute__((__format__ (printf, 1, 0)));
void		warnc(int, const char *, ...)
			__attribute__((__format__ (printf, 2, 3)));
void		vwarnc(int, const char *, __va_list)
			__attribute__((__format__ (printf, 2, 0)));
void		warnx(const char *, ...)
			__attribute__((__format__ (printf, 1, 2)));
void		vwarnx(const char *, __va_list)
			__attribute__((__format__ (printf, 1, 0)));

__END_DECLS

#endif /* !_ERR_H_ */
