/*
 * memcmp - compare memory
 *
 * Copyright (c) 2018-2022, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */
 
#include "asmdefs.h"
.arch armv8-a+sve

ENTRY (memcmp)
	mov	x3, 0

/* x3 is offset, condition: x3 < x2 */
0:	whilelo	p0.b, x3, x2
	b.none	2f

	ld1b	z0.b, p0/z, [x0, x3]
	ld1b	z1.b, p0/z, [x1, x3]
	incb	x3

	cmpne	p1.b, p0/z, z0.b, z1.b
	b.none	0b

/*lasta extract the last activate element*/
1:	brkb	p1.b, p0/z, p1.b
	lasta	w0, p1, z0.b		
	lasta	w1, p1, z1.b
	cmp     w0, w1
	cset    w0, ne
	cneg    w0, w0, lt		
	ret

2:	mov	x0, 0
	ret

END (memcmp)