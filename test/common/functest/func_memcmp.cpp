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

static const size_t MAX_TEST_SIZE = 64 * 1024;

typedef int (*memcmp_t)(const void *, const void *, size_t);
memcmp_t my_memcmp = nullptr;
int n_memcmp = 0;
void *handle_memcmp = nullptr;

void init_mymemcmp()
{
    if (my_memcmp != nullptr) {
        return;
    }

    handle_memcmp = dlopen(LIB_GLIBC_NAME, RTLD_LAZY);
    if (!handle_memcmp) {
        std::cerr << "Error loading libkpglibc.so memcmp: " << dlerror() << std::endl;
        exit(EXIT_FAILURE);
    }

    my_memcmp = (memcmp_t)dlsym(handle_memcmp, "memcmp");
    if (!my_memcmp) {
        std::cerr << "Error locating libkpglibc.so memcmp: " << dlerror() << std::endl;
        dlclose(handle_memcmp);
        exit(EXIT_FAILURE);
    }
}

inline int sign(int x)
{
    if (x > 0)
        return 1;
    if (x < 0)
        return -1;
    return 0;
}

inline void expect_memcmp_equal(const void *ptr1, const void *ptr2, size_t n, int expected_result,
                                const char *msg = nullptr)
{
    int actual = my_memcmp(ptr1, ptr2, n);
    EXPECT_EQ(sign(actual), sign(expected_result))
        << (msg ? msg : "") << " expected=" << expected_result << " actual=" << actual;
}

void test_memcmp(const char *test_name, const void *ptr1, const void *ptr2, size_t n, int expected_result)
{
    int actual = my_memcmp(ptr1, ptr2, n);

    std::cout << test_name << ": size = " << n << ", result = " << actual << ", expected = " << expected_result
              << std::endl;

    EXPECT_EQ(sign(actual), sign(expected_result));
}

// ==================== 基础功能测试 ====================

