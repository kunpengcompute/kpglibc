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
 
#include "mytest.h"
extern "C" const KPGLIBCLibraryVersion* KPGLIBC_GetLibVersion();
typedef KpGlibcResult (*GetProductVersionFn)(KPGlibcProVersion *packageInfo);
typedef const KPGLIBCLibraryVersion* (*GetLibVersionFunc)();

TEST(GetProductVersionTest, ReturnProductVersion) {
    void* handle = dlopen(LIB_GLIBC_NAME,  RTLD_LAZY);
    ASSERT_NE(handle, nullptr) << "Failed to open shared library";
    GetProductVersionFn GetProductVersion = (GetProductVersionFn) dlsym(handle, "KPGLIBC_GetProductVersion");
    ASSERT_NE(GetProductVersion, nullptr) << "Failed to find GetProductVersion function";

    KPGlibcProVersion packageInfo;
    KpGlibcResult result = GetProductVersion(&packageInfo);

    EXPECT_EQ(result, KPGLIBC_STS_NO_ERR);

    printf("GetProductVersion worked correctly.\n");
    printf("Product Name: %s\n", packageInfo.productName);
    printf("Product Version: %s\n", packageInfo.productVersion);
    printf("ComponentName: %s\n", packageInfo.componentName);
    printf("ComponentVersion: %s\n", packageInfo.componentVersion);

    GetLibVersionFunc getLibVersion = (GetLibVersionFunc)dlsym(handle, "KPGLIBC_GetLibVersion");
    ASSERT_NE(getLibVersion, nullptr) << "Failed to find function: " << dlerror();
    const KPGLIBCLibraryVersion* libVersion = getLibVersion();
    ASSERT_NE(libVersion, nullptr);

    printf("Library Version:\n");
    printf("Major: %d\n", libVersion->major);
    printf("Minor: %d\n", libVersion->minor);
    printf("Patch: %d\n", libVersion->patch);
    printf("Build Date: %s\n", libVersion->buildDate);
    dlclose(handle);
}