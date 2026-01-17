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

#define dst_in	x0
#define src	    x1
#define cnt	    x2
#define dst_now	x3
#define src_end	x4
#define dst_end	x5
#define tmpA_l	x6
#define tmpA_lw	w6
#define tmpA_h	x7
#define tmpB_l	x8
#define tmpB_lw	w8
#define tmpB_h	x9
#define tmpC_l	x10
#define tmpC_lw	w10
#define tmpC_h	x11
#define tmpD_l	x12
#define tmpD_h	x13
#define tmpE_l	x14
#define tmpE_h	x15
#define tmpF_l	x16
#define tmpF_h	x17
#define tmpG_l	cnt
#define tmpG_h	dst_now
#define tmpH_l	src
#define tmpH_h	src_end
#define tmp1	x14

ENTRY (memcpy)
	add	src_end, src, cnt
	add	dst_end, dst_in, cnt
	cmp	cnt, 128
	b.hi L(copy_128_more)
	cmp	cnt, 32
	b.hi L(copy_32_128)

	/* copy 0-32 B. */
	cmp	cnt, 16
	b.lo L(copy_less_16)
	ldp	tmpA_l, tmpA_h, [src]
	ldp	tmpD_l, tmpD_h, [src_end, -16]
	stp	tmpA_l, tmpA_h, [dst_in]
	stp	tmpD_l, tmpD_h, [dst_end, -16]
	ret

	/* Copy 8-15 B. */
L(copy_less_16):
	tbz	cnt, 3, L(copy_less_8)
	ldr	tmpA_l, [src]
	ldr	tmpA_h, [src_end, -8]
	str	tmpA_l, [dst_in]
	str	tmpA_h, [dst_end, -8]
	ret

	.p2align 3
L(copy_less_8):
	tbz	cnt, 2, L(copy_less_4)
	ldr	tmpA_lw, [src]
	ldr	tmpB_lw, [src_end, -4]
	str	tmpA_lw, [dst_in]
	str	tmpB_lw, [dst_end, -4]
	ret

L(copy_less_4):
	cbz	cnt, L(copy0)
	lsr	tmp1, cnt, 1
	ldrb tmpA_lw, [src]
	ldrb tmpC_lw, [src_end, -1]
	ldrb tmpB_lw, [src, tmp1]
	strb tmpA_lw, [dst_in]
	strb tmpB_lw, [dst_in, tmp1]
	strb tmpC_lw, [dst_end, -1]
L(copy0):
	ret

	.p2align 4
L(copy_32_128):
	ldp	tmpA_l, tmpA_h, [src]
	ldp	tmpB_l, tmpB_h, [src, 16]
	ldp	tmpC_l, tmpC_h, [src_end, -32]
	ldp	tmpD_l, tmpD_h, [src_end, -16]
	cmp	cnt, 64
	b.hi L(copy_65_128)
	stp	tmpA_l, tmpA_h, [dst_in]
	stp	tmpB_l, tmpB_h, [dst_in, 16]
	stp	tmpC_l, tmpC_h, [dst_end, -32]
	stp	tmpD_l, tmpD_h, [dst_end, -16]
	ret

	.p2align 4
	/* Copy 65-128 bytes.  */
L(copy_65_128):
	ldp	tmpE_l, tmpE_h, [src, 32]
	ldp	tmpF_l, tmpF_h, [src, 48]
	cmp	cnt, 96
	b.ls L(copy_96_128)
	ldp	tmpG_l, tmpG_h, [src_end, -64]
	ldp	tmpH_l, tmpH_h, [src_end, -48]
	stp	tmpG_l, tmpG_h, [dst_end, -64]
	stp	tmpH_l, tmpH_h, [dst_end, -48]
L(copy_96_128):
	stp	tmpA_l, tmpA_h, [dst_in]
	stp	tmpB_l, tmpB_h, [dst_in, 16]
	stp	tmpE_l, tmpE_h, [dst_in, 32]
	stp	tmpF_l, tmpF_h, [dst_in, 48]
	stp	tmpC_l, tmpC_h, [dst_end, -32]
	stp	tmpD_l, tmpD_h, [dst_end, -16]
	ret

	.p2align 4
