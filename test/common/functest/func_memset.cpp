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
#include <string.h>

typedef void* (*memset_t)(void* dest, int ch, size_t count);
memset_t my_memset = nullptr;
void* handle_memset = nullptr;

void init_memset_sve()
{
    if (my_memset != nullptr) {
        return;
    }
    handle_memset = dlopen(LIB_GLIBC_NAME, RTLD_LAZY);
    if (!handle_memset) {
        std::cerr << "Error loading libkpglibc.so for memset: " << dlerror() << std::endl;
        exit(EXIT_FAILURE);
    }
    my_memset = (memset_t)dlsym(handle_memset, "memset");
    if (!my_memset) {
        std::cerr << "Error finding memset in libkpglibc.so: " << dlerror() << std::endl;
        dlclose(handle_memset);
        exit(EXIT_FAILURE);
    }
}

bool check_memset_consistency(size_t size)
{
    init_memset_sve();
    char *buffer1 = (char*)malloc(size);
    char *buffer2 = (char*)malloc(size);
    if (buffer1 == NULL || buffer2 == NULL) {
        fprintf(stderr, "内存分配失败\n");
        exit(EXIT_FAILURE);
    }

    memset(buffer1, 'A', size);
    my_memset(buffer2, 'A', size);
    bool isConsistent1 = (memcmp(buffer1, buffer2, size) == 0);
    memset(buffer1, 0, size);
    my_memset(buffer2, 0, size);
    bool isConsistent2 = (memcmp(buffer1, buffer2, size) == 0);

    free(buffer1);
    free(buffer2);
    my_memset = nullptr;
    if (dlclose(handle_memset) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }

    return (isConsistent1 && isConsistent2);
}

TEST(MemsetFuncTest, ZeroSizeCheck)
{
    EXPECT_TRUE(check_memset_consistency(0)) << "Size: 0 bytes";
}

TEST(MemsetFuncTest, SmallSizesCheck)
{
    for (size_t size = 1; size <= 16; ++size) {
        EXPECT_TRUE(check_memset_consistency(size)) << "Size: " << size << " bytes";
    }
}

TEST(MemsetFuncTest, MediumSizesCheck)
{
    for (size_t size = 17; size <= 96; ++size) {
        EXPECT_TRUE(check_memset_consistency(size)) << "Size: " << size << " bytes";
    }
}

TEST(MemsetFuncTest, LargeSizesCheck)
{
    for (size_t size = 97; size <= 10000; ++size) {
        if (size == 97 || size == 1000 || size == 5000 || size == 10000) {
            EXPECT_TRUE(check_memset_consistency(size)) << "Size: " << size << " bytes";
        }
    }
}

TEST(MemsetFuncTest, NoOverflow)
{
    init_memset_sve();
    for (int j = 1; j < 10000; j++) {
        const size_t SET_SIZE = j;
        const size_t MALLOC_SIZE = 3 * SET_SIZE;
        char *src = (char *)malloc(sizeof(char) * MALLOC_SIZE);
        char *src_origin = (char *)malloc(sizeof(char) * MALLOC_SIZE);
        for (size_t i = 0; i < MALLOC_SIZE; ++i) {
            src[i] = static_cast<char>(i%256);
            src_origin[i] = src[i];
        }

        memset(src_origin + SET_SIZE, 0, SET_SIZE);
        my_memset(src + SET_SIZE, 0, SET_SIZE);
        ASSERT_EQ(memcmp(src + SET_SIZE, src_origin + SET_SIZE, SET_SIZE), 0);

        memset(src_origin + SET_SIZE, 'A', SET_SIZE);
        my_memset(src + SET_SIZE, 'A', SET_SIZE);
        ASSERT_EQ(memcmp(src + SET_SIZE, src_origin + SET_SIZE, SET_SIZE), 0);

        for (size_t i = 0; i < SET_SIZE; ++i) {
            ASSERT_EQ(src[i], src_origin[i]) << "检测到越界写入,值被修改为 0x" << std::hex << static_cast<int>(src[i])
                                             << ",越界位置在:" << i << ",SET_SIZE=" << SET_SIZE;
        }
        for (int i = 2 * SET_SIZE; i < MALLOC_SIZE; ++i) {
            ASSERT_EQ(src[i], src_origin[i]) << "检测到越界写入,值被修改为 0x" << std::hex << static_cast<int>(src[i])
                                             << ",越界位置在:" << i << ",SET_SIZE=" << SET_SIZE<<"origin="<< src_origin[i]-'\0';
        }
        free(src);
        free(src_origin);
    }
    my_memset = nullptr;
    if (dlclose(handle_memset) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}