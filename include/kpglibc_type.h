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
 
#ifndef KPGLIBC_TYPE_H_
#define KPGLIBC_TYPE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t major;
    int32_t minor;
    int32_t patch;
    const char *buildDate;
}KPGLIBCLibraryVersion;

#define KPGLIBC_VERSION_INFO_LEN 100
typedef struct {
    char productName[KPGLIBC_VERSION_INFO_LEN];
    char productVersion[KPGLIBC_VERSION_INFO_LEN];
    char componentName[KPGLIBC_VERSION_INFO_LEN];
    char componentVersion[KPGLIBC_VERSION_INFO_LEN];
    char componentAppendInfo[KPGLIBC_VERSION_INFO_LEN];
    char softwareName[KPGLIBC_VERSION_INFO_LEN];
    char softwareVersion[KPGLIBC_VERSION_INFO_LEN];
}KPGlibcProVersion;

/*
 * The following macro defines the status of the KPGLIBC operation.
 * value of 0 means no error.
 */
typedef enum {
    KPGLIBC_STS_NO_ERR = 0,
    KPGLIBC_STS_NULL_PTR_ERR,
    KPGLIBC_STS_PARAMETER_ERR,
    KPGLIBC_STS_NOT_SUPPORT,
    KPGLIBC_STS_DIV_BY_ZERO_ERR,
    KPGLIBC_STS_MALLOC_FAILED,
    KPGLIBC_STS_INT32_OVERFLOW_ERR,
} KpGlibcResult;

#ifdef __cplusplus
}
#endif

#endif /* KPGLIBC_TYPE_H__ */