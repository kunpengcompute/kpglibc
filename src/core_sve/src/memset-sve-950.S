/* Optimized glibc function for Huawei Kunpeng 950 processor.
   Copyright (C) 2026 Free Software Foundation, Inc.
   Copyright (C) 2026 Huawei Technologies Co., Ltd.
   
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
#define vlen	x5
#define tmp1	x6
#define tmp2	x7
#define tmp2w	w7

ENTRY (memset)
    cntb vlen
    dup z0.b, valw_in
	add	dst_end, dst_in, cnt
    cmp cnt, 256
    b.hi L(set_256_more)

L(less_64_vl32):
    whilelo p0.b, xzr, cnt
    whilelo p1.b, vlen, cnt
    st1b    z0.b, p0, [dst_in]
    st1b    z0.b, p1, [dst_in, 1, mul vl]
    subs cnt, cnt, 64
	b.hi L(less_128)
    ret

L(less_128):
    stp	q0, q0, [dst_end, -64]
	stp	q0, q0, [dst_end, -32]
    subs cnt, cnt, 64
	b.hi L(less_192)
    ret

L(less_192):
    stp	q0, q0, [dst_end, -128]
	stp	q0, q0, [dst_end, -96]
    subs cnt, cnt, 64
	b.hi L(less_256)
    ret
L(less_256):
    stp	q0, q0, [dst_in, 64]
	stp	q0, q0, [dst_in, 96]
    ret

L(set_256_more):
	and	valw_in, valw_in, 255
    and tmp1, dst_in, 63
	bic	dst_now, dst_in, 63
    add cnt, cnt, tmp1
	stp	q0, q0, [dst_in]
    stp	q0, q0, [dst_in, 32]
    cmp valw_in, 0
	b.eq L(try_zva_branch)
    
	.p2align 4
L(no_zva):
	sub	cnt, cnt, 64 + 64
1:	
	stp q0, q0, [dst_now, 64]
	stp q0, q0, [dst_now, 96]
	add dst_now, dst_now, 64
	subs cnt, cnt, 64
	b.hi 1b
2:	stp	q0, q0, [dst_end, -64]
	stp	q0, q0, [dst_end, -32]
	ret

	.p2align 4
L(try_zva_branch):
    mrs	tmp2, dczid_el0
	tbnz tmp2w, 4, L(no_zva)
	and	tmp2w, tmp2w, 15
	cmp	tmp2w, 4	/* ZVA size is 64 bytes.  */
	b.ne L(no_zva)

    add dst_now, dst_now, 64
    subs cnt, cnt, 128+64
    b.ls L(zva_loop_tail)

L(zva_loop_128):
	dc	zva, dst_now
	add	dst_now, dst_now, 64
	dc	zva, dst_now
	add	dst_now, dst_now, 64
	subs cnt, cnt, 128
	b.hi L(zva_loop_128)

L(zva_loop_tail):
    add cnt, cnt, 128
    cmp cnt, 64
    b.ls L(sve_tail_64)
    dc	zva, dst_now
    add dst_now, dst_now, 64
    sub cnt, cnt, 64
L(sve_tail_64):
    whilelo p1.b, xzr, cnt
	whilelo p2.b, vlen, cnt
	st1b    z0.b, p1, [dst_now, 0, mul vl]
	st1b    z0.b, p2, [dst_now, 1, mul vl]
    ret
END (memset)