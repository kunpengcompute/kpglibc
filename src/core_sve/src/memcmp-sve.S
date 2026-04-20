/* Optimized memcmp for Huawei Kunpeng 950 processor.
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
   <https://www.gnu.org/licenses/>.  */
 
#include "asmdefs.h"

#define src1    x0
#define src2    x1
#define cnt     x2
#define left    x3
#define off_vl      x4
#define off_vlx2    x5
#define off_vlx3    x6
#define off_vlx4    x7

ENTRY (memcmp)
    whilelo p0.b, xzr, cnt
    b.none  L(equal)
    cntb    off_vl
    ld1b    z0.b, p0/z, [src1]
    ld1b    z1.b, p0/z, [src2]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    whilelo p0.b, off_vl, cnt
    b.none  L(equal)
    cntb    off_vlx2, all, mul #2
    ld1b    z0.b, p0/z, [src1, 1, mul vl]
    ld1b    z1.b, p0/z, [src2, 1, mul vl]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    whilelo p0.b, off_vlx2, cnt
    b.none  L(equal)
    cntb    off_vlx3, all, mul #3
    ld1b    z0.b, p0/z, [src1, 2, mul vl]
    ld1b    z1.b, p0/z, [src2, 2, mul vl]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    whilelo p0.b, off_vlx3, cnt
    b.none  L(equal)
    cntb    off_vlx4, all, mul #4
    ld1b    z0.b, p0/z, [src1, 3, mul vl]
    ld1b    z1.b, p0/z, [src2, 3, mul vl]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    subs    left, cnt, off_vlx4
    b.ls    L(equal)
    add     src1, src1, off_vlx4
    add     src2, src2, off_vlx4
    cmp     left, off_vlx4
    b.lo    L(tail_4xvl)

    .p2align 4
L(loop_full):
    ld1b    z0.b, p0/z, [src1]
    ld1b    z1.b, p0/z, [src2]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    ld1b    z0.b, p0/z, [src1, off_vl]
    ld1b    z1.b, p0/z, [src2, off_vl]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    ld1b    z0.b, p0/z, [src1, off_vlx2]
    ld1b    z1.b, p0/z, [src2, off_vlx2]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    ld1b    z0.b, p0/z, [src1, off_vlx3]
    ld1b    z1.b, p0/z, [src2, off_vlx3]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    add     src1, src1, off_vlx4
    add     src2, src2, off_vlx4
    subs    left, left, off_vlx4
    cmp     left, off_vlx4
    b.hs    L(loop_full)

L(tail_4xvl):
    whilelo p0.b, xzr, left
    b.none  L(equal)
    ld1b    z0.b, p0/z, [src1]
    ld1b    z1.b, p0/z, [src2]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    whilelo p0.b, off_vl, left
    b.none  L(equal)
    ld1b    z0.b, p0/z, [src1, off_vl]
    ld1b    z1.b, p0/z, [src2, off_vl]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    whilelo p0.b, off_vlx2, left
    b.none  L(equal)
    ld1b    z0.b, p0/z, [src1, off_vlx2]
    ld1b    z1.b, p0/z, [src2, off_vlx2]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

    whilelo p0.b, off_vlx3, left
    b.none  L(equal)
    ld1b    z0.b, p0/z, [src1, off_vlx3]
    ld1b    z1.b, p0/z, [src2, off_vlx3]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch)

L(equal):
    mov x0, #0
    ret

L(mismatch):
    brkb    p2.b, p0/z, p1.b
    lasta   w0, p2, z0.b
    lasta   w1, p2, z1.b
    sub     x0, x0, x1
    ret
END (memcmp)
