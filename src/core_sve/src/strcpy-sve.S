/*
 * strcpy/stpcpy - copy a string returning pointer to start/end.
 *
 * Copyright (c) 2018-2022, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */
 
#include "asmdefs.h"
.arch armv8-a+sve

ENTRY (strcpy)
	setffr
	ptrue	p2.b, all
	mov	x2, 0

	.p2align 4
0:	ldff1b	z0.b, p2/z, [x1, x2]
	rdffrs	p0.b, p2/z
	b.nlast	1f

	/* whole vector is valid. */
	cmpeq	p1.b, p2/z, z0.b, 0
	b.any	2f

	/* No zero found. store data*/
	st1b	z0.b, p2, [x0, x2]
	incb	x2, all
	b	0b

	/* First fault failed */
1:	cmpeq	p1.b, p0/z, z0.b, 0
	b.any	2f

	/* No zero. store the valid portion.  */
	setffr
	st1b	z0.b, p0, [x0, x2]
	incp	x2, p0.b
	b	0b

	/* Zero found. */
2:	brka	p0.b, p2/z, p1.b
	st1b	z0.b, p0, [x0, x2]
	ret

END (strcpy)