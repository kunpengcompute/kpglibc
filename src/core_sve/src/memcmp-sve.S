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
#define current x3
#define safe_limit  x4
#define off_vl      x5
#define off_vlx2    x6
#define off_vlx3    x7

ENTRY (memcmp)
    whilelo p0.b, xzr, cnt
    b.none  L(equal)
    rdvl    off_vl, #1

    ld1b    z0.b, p0/z, [src1]
    ld1b    z1.b, p0/z, [src2]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch0)

    whilelo p2.b, off_vl, cnt
    b.none  L(equal)
    rdvl    off_vlx2, #2
    ld1b    z2.b, p2/z, [src1, off_vl]
    ld1b    z3.b, p2/z, [src2, off_vl]
    cmpne   p3.b, p2/z, z2.b, z3.b
    b.any   L(mismatch1)

    whilelo p4.b, off_vlx2, cnt
    b.none  L(equal)
    rdvl    off_vlx3, #3
    ld1b    z6.b, p4/z, [src1, off_vlx2]
    ld1b    z7.b, p4/z, [src2, off_vlx2]
    cmpne   p5.b, p4/z, z6.b, z7.b
    b.any   L(mismatch2)

    whilelo p6.b, off_vlx3, cnt
    b.none  L(equal)
    rdvl    current, #4
    ld1b    z16.b, p6/z, [src1, off_vlx3]
    ld1b    z17.b, p6/z, [src2, off_vlx3]
    cmpne   p7.b, p6/z, z16.b, z17.b
    b.any   L(mismatch3)

    subs    safe_limit, cnt, current
    csel    safe_limit, safe_limit, xzr, hs // Saturate to 0 if cnt < 4VL to prevent unsigned underflow
    cmp     current, safe_limit
    b.hi    L(tail_4xvl)

    ptrue   p0.b
    .p2align 4
L(loop_full):
    addvl   off_vl, current, #1
    addvl   off_vlx2, current, #2
    addvl   off_vlx3, current, #3

    ld1b    z0.b, p0/z, [src1, current]
    ld1b    z1.b, p0/z, [src2, current]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch0)

    ld1b    z2.b, p0/z, [src1, off_vl]
    ld1b    z3.b, p0/z, [src2, off_vl]
    cmpne   p3.b, p0/z, z2.b, z3.b
    b.any   L(mismatch1_full)

    ld1b    z6.b, p0/z, [src1, off_vlx2]
    ld1b    z7.b, p0/z, [src2, off_vlx2]
    cmpne   p5.b, p0/z, z6.b, z7.b
    b.any   L(mismatch2_full)

    ld1b    z16.b, p0/z, [src1, off_vlx3]
    ld1b    z17.b, p0/z, [src2, off_vlx3]
    cmpne   p7.b, p0/z, z16.b, z17.b
    b.any   L(mismatch3_full)

    addvl   current, current, #4
    cmp     current, safe_limit
    b.le    L(loop_full)

L(tail_4xvl):
    whilelo p0.b, current, cnt
    b.none  L(equal)
    addvl   off_vl, current, #1
    ld1b    z0.b, p0/z, [src1, current]
    ld1b    z1.b, p0/z, [src2, current]
    cmpne   p1.b, p0/z, z0.b, z1.b
    b.any   L(mismatch0)

    whilelo p2.b, off_vl, cnt
    b.none  L(equal)
    addvl   off_vlx2, current, #2
    ld1b    z2.b, p2/z, [src1, off_vl]
    ld1b    z3.b, p2/z, [src2, off_vl]
    cmpne   p3.b, p2/z, z2.b, z3.b
    b.any   L(mismatch1)

    whilelo p4.b, off_vlx2, cnt
    b.none  L(equal)
    addvl   off_vlx3, current, #3
    ld1b    z6.b, p4/z, [src1, off_vlx2]
    ld1b    z7.b, p4/z, [src2, off_vlx2]
    cmpne   p5.b, p4/z, z6.b, z7.b
    b.any   L(mismatch2)

    whilelo p6.b, off_vlx3, cnt
    b.none  L(equal)
    ld1b    z16.b, p6/z, [src1, off_vlx3]
    ld1b    z17.b, p6/z, [src2, off_vlx3]
    cmpne   p7.b, p6/z, z16.b, z17.b
    b.any   L(mismatch3)

    b   L(equal)

L(mismatch1_full):
    brkb    p2.b, p0/z, p3.b
    lasta   w0, p2, z2.b
    lasta   w1, p2, z3.b
    sub     x0, x0, x1
    ret

L(mismatch2_full):
    brkb    p2.b, p0/z, p5.b
    lasta   w0, p2, z6.b
    lasta   w1, p2, z7.b
    sub     x0, x0, x1
    ret

L(mismatch3_full):
    brkb    p2.b, p0/z, p7.b
    lasta   w0, p2, z16.b
    lasta   w1, p2, z17.b
    sub     x0, x0, x1
    ret

L(mismatch0):
    brkb    p2.b, p0/z, p1.b
    lasta   w0, p2, z0.b
    lasta   w1, p2, z1.b
    sub     x0, x0, x1
    ret

L(mismatch1):
    brkb    p0.b, p2/z, p3.b
    lasta   w0, p0, z2.b
    lasta   w1, p0, z3.b
    sub     x0, x0, x1
    ret

L(mismatch2):
    brkb    p6.b, p4/z, p5.b
    lasta   w0, p6, z6.b
    lasta   w1, p6, z7.b
    sub     x0, x0, x1
    ret

L(mismatch3):
    brkb    p4.b, p6/z, p7.b
    lasta   w0, p4, z16.b
    lasta   w1, p4, z17.b
    sub     x0, x0, x1
    ret

L(equal):
    mov x0, #0
    ret
END (memcmp)