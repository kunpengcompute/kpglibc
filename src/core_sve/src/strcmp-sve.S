/*
 * __strcmp_aarch64_sve - compare two strings
 *
 * Copyright (c) 2018-2022, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */
 
#include "asmdefs.h"
.arch armv8-a+sve

ENTRY (strcmp)
	setffr
	ptrue	p1.b, all
	mov	x2, 0

	.p2align 4
0:	ldff1b	z0.b, p1/z, [x0, x2]
	ldff1b	z1.b, p1/z, [x1, x2]
	rdffrs	p0.b, p1/z
	b.nlast	2f

	/* whole vector is valid.  ~(eq & ~zero) -> neq | zero */
	incb	x2, all	
	cmpeq	p2.b, p1/z, z0.b, z1.b	
	cmpne	p3.b, p1/z, z0.b, 0	
	nands	p2.b, p1/z, p2.b, p3.b	
	b.none	0b

	/* neq | zero, brkb & lasta extract position */
1:	brkb	p2.b, p1/z, p2.b	
	lasta	w0, p2, z0.b		
	lasta	w1, p2, z1.b
	sub	x0, x0, x1	
	ret

	/* First fault failed */
2:	incp	x2, p0.b
	setffr				
	cmpeq	p2.b, p0/z, z0.b, z1.b
	cmpne	p3.b, p0/z, z0.b, 0
	nands	p2.b, p0/z, p2.b, p3.b
	b.none	0b
	b	1b

END (strcmp)