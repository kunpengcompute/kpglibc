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
#define PAT8_01 0x0101010101010101
#define PAT8_7f 0x7f7f7f7f7f7f7f7f

#define src1_in		x0
#define src2_in		x1
#define res			x0

#define data1		x2
#define data1_low	w2
#define data2		x3
#define data2_low	w3
#define has_zero	x4
#define diff		x5
#define offset1		x5
#define synd_val	x6
#define tmp			x6
#define data3		x7
#define zeroone		x8
#define shift		x9
#define offset2		x10

ENTRY (strcmp)
	sub	offset2, src2_in, src1_in
	mov	zeroone, PAT8_01
	and	tmp, src1_in, 7
	tst	offset2, 7
	b.ne L(ma_less8)
	cbnz tmp, L(go_align)

	.p2align 4
L(loop_align):
	ldr	data2, [src1_in, offset2]
	ldr	data1, [src1_in], 8
L(realigned):
	sub	has_zero, data1, zeroone
	orr	tmp, data1, PAT8_7f
	bics has_zero, has_zero, tmp
	ccmp data1, data2, 0, eq
	b.eq L(loop_align)
	eor	diff, data1, data2
	orr	synd_val, diff, has_zero
L(end):
	rev	synd_val, synd_val
	rev	data1, data1
	rev	data2, data2
	clz	shift, synd_val
	lsl	data1, data1, shift
	lsl	data2, data2, shift
	lsr	data1, data1, 56
	sub	res, data1, data2, lsr 56
	ret

	.p2align 4
L(go_align):
	bic	src1_in, src1_in, 7
	ldr	data2, [src1_in, offset2]
	ldr	data1, [src1_in], 8
	neg	shift, src2_in, lsl 3
	mov	tmp, -1
	lsr	tmp, tmp, shift
	orr	data1, data1, tmp
	orr	data2, data2, tmp
	b L(realigned)

L(ma_less8):
	cbz	tmp, L(src1_align)
L(do_ma):
	ldrb data1_low, [src1_in], 1
	ldrb data2_low, [src2_in], 1
	cmp	data1_low, 0
	ccmp data1_low, data2_low, 0, ne
	b.ne L(done)
	tst	src1_in, 7
	b.ne L(do_ma)

L(src1_align):
	neg	shift, src2_in, lsl 3
	bic	src2_in, src2_in, 7
	ldr	data3, [src2_in], 8
	lsr	tmp, zeroone, shift
	orr	data3, data3, tmp
	sub	has_zero, data3, zeroone
	orr	tmp, data3, PAT8_7f
	bics has_zero, has_zero, tmp
	b.ne L(tail)
	sub	offset1, src2_in, src1_in

	.p2align 4
L(loop_unalign):
	ldr	data3, [src1_in, offset1]
	ldr	data2, [src1_in, offset2]
	sub	has_zero, data3, zeroone
	orr	tmp, data3, PAT8_7f
	ldr	data1, [src1_in], 8
	bics has_zero, has_zero, tmp
	ccmp data1, data2, 0, eq
	b.eq L(loop_unalign)

	lsl	tmp, has_zero, shift
	eor	diff, data1, data2
	orr	synd_val, diff, tmp
	cbnz synd_val, L(end)
L(tail):
	ldr	data1, [src1_in]
	neg	shift, shift
	lsr	data2, data3, shift
	lsr	has_zero, has_zero, shift

	eor	diff, data1, data2
	orr	synd_val, diff, has_zero
	b L(end)

L(done):
	sub	res, data1, data2
	ret

END (strcmp)