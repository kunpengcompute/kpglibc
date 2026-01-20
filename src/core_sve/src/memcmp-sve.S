/* memcmp - compare memory

   Copyright (C) 2013-2020 Free Software Foundation, Inc.
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
   <https://www.gnu.org/licenses/>.  */
 
#include "asmdefs.h"

ENTRY (memcmp)
    cbz    x2, .Lequal
    mov    x3, #0

1:  whilelo    p0.b, x3, x2
    b.none    .Lequal

    addvl    x7, x3, #1
    addvl    x8, x3, #2
    addvl    x9, x3, #3


    ld1b    z0.b, p0/z, [x0, x3]
    ld1b    z1.b, p0/z, [x1, x3]
    cmpne    p1.b, p0/z, z0.b, z1.b
    b.any    .Lmm0

    whilelo    p2.b, x7, x2
    b.none    .Lequal
    ld1b    z2.b, p2/z, [x0, x7]
    ld1b    z3.b, p2/z, [x1, x7]
    cmpne    p3.b, p2/z, z2.b, z3.b
    b.any    .Lmm1

    whilelo    p4.b, x8, x2
    b.none    .Lequal
    ld1b    z6.b, p4/z, [x0, x8]
    ld1b    z7.b, p4/z, [x1, x8]
    cmpne    p5.b, p4/z, z6.b, z7.b
    b.any    .Lmm2

    whilelo    p6.b, x9, x2
    b.none    .Lequal
    ld1b    z16.b, p6/z, [x0, x9]
    ld1b    z17.b, p6/z, [x1, x9]
    cmpne    p7.b, p6/z, z16.b, z17.b
    b.any    .Lmm3

    addvl    x3, x3, #4
    b    1b

.Lmm0:
    brkb    p2.b, p0/z, p1.b
    lasta    w0, p2, z0.b
    lasta    w1, p2, z1.b
    sub    x0, x0, x1
    ret

.Lmm1:
    brkb    p0.b, p2/z, p3.b
    lasta    w0, p0, z2.b
    lasta    w1, p0, z3.b
    sub    x0, x0, x1
    ret

.Lmm2:
    brkb    p6.b, p4/z, p5.b
    lasta    w0, p6, z6.b
    lasta    w1, p6, z7.b
    sub    x0, x0, x1
    ret

.Lmm3:
    brkb    p4.b, p6/z, p7.b
    lasta    w0, p4, z16.b
    lasta    w1, p4, z17.b
    sub    x0, x0, x1
    ret

.Lequal:
    mov    x0, #0
    ret
END (memcmp)