L(copy_128_more):
	sub	tmp1, dst_in, src
	cbz	tmp1, L(copy0)
	cmp	tmp1, cnt
	b.lo L(copy_long_backwards)

	ldp	tmpD_l, tmpD_h, [src]
	and	tmp1, dst_in, 15
	bic	dst_now, dst_in, 15
	sub	src, src, tmp1
	add	cnt, cnt, tmp1
	ldp	tmpA_l, tmpA_h, [src, 16]
	stp	tmpD_l, tmpD_h, [dst_in]
	ldp	tmpB_l, tmpB_h, [src, 32]
	ldp	tmpC_l, tmpC_h, [src, 48]
	ldp	tmpD_l, tmpD_h, [src, 64]!
	subs cnt, cnt, 128 + 16
	b.ls L(copy64_from_end)

L(loop64):
	stp	tmpA_l, tmpA_h, [dst_now, 16]
	ldp	tmpA_l, tmpA_h, [src, 16]
	stp	tmpB_l, tmpB_h, [dst_now, 32]
	ldp	tmpB_l, tmpB_h, [src, 32]
	stp	tmpC_l, tmpC_h, [dst_now, 48]
	ldp	tmpC_l, tmpC_h, [src, 48]
	stp	tmpD_l, tmpD_h, [dst_now, 64]!
	ldp	tmpD_l, tmpD_h, [src, 64]!
	subs cnt, cnt, 64
	b.hi L(loop64)

L(copy64_from_end):
	ldp	tmpE_l, tmpE_h, [src_end, -64]
	stp	tmpA_l, tmpA_h, [dst_now, 16]
	ldp	tmpA_l, tmpA_h, [src_end, -48]
	stp	tmpB_l, tmpB_h, [dst_now, 32]
	ldp	tmpB_l, tmpB_h, [src_end, -32]
	stp	tmpC_l, tmpC_h, [dst_now, 48]
	ldp	tmpC_l, tmpC_h, [src_end, -16]
	stp	tmpD_l, tmpD_h, [dst_now, 64]
	stp	tmpE_l, tmpE_h, [dst_end, -64]
	stp	tmpA_l, tmpA_h, [dst_end, -48]
	stp	tmpB_l, tmpB_h, [dst_end, -32]
	stp	tmpC_l, tmpC_h, [dst_end, -16]
	ret

	.p2align 4
L(copy_long_backwards):
	ldp	tmpD_l, tmpD_h, [src_end, -16]
	and	tmp1, dst_end, 15
	sub	src_end, src_end, tmp1
	sub	cnt, cnt, tmp1
	ldp	tmpA_l, tmpA_h, [src_end, -16]
	stp	tmpD_l, tmpD_h, [dst_end, -16]
	ldp	tmpB_l, tmpB_h, [src_end, -32]
	ldp	tmpC_l, tmpC_h, [src_end, -48]
	ldp	tmpD_l, tmpD_h, [src_end, -64]!
	sub	dst_end, dst_end, tmp1
	subs cnt, cnt, 128
	b.ls L(copy64_from_start)

L(loop64_backwards):
	stp	tmpA_l, tmpA_h, [dst_end, -16]
	ldp	tmpA_l, tmpA_h, [src_end, -16]
	stp	tmpB_l, tmpB_h, [dst_end, -32]
	ldp	tmpB_l, tmpB_h, [src_end, -32]
	stp	tmpC_l, tmpC_h, [dst_end, -48]
	ldp	tmpC_l, tmpC_h, [src_end, -48]
	stp	tmpD_l, tmpD_h, [dst_end, -64]!
	ldp	tmpD_l, tmpD_h, [src_end, -64]!
	subs cnt, cnt, 64
	b.hi L(loop64_backwards)

L(copy64_from_start):
	ldp	tmpG_l, tmpG_h, [src, 48]
	stp	tmpA_l, tmpA_h, [dst_end, -16]
	ldp	tmpA_l, tmpA_h, [src, 32]
	stp	tmpB_l, tmpB_h, [dst_end, -32]
	ldp	tmpB_l, tmpB_h, [src, 16]
	stp	tmpC_l, tmpC_h, [dst_end, -48]
	ldp	tmpC_l, tmpC_h, [src]
	stp	tmpD_l, tmpD_h, [dst_end, -64]
	stp	tmpG_l, tmpG_h, [dst_in, 48]
	stp	tmpA_l, tmpA_h, [dst_in, 32]
	stp	tmpB_l, tmpB_h, [dst_in, 16]
	stp	tmpC_l, tmpC_h, [dst_in]
	ret
END (memcpy)