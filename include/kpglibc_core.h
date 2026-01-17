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
 
#ifndef KPGLIBC_CORE_H_
#define KPGLIBC_CORE_H_
#include "kpglibc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GCCVERSION_LTE(majar, minor, plvl) (__GNUC__ < (majar) || (__GNUC__ == (majar) && __GNUC_MINOR__ < (minor)) \
            || (__GNUC__ == (majar) && __GNUC_MINOR__ == (minor) && __GNUC_PATCHLEVEL__ <= (plvl)))

#ifdef __GNUC__
#if GCCVERSION_LTE(4, 8, 5)
    typedef int16_t   float16_t;
#else
#endif
#endif

/**
 * /////////////////////////////get product version info/////////////////////////////////
 * @param[out]  packageInfo   Pointer to the constant kpglibc package infomation structure.
 * @retva       KpGlibcResult    KPGLIBC_STS_NO_ERR       Indicates no error.
                              KPGLIBC_STS_NULL_PTR_ERR Indicates an error when any of the specified pointers is NULL.
 */
KpGlibcResult KPGLIBC_GetProductVersion(KPGlibcProVersion *packageInfo);

#ifdef __cplusplus
}
#endif

#endif