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

typedef char* (*strcpy_t)(char*, const char*);
strcpy_t my_strcpy = nullptr;
void* handle_strcpy = nullptr;

void init_mystrcpy()
{
    if (my_strcpy != nullptr) {
        return;
    }
    handle_strcpy = dlopen(LIB_GLIBC_NAME, RTLD_LAZY);
    if (!handle_strcpy) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    my_strcpy = (strcpy_t)dlsym(handle_strcpy, "strcpy");
    if (!my_strcpy) {
        fprintf(stderr, "Error locating my_strcpy: %s\n", dlerror());
        dlclose(handle_strcpy);
        exit(EXIT_FAILURE);
    }
}

TEST(StrcpyFuncTest, CopyEmptyString)
{
    init_mystrcpy();
    const char* src = "";
    char dest[50];
    char* result = my_strcpy(dest, src);
    EXPECT_EQ(result, &dest[0]);
    EXPECT_STREQ(dest, src);
    my_strcpy = nullptr;
    if (dlclose(handle_strcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(StrcpyFuncTest, CopyShortString)
{
    init_mystrcpy();
    srand(time(NULL));
    const size_t buffer_size = rand() % 512;
    char src[buffer_size];
    char dest[buffer_size];
    memset(src, 'A', buffer_size - 1);
    src[buffer_size - 1] = '\0';
   
    char* result = my_strcpy(dest, src);
    EXPECT_EQ(result, &dest[0]);
    EXPECT_STREQ(dest, src);
    my_strcpy = nullptr;
    if (dlclose(handle_strcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(StrcpyFuncTest, CopyMiddleString)
{
    init_mystrcpy();
    srand(time(NULL));
    const size_t buffer_size = rand() % 4096 + 512;
    char src[buffer_size];
    char dest[buffer_size];
    memset(src, 'A', buffer_size - 1);
    src[buffer_size - 1] = '\0';
   
    char* result = my_strcpy(dest, src);
    EXPECT_EQ(result, &dest[0]);
    EXPECT_STREQ(dest, src);
    my_strcpy = nullptr;
    if (dlclose(handle_strcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(StrcpyFuncTest, CopyLongString)
{
    init_mystrcpy();
    srand(time(NULL));
    const size_t buffer_size = rand() % 65536 + 4096;
    char src[buffer_size];
    char dest[buffer_size];
    memset(src, 'A', buffer_size - 1);
    src[buffer_size - 1] = '\0';
   
    char* result = my_strcpy(dest, src);
    EXPECT_EQ(result, &dest[0]);
    EXPECT_STREQ(dest, src);
    my_strcpy = nullptr;
    if (dlclose(handle_strcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}