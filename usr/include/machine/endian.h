/*	$Unics: endian.h,v 1.0 2025/06/20 zero 0x16000 Exp $	*/

/*-
 * Copyright (c) 2025 0x16000
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MACHINE_ENDIAN_H_
#define _MACHINE_ENDIAN_H_

#define _LITTLE_ENDIAN 1234
#define _BIG_ENDIAN    4321
#define _PDP_ENDIAN    3412

/*
 * Define _BYTE_ORDER for the target architecture.
 * Extend this list for other architectures as needed.
 */

#if defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__)
#define _BYTE_ORDER _LITTLE_ENDIAN
#elif defined(__m68k__) || defined(__powerpc__) || defined(__sparc__)
#define _BYTE_ORDER _BIG_ENDIAN
#else
#error "Unknown or unsupported byte order for target architecture"
#endif

#endif /* _MACHINE_ENDIAN_H_ */
