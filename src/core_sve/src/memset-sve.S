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
#define vlen	x5
#define cnt_backup	x6

ENTRY (memset)
    cntb	vlen
    dup z0.b, valw_in // Critical: Avoiding 'dup z0' and 'dup q0' is required (redundant and prohibited)
	add	dst_end, dst_in, cnt
    mov cnt_backup, cnt

    subs cnt, cnt, 64
	b.hi L(set_64_more)

L(less_64):
    whilelo p0.b, xzr, cnt_backup
    whilelo p1.b, vlen, cnt_backup
    st1b    z0.b, p0, [dst_in]
    st1b    z0.b, p1, [dst_in, 1, mul vl]
    ret

L(set_64_more):
    cmp cnt_backup, 128
    b.ls L(less_128)
    cmp	cnt_backup, 256
    b.ls L(handle_128_256)
	and	valw_in, valw_in, 255
	bic	dst_now, dst_in, 15
	str	q0, [dst_in]
	ccmp valw_in, 0, 0, cs
	b.eq L(zva_branch)

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

L(handle_128_256):
    str q0, [dst_in, 96]
    str q0, [dst_in, 112]
    str q0, [dst_in]
    str q0, [dst_in, 16]
    str q0, [dst_in, 32]
    str q0, [dst_in, 48]
    str q0, [dst_in, 64]
    str q0, [dst_in, 80]
    
    add dst_now, dst_in, 128

    subs cnt, cnt,128
    b.hi L(handle_64_128_tail)
    stp	q0, q0, [dst_end, -64]
	stp	q0, q0, [dst_end, -32]
    ret

L(handle_64_128_tail):
    stp	q0, q0, [dst_now]
	stp	q0, q0, [dst_now, 32]
    whilelo p2.b, xzr, cnt
    whilelo p3.b, vlen, cnt
    st1b    z0.b, p2, [dst_now, 2, mul vl]
    st1b    z0.b, p3, [dst_now, 3, mul vl]
    ret

    .p2align 4
L(less_128):
    stp	q0, q0, [dst_in]
	stp	q0, q0, [dst_in, 32]
    whilelo p2.b, xzr, cnt
    whilelo p3.b, vlen, cnt
    st1b    z0.b, p2, [dst_in, 2, mul vl]
    st1b    z0.b, p3, [dst_in, 3, mul vl]
    ret

	.p2align 4
L(zva_branch):
	str	q0, [dst_now, 16]
	stp	q0, q0, [dst_now, 32]
	bic	dst_now, dst_now, 63
	stp	q0, q0, [dst_now, 64]
	stp	q0, q0, [dst_now, 96]
	
	add	dst_now, dst_now, 128
    sub	cnt, dst_end, dst_now

    /* have processed 112~128 bytes*/
    cmp cnt, 192
    b.ls L(left_128_192)

/*
 * Usage Warning: Avoid chaining 'subs cnt, cnt, #128' with 'b.hi' directly.
 * Reason: 'b.hi' evaluates condition flags (C=1 and Z=0), which can lead to
 *          unintended loop continuation in edge cases (e.g., 74B remaining).
 */
L(zva_loop_128):
    sub cnt, cnt, 128
	dc	zva, dst_now
	add	dst_now, dst_now, 64
	dc	zva, dst_now
	add	dst_now, dst_now, 64
	cmp cnt , 128
	b.hi L(zva_loop_128)

L(zva_loop_tail):
    cmp cnt,64
    b.ls L(sve_tail_64)
    dc	zva, dst_now
    add dst_now,dst_now,64
    sub cnt,cnt,64
L(sve_tail_64):
    whilelo p1.b, xzr, cnt
	whilelo p2.b, vlen, cnt
	st1b z0.b, p1, [dst_now, 0, mul vl]
	st1b z0.b, p2, [dst_now, 1, mul vl]
    ret
/* 
 * Process 128B-192B. This implementation delivers optimal performance:
 * 1. Tests reveal performance degradation when STP range overlaps ZVA region
 * 2. Single ZVA instruction outperforms two STP instructions
 * 3. Eliminates redundant update operation for 'dst_now'
 */
L(left_128_192):
    dc	zva, dst_now 
    stp	q0, q0, [dst_now, 64]
	stp	q0, q0, [dst_now, 96]
    stp	q0, q0, [dst_end, -64]
	stp	q0, q0, [dst_end, -32]
	ret

END (memset)