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
#include <stdlib.h>
#include <unistd.h>
#include "kpglibc_core.h"
#include "core_precomp.h"

#define CONSTRUCTOR __attribute__((constructor))
#define DESTRUCTOR __attribute__((destructor))

#define ARM_CPU_IMP_HISI 0x48

static int32_t gotokpglibc_initialized = 0;

FORCE_INLINE void get_page_size()
{
    g_pageSize = sysconf(_SC_PAGESIZE);
}

FORCE_INLINE void KPGlibcHwDetect(void)
{
    int32_t cpu_id;
    __asm__ volatile("mrs %0, MIDR_EL1" : "=r"(cpu_id));
    int32_t vendor = (cpu_id >> 0x18) & 0xFF;

#if defined(KPGLIBC_CPU_LIMIT) && KPGLIBC_CPU_LIMIT != 0
    if (vendor != ARM_CPU_IMP_HISI) {
        fprintf(stderr, "KPGLIBC: The software is running into an error, please check CPU ID.\n");
        abort();
    }
#endif
}

KPGLIBC_API_LOCAL CONSTRUCTOR void GotoKPGlibcInit(void)
{
    if (gotokpglibc_initialized) {
        return;
    }
    KPGlibcHwDetect();
    get_page_size();
    gotokpglibc_initialized = 1;
}