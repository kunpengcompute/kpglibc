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

#define src1_in		x0
#define src2_in		x1
#define cnt			x2
#define res			w0

#define data1_lo	x3
#define data1_w		w3
#define data1_hi	x4
#define data2_lo	x5
#define data2_w		w5
#define data2_hi	x6
#define tmp1		x7
#define tmp2		x8

ENTRY (memcmp)
	subs cnt, cnt, 16
	b.lo L(less_16)

	ldp	data1_lo, data1_hi, [src1_in], 16
	ldp	data2_lo, data2_hi, [src2_in], 16
	ccmp data1_lo, data2_lo, 0, ne
	ccmp data1_hi, data2_hi, 0, eq
	b.ne L(ret_less64)

	subs cnt, cnt, 16
	b.ls L(left_bytes)
	cmp	cnt, 112
	b.lo L(loop_16)

	and	tmp1, src1_in, 15
	add	cnt, cnt, tmp1
	sub	src1_in, src1_in, tmp1
	sub	src2_in, src2_in, tmp1
	subs cnt, cnt, 48

	.p2align 4
L(loop_align_64):
	ldp	data1_lo, data1_hi, [src1_in]
	ldp	data2_lo, data2_hi, [src2_in]
	cmp	data1_lo, data2_lo
	ccmp data1_hi, data2_hi, 0, eq
	b.ne L(ret_less64)

	ldp	data1_lo, data1_hi, [src1_in, 16]
	ldp	data2_lo, data2_hi, [src2_in, 16]
	cmp	data1_lo, data2_lo
	ccmp data1_hi, data2_hi, 0, eq
	b.ne L(ret_less64)

	ldp	data1_lo, data1_hi, [src1_in, 32]
	ldp	data2_lo, data2_hi, [src2_in, 32]
	cmp	data1_lo, data2_lo
	ccmp data1_hi, data2_hi, 0, eq
	b.ne L(ret_less64)

	ldp	data1_lo, data1_hi, [src1_in, 48]
	ldp	data2_lo, data2_hi, [src2_in, 48]
	cmp	data1_lo, data2_lo
	ccmp data1_hi, data2_hi, 0, eq
	b.ne L(ret_less64)

	subs cnt, cnt, 64
	add	src1_in, src1_in, 64
	add	src2_in, src2_in, 64
	b.pl L(loop_align_64)
	adds cnt, cnt, 48
	b.lo L(left_bytes)

L(loop_16):
	ldp	data1_lo, data1_hi, [src1_in], 16
	ldp	data2_lo, data2_hi, [src2_in], 16
	cmp	data1_lo, data2_lo
	ccmp data1_hi, data2_hi, 0, eq
	b.ne L(ret_less64)

	subs cnt, cnt, 16
	b.hi L(loop_16)
L(left_bytes):
	add	src1_in, src1_in, cnt
	add	src2_in, src2_in, cnt
	ldp	data1_lo, data1_hi, [src1_in]
	ldp	data2_lo, data2_hi, [src2_in]

	/*return 0, -1 or 1.  */
L(ret_less64):
	cmp	data1_lo, data2_lo
	csel data1_lo, data1_lo, data1_hi, ne
	csel data2_lo, data2_lo, data2_hi, ne
L(return):
/*Little Endian reverse*/
#ifndef __AARCH64EB__
	rev	data1_lo, data1_lo
	rev	data2_lo, data2_lo
#endif
	cmp	data1_lo, data2_lo
	cset res, ne
	cneg res, res, lo
	ret

	.p2align 4
L(less_16):
	adds cnt, cnt, 8
	b.lo L(less8)
	ldr	data1_lo, [src1_in]
	ldr	data2_lo, [src2_in]
	ccmp data1_lo, data2_lo, 0, ne
	b.ne L(return)

	ldr	data1_lo, [src1_in, cnt]
	ldr	data2_lo, [src2_in, cnt]
	b L(return)

	.p2align 4
L(less8):
	adds cnt, cnt, 4
	b.lo L(less4)
	ldr	data1_w, [src1_in]
	ldr	data2_w, [src2_in]
	ccmp data1_w, data2_w, 0, ne
	b.ne L(return)
	ldr	data1_w, [src1_in, cnt]
	ldr	data2_w, [src2_in, cnt]
	b L(return)

	.p2align 4
L(less4):
	adds cnt, cnt, 4
	b.eq L(ret_0)

L(byte_loop):
	ldrb data1_w, [src1_in], 1
	ldrb data2_w, [src2_in], 1
	subs cnt, cnt, 1
	ccmp data1_w, data2_w, 0, ne
	b.eq L(byte_loop)
	sub	res, data1_w, data2_w
	ret
L(ret_0):
	mov	res, 0
	ret

END (memcmp)