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
 
#include <string.h>
#include "core_precomp.h"
#include "kpglibc_core.h"
#include "kpglibcver.h"

const KPGLIBCLibraryVersion* KPGLIBC_GetLibVersion()
{
    static KPGLIBCLibraryVersion libVersion = {
        KPGLIBC_VERSION_MAJOR,
        KPGLIBC_VERSION_MINOR,
        KPGLIBC_VERSION_PATCH,
        KPGLIBC_VERSION_BUILDDATE
    };
    return &libVersion;
}

KpGlibcResult KPGLIBC_GetProductVersion(KPGlibcProVersion *packageInfo)
{
    KPGLIBC_RETURN_IF_NULL(packageInfo, KPGLIBC_STS_NULL_PTR_ERR);
    strcpy(packageInfo->componentName, KPGLIBC_COMPONENT_NAME);
    strcpy(packageInfo->componentVersion, KPGLIBC_VERSION);
    strcpy(packageInfo->componentAppendInfo, KPGLIBC_TOOLCHAIN_INFO);
    strcpy(packageInfo->productName, "Kunpeng Boostkit");
    strcpy(packageInfo->productVersion, KPGLIBC_PRODUCT_VERSION);
    strcpy(packageInfo->softwareName, KPGLIBC_SOFTWARE_NAME);
    strcpy(packageInfo->softwareVersion, KPGLIBC_VERSION);
    return KPGLIBC_STS_NO_ERR;
}