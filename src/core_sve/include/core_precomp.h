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
 
#ifndef CORE_PRECOMP_H_
#define CORE_PRECOMP_H_

#include <stdio.h>
#include <stdint.h>
#include "kpglibc_type.h"

// Define the page size
int g_pageSize;

/* ****************************************************************************
 *  The definition of forceinline
 ******************************************************************************/
#if defined (__GNUC__) || defined(__cplusplus) || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L   /* C99 */
#  define INLINE inline
#else
#  define INLINE
#endif

#define PAGE_4K_SIZE 4096
#define PAGE_64K_SIZE 65536

# define FORCE_ATTR_INLINE __attribute__((__always_inline__, __gnu_inline__, __artificial__))
#define FORCE_INLINE static INLINE FORCE_ATTR_INLINE
#define KPGLIBC_API_LOCAL __attribute__((visibility("hidden")))

/* ****************************************************************************
 *  Branch prediction
 ******************************************************************************/
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/* **************************************************************************
 *  The definition used for avoiding the warning--"unused parameter"
 ****************************************************************************/
#define UNUSED_PARAM(param) (void)(param)

/* **************************************************************************
 *  The definition used for checking common inputs
 ****************************************************************************/
#define KPGLIBC_RETURN_IF_NULL(p, ret)                 \
    if ((p) == NULL) {                              \
        return (ret);                               \
    }

#define KPGLIBC_RETURN_IF_BOTH_NULL(p1, p2, ret)       \
    if ((p1) == NULL && (p2) == NULL) {             \
        return (ret);                               \
    }

#define KPGLIBC_RETURN_IF_PARRAY_NULL(p, len, ret) do {    \
    if ((p) == NULL) {                                  \
        return (ret);                                   \
    }                                                   \
    for (int32_t i = 0; i < (len); ++i) {               \
        if ((p)[i] == NULL) {                           \
            return (ret);                               \
        }                                               \
    }                                                   \
} while (0)

// Return if the value less than or equal 0
#define KPGLIBC_RETURN_IF_LE_ZERO(value, ret)          \
    if ((value) <= 0) {                             \
        return (ret);                               \
    }

// Return if the value is 0
#define KPGLIBC_RETURN_IF_EQ_ZERO(value, ret)          \
    if ((value) == 0) {                             \
        return (ret);                               \
    }

// Return if the value is not 0
#define KPGLIBC_RETURN_IF_NE_ZERO(value, ret)          \
    if ((value) != 0) {                             \
        return (ret);                               \
    }

// Return if the len less than 0
#define KPGLIBC_RETURN_IF_LT_ZERO(value, ret)          \
    if ((value) < 0) {                              \
        return (ret);                               \
    }

// Return if origin > target
#define KPGLIBC_RETURN_IF_GT(origin, target, errCode)               \
    if ((origin) > (target)) {                                   \
        return errCode;                                          \
    }

// Return if origin < target
#define KPGLIBC_RETURN_IF_LT(origin, target, errCode)               \
    if ((origin) < (target)) {                                   \
        return errCode;                                          \
    }

// Return if origin >= target
#define KPGLIBC_RETURN_IF_GE(origin, target, errCode)               \
    if ((origin) >= (target)) {                                  \
        return errCode;                                          \
    }

// Return if origin <= target
#define KPGLIBC_RETURN_IF_LE(origin, target, errCode)               \
    if ((origin) <= (target)) {                                  \
        return errCode;                                          \
    }

// Return if origin != target
#define KPGLIBC_RETURN_IF_NE(origin, target, errCode)               \
    if ((origin) != (target)) {                                  \
        return errCode;                                          \
    }

#define KPGLIBC_RETURN_IF_EQ(origin, target, errCode)               \
    if ((origin) == (target)) {                                  \
        return (errCode);                                        \
    }

// Return if the value less than or equal 0, or is infinity,
// or is nan(nan or -nan)
#define KPGLIBC_RETURN_IF_SCALE_ERR(p, ret)                         \
    if (((p) <= 0) || ((p) == INFINITY) || (isnan(p))) {         \
        return (ret);                                            \
    }

#define KPGLIBC_RETURN_IF_ROUNDMODE_ERR(value, ret)                \
    if ((value) != KPGLIBC_RND_ZERO && (value) != KPGLIBC_RND_NEAR && (value) != KPGLIBC_RND_FINANCIAL){        \
        return (ret);                                                                                  \
    }

// Return if the roi not support 16bit step.
#define KPGLIBC_RETURN_IF_NOT_16BITEVEN_STEP(value, ret)    \
    if (((value) & 0x1) != 0) {                            \
        return (ret);                                  \
    }

// Return if the roi not support 32bit step.
#define KPGLIBC_RETURN_IF_NOT_32BITEVEN_STEP(value, ret)    \
    if (((value) & 0x3) != 0) {                            \
        return (ret);                                   \
    }

// Return if the roi not support 64bit step.
#define KPGLIBC_RETURN_IF_NOT_64BITEVEN_STEP(step, ret)      \
    if (((step) & 0x7) != 0) {                            \
        return (ret);                                     \
    }

// Return if COI is not 1, 2, or 3.
#define KPGLIBC_RETURN_IF_COI_ERR(value, ret)                \
    if (!((value) == 1 || (value) == 2 || (value) == 3)){ \
        return (ret);                                     \
    }

// Return if norm is not 0, 1.
#define KPGLIBC_RETURN_IF_NORMERR(value, ret)                \
    if (!((value) == 0 || (value) == 1)){                 \
        return (ret);                                     \
    }

#define LSHIF_7F_17BIT (0x7F << 0x17)

#ifndef RSHIFT
#define RSHIFT32(a, shift) ((a) >> (shift))     // shift >= 0, shift < 32
#define RSHIFT64(a, shift) ((a) >> (shift))     // shift >= 0, shift < 64
#define RSHIFT(a, shift) RSHIFT32(a, shift) // shift >= 0, shift < 32
#endif

#define LSHIFTUNS(a, b)  ((unsigned)(a) << (unsigned)(b))

// & operator of Ln function
#define BITWISEANDLN(a, b) ((a) & (b))

// & | ^ ~ operator of modules
#define BITWISEAND(a, b) ((unsigned)(a) & (unsigned)(b))
#define BITWISEOR(a, b)  ((unsigned)(a) | (unsigned)(b))
#define BITWISEXOR(a, b) ((unsigned)(a) ^ (unsigned)(b))
#define BITWISENOT(a)    ((unsigned)~(a))

// Return larger of a and b
#define KPGLIBC_MAX(a, b) (((a) >= (b)) ? (a) : (b))
#define KPGLIBC_MIN(a, b) (((a) <= (b)) ? (a) : (b))

#endif