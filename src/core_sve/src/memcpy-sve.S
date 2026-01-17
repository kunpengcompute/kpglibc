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
#define src	    x1
#define cnt	    x2
#define dst_now	x3
#define src_end	x4
#define dst_end	x5
#define tmpA_l	x6
#define tmpA_h	x7
#define tmpB_l	x8
#define tmpB_h	x9
#define tmpC_l	x10
#define tmpC_h	x11
#define tmpD_l	x12
#define tmpD_h	x13
#define tmpE_l	x14
#define tmpE_h	x15

#define vlen	x16
#define cnt2	x17
#define tmp1 x18

ENTRY (memcpy)
	cntb vlen
    add src_end, src, cnt
	add dst_end, dst_in, cnt

	subs cnt2, cnt, 192
	b.hi L(192_more)

L(less_64):
	whilelo p0.b, xzr, cnt
	whilelo p1.b, vlen, cnt
	ld1b	z0.b, p0/z, [src, 0, mul vl]
	ld1b	z1.b, p1/z, [src, 1, mul vl]
	st1b	z0.b, p0, [dst_in, 0, mul vl]
	st1b	z1.b, p1, [dst_in, 1, mul vl]
	subs cnt,cnt,64
	b.hi L(64_more)
	ret

L(64_more):
	whilelo p2.b, xzr, cnt
	whilelo p3.b, vlen, cnt
	ld1b	z2.b, p2/z, [src, 2, mul vl]
	ld1b	z3.b, p3/z, [src, 3, mul vl]
	st1b	z2.b, p2, [dst_in, 2, mul vl]
	st1b	z3.b, p3, [dst_in, 3, mul vl]
	subs cnt,cnt,64
	b.hi L(128_more)
	ret
L(128_more):
	whilelo p4.b, xzr, cnt
	whilelo p5.b, vlen, cnt
	ld1b	z4.b, p4/z, [src, 4, mul vl]
	ld1b	z5.b, p5/z, [src, 5, mul vl]
	st1b	z4.b, p4, [dst_in, 4, mul vl]
	st1b	z5.b, p5, [dst_in, 5, mul vl]
	ret

	.p2align 4
L(192_more):
	ldp	tmpA_l, tmpA_h, [src]
	ldp	tmpB_l, tmpB_h, [src, 16]
	ldp	tmpC_l, tmpC_h, [src, 32]
	ldp	tmpD_l, tmpD_h, [src, 48]

	stp	tmpA_l, tmpA_h, [dst_in]
	ldp	tmpA_l, tmpA_h, [src, 64]
	stp	tmpB_l, tmpB_h, [dst_in, 16]
	ldp	tmpB_l, tmpB_h, [src, 80]
	stp	tmpC_l, tmpC_h, [dst_in, 32]
	ldp	tmpC_l, tmpC_h, [src, 96]
	stp	tmpD_l, tmpD_h, [dst_in, 48]
	ldp	tmpD_l, tmpD_h, [src, 112]

	stp	tmpA_l, tmpA_h, [dst_in, 64]
	ldp	tmpA_l, tmpA_h, [src, 128]
	stp	tmpB_l, tmpB_h, [dst_in, 80]
	ldp	tmpB_l, tmpB_h, [src, 144]
	stp	tmpC_l, tmpC_h, [dst_in, 96]
	ldp	tmpC_l, tmpC_h, [src, 160]
	stp	tmpD_l, tmpD_h, [dst_in, 112]
	ldp	tmpD_l, tmpD_h, [src, 176]

	stp	tmpA_l, tmpA_h, [dst_in, 128]
	stp	tmpB_l, tmpB_h, [dst_in, 144]
	stp	tmpC_l, tmpC_h, [dst_in, 160]
	stp	tmpD_l, tmpD_h, [dst_in, 176]
	
	subs cnt,cnt,256
	b.hi L(256_more)
	
	whilelo p0.b, xzr, cnt2
	whilelo p1.b, vlen, cnt2
	ld1b	z0.b, p0/z, [src, 6, mul vl]
	ld1b	z1.b, p1/z, [src, 7, mul vl]
	st1b	z0.b, p0, [dst_in, 6, mul vl]
	st1b	z1.b, p1, [dst_in, 7, mul vl]
	ret

L(256_more):
	add src,src,192
	add dst_now,dst_in,192

	ldp	tmpD_l, tmpD_h, [src]
	and	tmp1, dst_now, 15
	bic	dst_now, dst_now, 15
	sub	src, src, tmp1
	add	cnt, cnt, tmp1
	ldp	tmpA_l, tmpA_h, [src, 16]
	stp	tmpD_l, tmpD_h, [dst_in, 192]

	ldp	tmpB_l, tmpB_h, [src, 32]
	ldp	tmpC_l, tmpC_h, [src, 48]!
	    
	subs cnt, cnt, 64  /* 192-256B minus 64B (previously) and then minus another 64B, leaving 112-128B */
	b.ls L(copy128_from_end)
L(loop64):
	stp tmpA_l, tmpA_h, [dst_now, 16]
	ldp tmpD_l, tmpD_h, [src, 16]
	stp tmpB_l, tmpB_h, [dst_now, 32]
	ldp tmpA_l, tmpA_h, [src, 32]
	stp tmpC_l, tmpC_h, [dst_now, 48]
	ldp tmpB_l, tmpB_h, [src, 48]
	stp	tmpD_l, tmpD_h, [dst_now, 64]!
	ldp	tmpC_l, tmpC_h, [src, 64]!
	subs cnt, cnt, 64
	b.hi L(loop64)

L(copy128_from_end):
	ldp	tmpD_l, tmpD_h, [src_end, -80]
	stp	tmpA_l, tmpA_h, [dst_now, 16]
	ldp	tmpE_l, tmpE_h, [src_end, -64]
	stp	tmpB_l, tmpB_h, [dst_now, 32]
	ldp	tmpA_l, tmpA_h, [src_end, -48]
	stp	tmpC_l, tmpC_h, [dst_now, 48]
	ldp	tmpB_l, tmpB_h, [src_end, -32]
	stp	tmpD_l, tmpD_h, [dst_end, -80]
	ldp	tmpC_l, tmpC_h, [src_end, -16]

	stp	tmpE_l, tmpE_h, [dst_end, -64]
	stp	tmpA_l, tmpA_h, [dst_end, -48]
	stp	tmpB_l, tmpB_h, [dst_end, -32]
	stp	tmpC_l, tmpC_h, [dst_end, -16]
	ret

END (memcpy)