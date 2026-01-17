/*
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <stdio.h>
#include <time.h>
#include <sys/auxv.h>
#include <sys/syscall.h>
#include "core_precomp.h"

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    if (clk_id != CLOCK_REALTIME) {
        return syscall(SYS_clock_gettime, clk_id, tp);
    }
    if (tp == NULL) {
        return KPGLIBC_STS_PARAMETER_ERR;
    }
    if (g_pageSize == PAGE_4K_SIZE) {
    unsigned long long vdsoData = getauxval(AT_SYSINFO_EHDR) - 0x2000;
    __asm__ __volatile__(
        "mov x0, %0    \n\t"
        "mov x2, %1    \n\t"

        /* seqcnt_acquire */
        "1: ldr	w7, [x2]        \n\t"
        "tbnz	w7, #0, 1b            \n\t"
        "dmb	ishld                 \n\t"
        "mrs	x8, cntvct_el0        \n\t"

        /* Load cs_cycle_last, multiplier, shift, xtime_clock_sec,
        xtime_clock_nsec from vdso_data */
        "ldr	x10, [x2,#8]             \n\t"
        "ldp	w11, w12, [x2, #24]   \n\t"
        "ldp	x13, x14, [x2, #32]   \n\t"
        "ldr	x1, [x2,#16]             \n\t"

        /* seqcnt_check */
        "dmb	ishld                 \n\t"
        "ldr	w6, [x2]         \n\t"
        "cmp	w6, w7                \n\t"
        "b.ne	1b                    \n\t"

        /* 0x3b9aca00 = 1e9, s->ns, x12 convert CPU cycles to time */
        "mov	x9, #0xca00           \n\t"
        "movk	x9, #0x3b9a, lsl #16  \n\t"
        "lsl	x9, x9, x12           \n\t"

        "sub	x15, x8, x10          \n\t"
        "and	x15, x1, x15          \n\t"
        "mul	x15, x15, x11         \n\t"

        /* kernel time_ns + diff, store the carry in x8 */
        "add	x11, x15, x14         \n\t"
        "udiv	x8, x11, x9           \n\t"

        "add	x10, x13, x8          \n\t"
        "msub	x11, x8, x9, x11      \n\t"

        // x10: s，x11: ns
        "mov	x13, #1            \n\t"
        "lsl	x13, x13, x12         \n\t"
        "udiv	x11, x11, x13         \n\t"
        "stp	x10, x11, [x0]        \n\t"

        "mov	x0, xzr             \n\t"
        : "+r"(tp)                  /* x0 %0 */
        : "r"(vdsoData)           /* x2 %1 */
        : "cc", "memory", "x8","x9","x10","x11","x12","x13","x14","x15","w4","w5","w6","w7","w11","w12"
    );
    return KPGLIBC_STS_NO_ERR;
    } else if (g_pageSize == PAGE_64K_SIZE) {
        unsigned long long vdsoData = getauxval(AT_SYSINFO_EHDR) - 0x10000;
        __asm__ __volatile__ (
        "mov x0, %0    \n\t"
        "mov x2, %1    \n\t"

        /* seqcnt_acquire */
        "1: ldr	w7, [x2,#72]        \n\t"
        "tbnz	w7, #0, 1b            \n\t"
        "dmb	ishld                 \n\t"
        "mrs	x8, cntvct_el0        \n\t"

        /* Load cs_cycle_last, multiplier(w11), shift(w12), xtime_clock_sec(x13),
        xtime_clock_nsec(x14) from vdso_data */
        "ldr	x10, [x2]             \n\t"
        "ldp	w11, w12, [x2, #76]   \n\t"
        "ldp	x13, x14, [x2, #24]   \n\t"

        /* seqcnt_check */
        "dmb	ishld                 \n\t"
        "ldr	w6, [x2, #72]         \n\t"
        "cmp	w6, w7                \n\t"
        "b.ne	1b                    \n\t"

        /* Compute the time of day */
        "mov	x9, #0xca00           \n\t"
        "movk	x9, #0x3b9a, lsl #16  \n\t"
        "lsl	x9, x9, x12           \n\t"

        "sub	x15, x8, x10          \n\t"

        "movn	x8, #0xff00, lsl #48  \n\t"
        "and	x15, x8, x15          \n\t"
        "mul	x15, x15, x11         \n\t"

        /* kernel time_ns + diff, store the carry in x8 */
        "add	x11, x15, x14         \n\t"
        "udiv	x8, x11, x9           \n\t"

        "add	x10, x13, x8          \n\t"
        "msub	x11, x8, x9, x11      \n\t"

        /* x10: s，x11: ns */
        "mov	x13, #1               \n\t"
        "lsl	x13, x13, x12         \n\t"
        "udiv	x11, x11, x13         \n\t"
        "stp	x10, x11, [x0]        \n\t"
        
        "mov	x0, xzr               \n\t"
        : "+r"(tp)          /* x0 %0 */
        : "r"(vdsoData)    /* x2 %2 */
        : "cc", "memory", "x8","x9","x10","x11","x12","x13","x14","x15","w4","w5","w6","w7","w11","w12"
    );
        return KPGLIBC_STS_NO_ERR;
    } else {
        return syscall(SYS_clock_gettime, clk_id, tp);
    }
}