TEST(MemcmpFuncTest, ZeroLength)
{
    init_mymemcmp();
    char str1[] = "different";
    char str2[] = "strings";

    test_memcmp("Zero length comparison", str1, str2, 0, 0);

    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcmpFuncTest, IdenticalMemory)
{
    init_mymemcmp();
    srand(time(NULL));
    const size_t len = (rand() % MAX_TEST_SIZE) + 1;
    std::cout << "Testing identical memory with size: " << len / 1024.0 / 1024.0 << " MB" << std::endl;

    std::unique_ptr<char[]> str1(new char[len]);
    std::unique_ptr<char[]> str2(new char[len]);

    // 按块填充以提高效率
    for (size_t i = 0; i < len; i += 4096) {
        size_t chunk = std::min(size_t(4096), len - i);
        memset(str1.get() + i, 'A', chunk);
        memset(str2.get() + i, 'A', chunk);
    }

    test_memcmp("Identical memory blocks", str1.get(), str2.get(), len, memcmp(str1.get(), str2.get(), len));

    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcmpFuncTest, PartiallyIdenticalMemory)
{
    init_mymemcmp();
    srand(time(NULL));
    const size_t len = (rand() % MAX_TEST_SIZE) + 1;
    std::cout << "Testing partially identical memory with size: " << len / 1024.0 / 1024.0 << " MB" << std::endl;

    std::unique_ptr<char[]> str1(new char[len]);
    std::unique_ptr<char[]> str2(new char[len]);

    size_t half = len / 2;

    for (size_t i = 0; i < half; i += 4096) {
        size_t chunk = std::min(size_t(4096), half - i);
        memset(str1.get() + i, 'A', chunk);
        memset(str2.get() + i, 'A', chunk);
    }

    for (size_t i = half; i < len; i += 4096) {
        size_t chunk = std::min(size_t(4096), len - i);
        memset(str1.get() + i, 'B', chunk);
        memset(str2.get() + i, 'D', chunk);
    }

    test_memcmp("Partially identical memory blocks", str1.get(), str2.get(), len, memcmp(str1.get(), str2.get(), len));
    test_memcmp("Partially identical memory blocks (reversed)", str2.get(), str1.get(), len,
                memcmp(str2.get(), str1.get(), len));

    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcmpFuncTest, DifferentMemory)
{
    init_mymemcmp();
    srand(time(NULL));
    const size_t len = (rand() % MAX_TEST_SIZE) + 1;
    std::cout << "Testing different memory with size: " << len / 1024.0 / 1024.0 << " MB" << std::endl;

    std::unique_ptr<char[]> str1(new char[len]);
    std::unique_ptr<char[]> str2(new char[len]);

    for (size_t i = 0; i < len; i += 4096) {
        size_t chunk = std::min(size_t(4096), len - i);
        memset(str1.get() + i, 'A', chunk);
        memset(str2.get() + i, 'D', chunk);
    }

    test_memcmp("Completely different memory blocks", str1.get(), str2.get(), len, memcmp(str1.get(), str2.get(), len));
    test_memcmp("Completely different memory blocks (reversed)", str2.get(), str1.get(), len,
                memcmp(str2.get(), str1.get(), len));

    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

// ==================== 边界测试 ====================

TEST(MemcmpFuncTest, SingleByte)
{
    init_mymemcmp();

    char a = 'A', b = 'B', c = 'A';
    test_memcmp("Single byte equal", &a, &c, 1, 0);
    test_memcmp("Single byte different", &a, &b, 1, memcmp(&a, &b, 1));

    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcmpFuncTest, MemoryBlockLarger)
{
    init_mymemcmp();
    srand(time(NULL));
    const size_t len = (rand() % MAX_TEST_SIZE) + 1;
    std::cout << "Testing different memory with size: " << len / 1024.0 / 1024.0 << " MB" << std::endl;

    std::unique_ptr<char[]> str1(new char[len+1]);
    std::unique_ptr<char[]> str2(new char[len]);

    for (size_t i = 0; i < len; i += 4096) {
        size_t chunk = std::min(size_t(4096), len - i);
        memset(str1.get() + i, 'A', chunk);
    }
    memcpy(str2.get(), str1.get(), len);
    str1[len] = 'B';
    str2[len - 1] = 'D';


    test_memcmp("One memory blocks larger", str1.get(), str2.get(), len, memcmp(str1.get(), str2.get(), len));
    test_memcmp("One memory blocks larger (reversed)", str2.get(), str1.get(), len,
                memcmp(str2.get(), str1.get(), len));

    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

// ==================== 不同大小测试 ====================

TEST(MemcmpFuncTest, SmallSizes)
{
    init_mymemcmp();

    // 测试 1-4096 字节，覆盖小数据路径的不同谓词循环
    for (size_t len = 1; len <= 4096; len++) {
        std::unique_ptr<char[]> str1(new char[len]);
        std::unique_ptr<char[]> str2(new char[len]);

        for (size_t i = 0; i < len; i++) {
            str1[i] = str2[i] = (char)(i % 256);
        }

        int expected = memcmp(str1.get(), str2.get(), len);
        expect_memcmp_equal(str1.get(), str2.get(), len, expected,
                            ("Small size equal test, len=" + std::to_string(len)).c_str());

        // 测试不同的情况
        str2[len - 1] = str1[len - 1] + 1;
        expected = memcmp(str1.get(), str2.get(), len);
        expect_memcmp_equal(str1.get(), str2.get(), len, expected,
                            ("Small size diff test, len=" + std::to_string(len)).c_str());
    }

    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcmpFuncTest, SpecialSizes)
{
    init_mymemcmp();

    std::vector<size_t> sizes = {8 * 1024, 10 * 1024 - 1, 64 * 1024 + 1, 1024 * 1024, 10 * 1024 * 1024, 64 * 1024 * 1024};

    for (size_t len : sizes) {
        std::unique_ptr<char[]> str1(new char[len]);
        std::unique_ptr<char[]> str2(new char[len]);

        for (size_t i = 0; i < len; i++) {
            str1[i] = str2[i] = (char)(rand() % 256);
        }

        int expected = memcmp(str1.get(), str2.get(), len);
        expect_memcmp_equal(str1.get(), str2.get(), len, expected,
                            ("Medium size equal test, len=" + std::to_string(len)).c_str());

        // 在不同位置制造差异
        for (size_t diff_pos : {0ul, len / 4, len / 2, 3 * len / 4, len - 1}) {
            memcpy(str2.get(), str1.get(), len);
            str2[diff_pos] = str1[diff_pos] + 1;
            expected = memcmp(str1.get(), str2.get(), len);
            expect_memcmp_equal(
                str1.get(), str2.get(), len, expected,
                ("Medium size diff at " + std::to_string(diff_pos) + ", len=" + std::to_string(len)).c_str());
        }
    }

    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

// ==================== 越界读取测试 ====================

TEST(MemcmpFuncTest, NoOverreadAtPageEndWithGuardPage)
{
    init_mymemcmp();
    const size_t page_size = getpagesize();

    void *mem = mmap(nullptr, 2 * page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(mem, MAP_FAILED) << "mmap failed";

    char *guard_page = static_cast<char *>(mem) + page_size;
    ASSERT_EQ(mprotect(guard_page, page_size, PROT_NONE), 0) << "mprotect failed";

    char *page_start = static_cast<char *>(mem);
    std::vector<char> s2_buffer(page_size);

    for (size_t n = 1; n <= page_size; ++n) {
        char *s1 = page_start + page_size - n;
        char *s2 = s2_buffer.data();

        for (size_t i = 0; i < n; ++i) {
            s1[i] = s2[i] = static_cast<char>(i % 256);
        }

        ASSERT_EQ(my_memcmp(s1, s2, n), 0) << "Data mismatch on s1 check at n = " << n;
        ASSERT_EQ(my_memcmp(s2, s1, n), 0) << "Data mismatch on s2 check at n = " << n;
    }

    munmap(mem, 2 * page_size);
    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcmpFuncTest, NoOverreadAtPageStartWithGuardPage)
{
    init_mymemcmp();
    const size_t page_size = getpagesize();

    void *mem = mmap(nullptr, 2 * page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(mem, MAP_FAILED) << "mmap failed";

    char *guard_page = static_cast<char *>(mem);
    ASSERT_EQ(mprotect(guard_page, page_size, PROT_NONE), 0) << "mprotect failed";

    char *data_page_start = static_cast<char *>(mem) + page_size;
    std::vector<char> s2_buffer(page_size);

    for (int offset = 0; offset <= 64; offset++) {
        for (size_t n = 0; n <= page_size - 64; ++n) {
            char *s1 = data_page_start + offset;
            char *s2 = s2_buffer.data();

            for (size_t i = 0; i < n; ++i) {
                s1[i] = s2[i] = static_cast<char>(i % 256);
            }

            ASSERT_EQ(my_memcmp(s1, s2, n), 0) << "Under-read check failed for s1 at offset=" << offset << ", n=" << n;
            ASSERT_EQ(my_memcmp(s2, s1, n), 0) << "Under-read check failed for s2 at offset=" << offset << ", n=" << n;
        }
    }

    munmap(mem, 2 * page_size);
    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

// ==================== 随机数据压力测试 ====================

TEST(MemcmpFuncTest, RandomPosDifferences)
{
    init_mymemcmp();
    srand(time(NULL));

    // 测试随机大小和位置差异
    for (int test = 0; test < 1000; test++) {
        size_t len = (rand() % MAX_TEST_SIZE) + 1;

        std::unique_ptr<char[]> str1(new char[len]);
        std::unique_ptr<char[]> str2(new char[len]);

        for (size_t i = 0; i < len; ++i) {
            str1[i] = rand() % 256;
        }
        memcpy(str2.get(), str1.get(), len);

        int expected = memcmp(str1.get(), str2.get(), len);
        expect_memcmp_equal(str1.get(), str2.get(), len, expected,
                            ("Random stress test equal, len=" + std::to_string(len)).c_str());

        size_t diff_pos = rand() % len;
        str2[diff_pos] = str1[diff_pos] + 1;
        expected = memcmp(str1.get(), str2.get(), len);
        expect_memcmp_equal(str1.get(), str2.get(), len, expected,
                            ("Random stress test diff, len=" + std::to_string(len)).c_str());
    }

    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}