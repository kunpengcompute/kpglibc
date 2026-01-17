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

#define srcin		x0
#define tgtchr		w1
#define cnt			x2
#define res			x0
#define src			x3
#define	tmpval		x4
#define	wtmpval		w4
#define pattern		w5
#define syndval		x6
#define srcoff		x9
#define cntleft		x10

#define vtgtchr		v0
#define vsrcdata1	v1
#define qdata		q1
#define vsrcdata2	v2
#define vis_match1	v3
#define vis_match2	v4
#define vpattern	v5
#define vend		v6
#define dend		d6

ENTRY (memchr)
	cbz	cnt, L(cnt_zero)
	cmp cnt, 256
	b.hi L(large_align_32)

L(small_align_16):
	bic	src, srcin, 15
	ld1	{vsrcdata1.16b}, [src]
	dup	vtgtchr.16b, tgtchr
	mov	wtmpval, 0xf00f
	dup	vpattern.8h, wtmpval
	cmeq vis_match1.16b, vsrcdata1.16b, vtgtchr.16b
	lsl	srcoff, srcin, 2
	and	vis_match1.16b, vis_match1.16b, vpattern.16b
	addp vend.16b, vis_match1.16b, vis_match1.16b
	fmov syndval, dend
	lsr	syndval, syndval, srcoff
	cbz	syndval, L(start_loop_16_align)

	rbit syndval, syndval
	clz	syndval, syndval
	add	res, srcin, syndval, lsr 2
	cmp	cnt, syndval, lsr 2
	csel res, res, xzr, hi
	ret

L(start_loop_16_align):
	sub	tmpval, src, srcin
	add	tmpval, tmpval, 16
	subs cntleft, cnt, tmpval
	b.ls L(cnt_zero)

	add	tmpval, cntleft, 15
	tbnz tmpval, 4, L(loop32_2)

	.p2align 4
L(loop32):
	ldr	qdata, [src, 16]!
	cmeq vis_match1.16b, vsrcdata1.16b, vtgtchr.16b
	umaxp vend.16b, vis_match1.16b, vis_match1.16b	
	fmov syndval, dend
	cbnz syndval, L(end_align_16)

L(loop32_2):
	ldr	qdata, [src, 16]!
	subs cntleft, cntleft, 32
	cmeq vis_match1.16b, vsrcdata1.16b, vtgtchr.16b
	b.ls L(end_align_16)
	umaxp vend.16b, vis_match1.16b, vis_match1.16b
	fmov syndval, dend
	cbz	syndval, L(loop32)
L(end_align_16):
	and	vis_match1.16b, vis_match1.16b, vpattern.16b
	addp vend.16b, vis_match1.16b, vis_match1.16b
	fmov syndval, dend
	add	tmpval, srcin, cnt
	sub	cntleft, tmpval, src
	rbit syndval, syndval
	clz	syndval, syndval
	cmp	cntleft, syndval, lsr 2
	add	res, src, syndval, lsr 2
	csel res, res, xzr, hi
	ret
	
L(large_align_32):
	/* 0x40100401 identify which lane matches the target char */
	mov	pattern, #0x0401
	movk pattern, #0x4010, lsl #16
	dup	vtgtchr.16b, tgtchr

	/* aligned memory 32 */
	bic	src, srcin, #31
	dup	vpattern.4s, pattern
	ands srcoff, srcin, #31
	and	cntleft, cnt, #31
	b.eq L(loop_start)

	/* 32 bytes syndrome */
	ld1	{vsrcdata1.16b, vsrcdata2.16b}, [src], #32
	sub	tmpval, srcoff, #32
	adds cnt, cnt, tmpval
	cmeq vis_match1.16b, vsrcdata1.16b, vtgtchr.16b
	cmeq vis_match2.16b, vsrcdata2.16b, vtgtchr.16b
	and	vis_match1.16b, vis_match1.16b, vpattern.16b
	and	vis_match2.16b, vis_match2.16b, vpattern.16b
	addp vend.16b, vis_match1.16b, vis_match2.16b
	addp vend.16b, vend.16b, vend.16b
	mov	syndval, vend.d[0]
	
	lsl	tmpval, srcoff, #1
	lsr	syndval, syndval, tmpval
	lsl	syndval, syndval, tmpval
	b.ls L(masklast)
	cbnz syndval, L(tail)

L(loop_start):
	ld1	{vsrcdata1.16b, vsrcdata2.16b}, [src], #32
	subs cnt, cnt, #32
	cmeq vis_match1.16b, vsrcdata1.16b, vtgtchr.16b
	cmeq vis_match2.16b, vsrcdata2.16b, vtgtchr.16b
	b.ls L(end_align_32)
	orr	vend.16b, vis_match1.16b, vis_match2.16b
	addp vend.2d, vend.2d, vend.2d
	mov	syndval, vend.d[0]
	cbz	syndval, L(loop_start)

L(end_align_32):
	and	vis_match1.16b, vis_match1.16b, vpattern.16b
	and	vis_match2.16b, vis_match2.16b, vpattern.16b
	addp vend.16b, vis_match1.16b, vis_match2.16b
	addp vend.16b, vend.16b, vend.16b
	mov	syndval, vend.d[0]
	b.hs L(tail)

L(masklast):
	add	tmpval, cntleft, srcoff
	and	tmpval, tmpval, #31
	sub	tmpval, tmpval, #32
	neg	tmpval, tmpval, lsl #1
	lsl	syndval, syndval, tmpval
	lsr	syndval, syndval, tmpval

L(tail):
	rbit syndval, syndval
	sub	src, src, #32
	cmp	syndval, #0
	clz	syndval, syndval
	add	res, src, syndval, lsr #1
	csel res, xzr, res, eq
	ret

L(cnt_zero):
	mov	res, #0
	ret
END (memchr)