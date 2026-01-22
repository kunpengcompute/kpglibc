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

#define dstin	x0
#define src		x1
#define count	x2
#define dst 	x3
#define srcend	x4
#define dstend	x5
#define vlen	x6
#define tmp1	x7
#define dstend_tmp	x6

#define A_q	q0
#define B_q	q1
#define C_q	q2
#define D_q	q3
#define E_q	q4
#define F_q	q5
#define G_q	q6
#define H_q	q7
#define I_q	q16
#define J_q	q17
#define K_q	q18
#define L_q	q19
#define M_q	q20
#define N_q	q21
#define O_q	q22
#define P_q	q23

.arch armv8.2-a+sve

ENTRY (memcpy)
	cmp	count, 128
	b.hi	L(copy_128_more)
	cntb	vlen
	cmp	count, vlen, lsl 1
	b.hi	L(copy_2vl_more)
L(copy_0_2vl):
	whilelo p0.b, xzr, count
	whilelo p1.b, vlen, count
	ld1b	z0.b, p0/z, [src]
	ld1b	z1.b, p1/z, [src, 1, mul vl]
	st1b	z0.b, p0, [dstin]
	st1b	z1.b, p1, [dstin, 1, mul vl]
	ret

	.p2align 4
L(copy_2vl_more):
	add	srcend, src, count
	add	dstend, dstin, count
	ldp	A_q, B_q, [src]
	ldp	C_q, D_q, [srcend, -32]
	cmp	count, 64
	b.ls	L(copy0_64)
	ldp	E_q, F_q, [src, 32]
	ldp	G_q, H_q, [srcend, -64]
	stp	A_q, B_q, [dstin]
	stp	C_q, D_q, [dstend, -32]
	stp	E_q, F_q, [dstin, 32]
	stp	G_q, H_q, [dstend, -64]
	ret
L(copy0_64):
	stp	A_q, B_q, [dstin]
	stp	C_q, D_q, [dstend, -32]
	ret

	.p2align 4
L(copy_128_more):
	ldp	E_q, F_q, [src]
	add	srcend, src, count
	add	dstend, dstin, count

	and	tmp1, dstin, 31
	bic	dst, dstin, 31
	sub	src, src, tmp1
	add	count, count, tmp1

	ldp	A_q, B_q, [src, 32]
	ldp	C_q, D_q, [src, 64]!
	stp	E_q, F_q, [dstin]
	subs	count, count, 64+64+32
	b.ls	L(copy64_from_end)

L(copy_loop64):
	stp	A_q, B_q, [dst, 32]
	ldp	A_q, B_q, [src, 32]
	stp	C_q, D_q, [dst, 64]!
	ldp	C_q, D_q, [src, 64]!
	subs	count, count, 64
	b.hi	L(copy_loop64)
L(copy64_from_end):
	ldp	M_q, N_q, [srcend, -64]
	stp	A_q, B_q, [dst,32]
	ldp	O_q, P_q, [srcend, -32]
	stp	C_q, D_q, [dst, 64]
	stp	M_q, N_q, [dstend, -64]
	stp	O_q, P_q, [dstend, -32]
	ret
END (memcpy)

ENTRY (memmove)
	cmp	count, 128
	b.hi	L(move_128_more)
	cntb	vlen
	cmp	count, vlen, lsl 1
	b.hi	L(move_2vl_more)
L(move_0_2vl):
	whilelo p0.b, xzr, count
	whilelo p1.b, vlen, count
	ld1b	z0.b, p0/z, [src]
	ld1b	z1.b, p1/z, [src, 1, mul vl]
	st1b	z0.b, p0, [dstin]
	st1b	z1.b, p1, [dstin, 1, mul vl]
	ret

	.p2align 4
L(move_2vl_more):
	add	srcend, src, count
	add	dstend, dstin, count
	ldp	A_q, B_q, [src]
	ldp	C_q, D_q, [srcend, -32]
	cmp	count, 64
	b.ls	L(move0_64)
	ldp	E_q, F_q, [src, 32]
	ldp	G_q, H_q, [srcend, -64]
	stp	A_q, B_q, [dstin]
	stp	E_q, F_q, [dstin, 32]
	stp	C_q, D_q, [dstend, -32]
	stp	G_q, H_q, [dstend, -64]
	ret
L(move0_64):
	stp	A_q, B_q, [dstin]
	stp	C_q, D_q, [dstend, -32]
	ret

	.p2align 4
L(move_128_more):
	add	srcend, src, count
	add	dstend_tmp, dstin, count
	sub	tmp1, dstin, src
	cbz	tmp1, L(move0)
	cmp	tmp1, count
	b.hs	L(copy_128_more)
	ldp	E_q, F_q, [srcend, -32]

	and	tmp1, dstend_tmp, 31
	bic	dstend, dstend_tmp, 31
	sub	srcend, srcend, tmp1
	sub	count, count, tmp1

	ldp	A_q, B_q, [srcend, -32]
	ldp	C_q, D_q, [srcend, -64]!
	stp	E_q, F_q, [dstend_tmp, -32]

	subs	count, count, 64+64
	b.ls	L(move64_from_end)
L(move_loop64):
	stp	A_q, B_q, [dstend, -32]
	ldp	A_q, B_q, [srcend, -32]
	stp	C_q, D_q, [dstend, -64]!
	ldp	C_q, D_q, [srcend, -64]!
	subs	count, count, 64
	b.hi	L(move_loop64)
L(move64_from_end):
	ldp	M_q, N_q, [src, 32]
	stp	A_q, B_q, [dstend, -32]
	ldp	O_q, P_q, [src]
	stp	C_q, D_q, [dstend, -64]
	stp	M_q, N_q, [dstin, 32]
	stp	O_q, P_q, [dstin]
L(move0):
	ret
END (memmove)