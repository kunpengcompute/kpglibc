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

typedef void* (*memcpy_t)(void *dest, const void *src, size_t n);
memcpy_t my_memcpy = nullptr;
void* handle_memcpy = nullptr;

void init_mymemcpy() {
    if (my_memcpy != nullptr) {
        return;
    }
    handle_memcpy = dlopen(LIB_GLIBC_NAME, RTLD_LAZY);
    if (!handle_memcpy) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    my_memcpy = (memcpy_t)dlsym(handle_memcpy, "memcpy");
    if (!my_memcpy) {
        fprintf(stderr, "Error locating my_memcpy: %s\n", dlerror());
        dlclose(handle_memcpy);
        exit(EXIT_FAILURE);
    }
}

TEST(MemcpyFuncTest, CopySmallBuffer) {
    init_mymemcpy();
    srand(time(NULL));
    const size_t buffer_size = rand() % 512;
    char source[buffer_size];
    char destination[buffer_size];

    for (size_t i = 0; i < buffer_size; ++i) {
        source[i] = static_cast<char>(i % 256);
    }

    my_memcpy(destination, source, buffer_size);

    ASSERT_EQ(memcmp(destination, source, buffer_size), 0);
    my_memcpy = nullptr;
    if (dlclose(handle_memcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcpyFuncTest, CopyMiddleBuffer) {
    init_mymemcpy();
    srand(time(NULL));
    const size_t buffer_size = rand() % 4096 + 512;
    char source[buffer_size];
    char destination[buffer_size];

    for (size_t i = 0; i < buffer_size; ++i) {
        source[i] = static_cast<char>(i % 256);
    }

    my_memcpy(destination, source, buffer_size);

    ASSERT_EQ(memcmp(destination, source, buffer_size), 0);
    my_memcpy = nullptr;
    if (dlclose(handle_memcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcpyFuncTest, CopyLargeBuffer) {
    init_mymemcpy();
    srand(time(NULL));
    const size_t buffer_size = rand() % 65536 + 4096;
    char source[buffer_size];
    char destination[buffer_size];

    for (size_t i = 0; i < buffer_size; ++i) {
        source[i] = static_cast<char>(i % 256);
    }

    my_memcpy(destination, source, buffer_size);

    ASSERT_EQ(memcmp(destination, source, buffer_size), 0);
    my_memcpy = nullptr;
    if (dlclose(handle_memcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcpyFuncTest, ZeroLength) {
    init_mymemcpy();
    char source[] = "No copy here!";
    char dest[20] = {};

    my_memcpy(dest, source, 0);

    for (size_t i = 0; i < sizeof(dest); ++i) {
        ASSERT_EQ(dest[i], 0);
    }
    my_memcpy = nullptr;
    if (dlclose(handle_memcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcpyFuncTest, NoOverflow)
{
    init_mymemcpy();
    for (int j = 0; j < 10000; j++) {
        const size_t COPY_SIZE = j;
        const size_t DST_SIZE = 3 * COPY_SIZE;

        char *src = (char *)malloc(sizeof(char) * COPY_SIZE);
        char *dst = (char *)malloc(sizeof(char) * DST_SIZE);
        for (size_t i = 0; i < COPY_SIZE; ++i) {
            src[i] = static_cast<char>(i % 256);
        }
        memset(dst, 0xBB, DST_SIZE);

        my_memcpy(dst + COPY_SIZE, src, COPY_SIZE);

        ASSERT_EQ(memcmp(dst + COPY_SIZE, src, COPY_SIZE), 0);

        for (size_t i = 0; i < COPY_SIZE; ++i) {
            ASSERT_EQ(dst[i], 0xBB) << "检测到越界写入,值被修改为 0x" << std::hex << static_cast<int>(dst[i])
                                    << ",越界位置在:" << i << ",COPY_SIZE=" << COPY_SIZE;
        }
        for (int i = 2 * COPY_SIZE; i < DST_SIZE; ++i) {
            ASSERT_EQ(dst[i], 0xBB) << "检测到越界写入,值被修改为 0x" << std::hex << static_cast<int>(dst[i])
                                    << ",越界位置在:" << i << ",COPY_SIZE=" << COPY_SIZE;
        }
        free(src);
        free(dst);
    }
    my_memcpy = nullptr;
    if (dlclose(handle_memcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcpyFuncTest, CopyLoopBuffer) {
    init_mymemcpy();
    for(int i = 0;i < 4096; i++)
    {
        const size_t buffer_size = i;
        char source[buffer_size];
        char destination[buffer_size];

        for (size_t i = 0; i < buffer_size; ++i) {
            source[i] = static_cast<char>(i % 256);
        }

        my_memcpy(destination, source, buffer_size);
        ASSERT_EQ(memcmp(destination, source, buffer_size), 0);
    }

    my_memcpy = nullptr;
    if (dlclose(handle_memcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcpyFuncTest, NoOverreadAtPageEndWithGuardPage) {
    init_mymemcpy();

    const size_t page_size = getpagesize();
    // 分配两页内存：第一页用于数据，第二页作为 guard page
    void* mem = mmap(nullptr, 2 * page_size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(mem, MAP_FAILED) << "mmap failed";

    // 将第二页设为不可访问（PROT_NONE）
    char* guard_page = static_cast<char*>(mem) + page_size;
    ASSERT_EQ(mprotect(guard_page, page_size, PROT_NONE), 0)
        << "mprotect failed";

    char* page_start = static_cast<char*>(mem);
    std::vector<char> dest(page_size);

    for (size_t n = 1; n <= page_size; ++n) {
        char* src = page_start + page_size - n;

        for (size_t i = 0; i < n; ++i) {
            src[i] = static_cast<char>(i % 256);
        }
        //防止load向后越界，如果向后越界会coredump
        my_memcpy(dest.data(), src, n);
        ASSERT_EQ(memcmp(dest.data(), src, n), 0)
            << "Data mismatch at n = " << n;

        //防止store向后越界，如果向后越界会coredump
        my_memcpy(src, src, n);
        ASSERT_EQ(memcmp(dest.data(), src, n), 0)
            << "Data mismatch at n = " << n;
    }

    munmap(mem, 2 * page_size);
    my_memcpy = nullptr;
    if (dlclose(handle_memcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcpyFuncTest, NoOverreadAtPageStartWithGuardPage) {
    init_mymemcpy();

    const size_t page_size = getpagesize();
    // 分配两页内存：第2页用于数据，第1页作为 guard page
    void* mem = mmap(nullptr, 2 * page_size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(mem, MAP_FAILED) << "mmap failed";

    // 将第一页设为不可访问（PROT_NONE）
    char* guard_page = static_cast<char*>(mem);
    ASSERT_EQ(mprotect(guard_page, page_size, PROT_NONE), 0)
        << "mprotect failed";

    char* page_start = static_cast<char*>(mem) + page_size;
    char *correct_dst = (char *)malloc(sizeof(char) * page_size);

    for (int offset = 0; offset <= 64; offset++)
    {
        char* dest = page_start + offset;
        char* src = page_start + offset;
        for (size_t copy_size = 0; copy_size <= page_size - 64; ++copy_size) {
            for (size_t i = 0; i < copy_size; ++i) {
                src[i] = static_cast<char>(i % 256);
            }

            memcpy(correct_dst, src, copy_size);
            my_memcpy(dest, src, copy_size);

            ASSERT_EQ(memcmp(dest, correct_dst, copy_size), 0)
                << "Data mismatch at copy_size = " << copy_size;
        }
    }

    free(correct_dst);
    munmap(mem, 2 * page_size);
    my_memcpy = nullptr;
    if (dlclose(handle_memcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}