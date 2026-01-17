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

typedef int (*memcmp_t)(const void*, const void*, size_t);
memcmp_t my_memcmp = nullptr;
int n_memcmp = 0;
void* handle_memcmp = nullptr;

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

void test_memcmp(const char* test_name, const void* ptr1, const void* ptr2, size_t n, int expected_result)
{
    int result = my_memcmp(ptr1, ptr2, n);
    std::cout << test_name << ": result = " << result << std::endl;
    EXPECT_EQ(result, expected_result);
}

TEST(MemcmpFuncTest, IdenticalMemory)
{
    init_mymemcmp();
    srand(time(NULL));
    const size_t len = rand() % 65537;
    char* str1 = new char[len];
    char* str2 = new char[len];
    
    memset(str1, 'A', len);
    memset(str2, 'A', len);

    test_memcmp("Identical memory blocks", str1, str2, len, memcmp(str1, str2, len));

    delete[] str1;
    delete[] str2;
    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcmpFuncTest, PartiallyIdenticalMemory)
{
    init_mymemcmp();
    srand(time(NULL));
    const size_t len = rand() % 65537;
    char* str1 = new char[len];
    char* str2 = new char[len];
    
    memset(str1, 'A', len / 2);
    memset(str1 + len / 2, 'B', len - len / 2);
    
    memset(str2, 'A', len / 2);
    memset(str2 + len / 2, 'D', len - len / 2);

    test_memcmp("Partially identical memory blocks", str1, str2, len, memcmp(str1, str2, len));
    test_memcmp("Partially identical memory blocks", str2, str1, len, memcmp(str2, str1, len));

    delete[] str1;
    delete[] str2;
    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcmpFuncTest, DifferentMemory)
{
    init_mymemcmp();
    srand(time(NULL));
    const size_t len = rand() % 65537;
    char* str1 = new char[len];
    char* str2 = new char[len];
    
    memset(str1, 'A', len);
    memset(str2, 'D', len);

    test_memcmp("Completely different memory blocks", str1, str2, len, memcmp(str1, str2, len));
    test_memcmp("Completely different memory blocks", str2, str1, len, memcmp(str2, str1, len));

    delete[] str1;
    delete[] str2;
    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcmpFuncTest, OneMemoryBlockLarger)
{
    init_mymemcmp();
    srand(time(NULL));
    const size_t len = rand() % 65537;
    char* str1 = new char[len + 1];
    char* str2 = new char[len];
    
    memset(str1, 'A', len);
    str1[len] = 'B';
    memset(str2, 'A', len);
    str2[len - 1] = 'D';

    test_memcmp("One memory block is larger", str1, str2, len, memcmp(str1, str2, len));

    delete[] str1;
    delete[] str2;
    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemcmpFuncTest, BigOrLittleEndian)
{
    init_mymemcmp();
    char str1[] = "sssa";
    char str2[] = "mmmm";
    int len = strlen(str1);

    test_memcmp("BigOrLittleEndian", str1, str2, len, memcmp(str1, str2, len));
    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}