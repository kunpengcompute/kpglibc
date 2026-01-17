/*
   Copyright (C) 2012-2022 Free Software Foundation, Inc.
   Copyright (c) 2025 Huawei Technologies Co., Ltd.
   Optimized glibc function for Huawei Kunpeng processor.
   
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
#define dst_in	x0
#define val_in	x1
#define valw_in	w1
#define cnt		x2
#define dst_now	x3
#define dst_end	x4

ENTRY (memset)
	dup	v0.16B, valw_in
	add	dst_end, dst_in, cnt

	cmp	cnt, 96
	b.hi L(set_96_more)
	cmp	cnt, 16
	b.hs L(set_16_96)
	mov	val_in, v0.D[0]
	/* 0-15 bytes. */
	tbz	cnt, 3, 1f
	str	val_in, [dst_in]
	str	val_in, [dst_end, -8]
	ret
	nop
    /* 0-8 bytes. */
1:	tbz	cnt, 2, 2f
	str	valw_in, [dst_in]
	str	valw_in, [dst_end, -4]
	ret
    /* 0-3 bytes. */
2:	cbz	cnt, 3f
	strb valw_in, [dst_in]
	tbz	cnt, 1, 3f
	strh valw_in, [dst_end, -2]
3:	ret

	/* 16-96 bytes. */
L(set_16_96):
	str	q0, [dst_in]
	tbnz cnt, 6, L(set_64_96)
	str	q0, [dst_end, -16]
	tbz	cnt, 5, 1f
	str	q0, [dst_in, 16]
	str	q0, [dst_end, -32]
1:	ret

L(set_64_96):
	str	q0, [dst_in, 16]
	stp	q0, q0, [dst_in, 32]
	stp	q0, q0, [dst_end, -32]
	ret

L(set_96_more):
	and	valw_in, valw_in, 255
	bic	dst_now, dst_in, 15
	str	q0, [dst_in]
	cmp	cnt, 256
	ccmp valw_in, 0, 0, cs
	b.eq L(zva_loop)

	.p2align 4
L(no_zva):
	sub	cnt, dst_end, dst_now
	sub	dst_now, dst_now, 16
	sub	cnt, cnt, 64 + 16
1:	
	str q0, [dst_now, 32]
	str q0, [dst_now, 48]
	str q0, [dst_now, 64]
	str q0, [dst_now, 80]
	add dst_now, dst_now, 64
	subs cnt, cnt, 64
	b.hi 1b
2:	stp	q0, q0, [dst_end, -64]
	stp	q0, q0, [dst_end, -32]
	ret

	.p2align 3
L(zva_loop):
	str	q0, [dst_now, 16]
	stp	q0, q0, [dst_now, 32]
	bic	dst_now, dst_now, 63
	stp	q0, q0, [dst_now, 64]
	stp	q0, q0, [dst_now, 96]
	sub	cnt, dst_end, dst_now	
	sub	cnt, cnt, 256
	add	dst_now, dst_now, 128
1:	dc	zva, dst_now
	add	dst_now, dst_now, 64
	subs cnt, cnt, 64
	b.hi 1b
	stp	q0, q0, [dst_now, 0]
	stp	q0, q0, [dst_now, 32]
	stp	q0, q0, [dst_end, -64]
	stp	q0, q0, [dst_end, -32]
	ret
END (memset)