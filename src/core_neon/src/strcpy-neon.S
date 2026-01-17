/*
   Copyright (C) 2012-2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <https://www.gnu.org/licenses/>.
*/

#include "asmdefs.h"
#define dst_in		x0
#define src_in		x1
#define res			x0

#define src_now		x2
#define dst_now		x3
#define length		x4
#define synd_val	x4
#define	tmp			x5
#define shift		x5
#define data1		x6
#define data1_low	w6
#define data2		x7
#define data2_low	w7

#define dataq0		q0
#define vdata0		v0
#define vhas_zero	v1
#define vend		v2
#define dend		d2
#define dataq1		q1

ENTRY (strcpy)
	bic	src_now, src_in, 15
	ld1	{vdata0.16b}, [src_now]
	cmeq vhas_zero.16b, vdata0.16b, 0
	lsl	shift, src_in, 2
	shrn vend.8b, vhas_zero.8h, 4
	fmov synd_val, dend
	lsr	synd_val, synd_val, shift
	cbnz synd_val, L(tail)

	ldr	dataq0, [src_now, 16]!
	cmeq vhas_zero.16b, vdata0.16b, 0
	shrn vend.8b, vhas_zero.8h, 4
	fmov synd_val, dend
	cbz	synd_val, L(loop_start)

	rbit synd_val, synd_val
	sub	tmp, src_now, src_in
	clz	length, synd_val
	add	length, tmp, length, lsr 2
	tbz	length, 4, L(less_16)
	sub	tmp, length, 15
	ldr	dataq0, [src_in]
	ldr	dataq1, [src_in, tmp]
	str	dataq0, [dst_in]
	str	dataq1, [dst_in, tmp]
	ret

L(tail):
	rbit synd_val, synd_val
	clz	length, synd_val
	lsr	length, length, 2
L(less_16):
	tbz	length, 3, L(less_8)
	sub	tmp, length, 7
	ldr	data1, [src_in]
	ldr	data2, [src_in, tmp]
	str	data1, [dst_in]
	str	data2, [dst_in, tmp]
	ret

	.p2align 4
L(less_8):
	subs tmp, length, 3
	b.lo L(less_4)
	ldr	data1_low, [src_in]
	ldr	data2_low, [src_in, tmp]
	str	data1_low, [dst_in]
	str	data2_low, [dst_in, tmp]
	ret

L(less_4):
	cbz	length, L(zerofind)
	ldrh data1_low, [src_in]
	strh data1_low, [dst_in]
L(zerofind):
	strb wzr, [dst_in, length]
	ret

	.p2align 4
L(loop_start):
	sub	tmp, src_in, dst_in
	ldr	dataq1, [src_in]
	sub	dst_now, src_now, tmp
	str	dataq1, [dst_in]
L(loop_main):
	str	dataq0, [dst_now], 32
	ldr	dataq0, [src_now, 16]
	cmeq vhas_zero.16b, vdata0.16b, 0
	umaxp vend.16b, vhas_zero.16b, vhas_zero.16b
	fmov synd_val, dend
	cbnz synd_val, L(loop_end)
	str	dataq0, [dst_now, -16]
	ldr	dataq0, [src_now, 32]!
	cmeq vhas_zero.16b, vdata0.16b, 0
	umaxp vend.16b, vhas_zero.16b, vhas_zero.16b
	fmov synd_val, dend
	cbz	synd_val, L(loop_main)
	add	dst_now, dst_now, 16
L(loop_end):
	shrn vend.8b, vhas_zero.8h, 4
	fmov synd_val, dend
	sub	dst_now, dst_now, 31
	rbit synd_val, synd_val
	clz	length, synd_val
	lsr	length, length, 2
	add	dst_now, dst_now, length
	ldr	dataq0, [dst_now, tmp]
	str	dataq0, [dst_now]
	ret

END (strcpy)