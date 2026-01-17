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
#include <random>
#include <ctime>

typedef void* (*memchr_t)(const void* src, int c, size_t n);
memchr_t my_memchr = nullptr;
void* handle_memchr = nullptr;

void init_memchr_sve() 
{
    if (my_memchr != nullptr) {
        return;
    }
    handle_memchr = dlopen(LIB_GLIBC_NAME, RTLD_LAZY);
    if (!handle_memchr) {
        std::cerr << "Error loading libkpglibc.so for memchr: " << dlerror() << std::endl;
        exit(EXIT_FAILURE);
    }
    my_memchr = (memchr_t)dlsym(handle_memchr, "memchr");
    if (!my_memchr) {
        std::cerr << "Error finding memchr in libkpglibc.so: " << dlerror() << std::endl;
        dlclose(handle_memchr);
        exit(EXIT_FAILURE);
    }
}

struct TestCase {
    const char* src;
    int c;
    size_t n;
};

bool check_memchr_consistency(const void* src, int c, size_t n) 
{
    init_memchr_sve();
    void* result1 = memchr(const_cast<void*>(src), c, n);
    void* result2 = my_memchr(src, c, n);
    my_memchr = nullptr;
    if (dlclose(handle_memchr) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
    return (result1 == result2);
}

void run_test_cases(const char* description, const TestCase test_cases[], size_t num_test_cases) 
{
    for (size_t i = 0; i < num_test_cases; ++i) {
        EXPECT_TRUE(check_memchr_consistency(test_cases[i].src, test_cases[i].c, test_cases[i].n))
            << description << "Src: \"" << test_cases[i].src << "\", Char: '" << test_cases[i].c << "', N: " << test_cases[i].n;
    }
}

std::string generate_random_string(size_t length) 
{
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz"
                           "0123456789";
    std::default_random_engine rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string random_string;
    for (size_t i = 0; i < length; ++i) {
        random_string += charset[dist(rng)];
    }
    return random_string;
}

TEST(MemchrFuncTest, EmptyStringCheck)
{
    struct TestCase test_cases[] = {
        {"", 'A', 0}
    };
    size_t num_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    run_test_cases("Empty String", test_cases, num_test_cases);
}

TEST(MemchrFuncTest, SingleMatchCheck)
{
    struct TestCase test_cases[] = {
        {"A", 'A', 1},
        {"AB", 'B', 2},
        {"ABC", 'C', 3},
        {"ABCD", 'D', 4}
    };
    size_t num_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    run_test_cases("Single Match", test_cases, num_test_cases);
}

TEST(MemchrFuncTest, MultiCharMatchCheck)
{
    struct TestCase test_cases[] = {
        {"ABBB", 'B', 4},
        {"CCC", 'C', 3},
        {"AAAA", 'A', 4},
        {"ABCDDDD", 'D', 7}
    };
    size_t num_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    run_test_cases("Multi-Character Match", test_cases, num_test_cases);
}

TEST(MemchrFuncTest, NoMatchCheck)
{
    struct TestCase test_cases[] = {{"ABCD", 'E', 4}};
    size_t num_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    run_test_cases("No Match", test_cases, num_test_cases);
}

TEST(MemchrFuncTest, NullCharIncludedCheck)
{
    struct TestCase test_cases[] = {{"A\0C", 'C', 3}};
    size_t num_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    run_test_cases("Includes Null Character", test_cases, num_test_cases);
}

TEST(MemchrFuncTest, LongRandomStringCheck)
{
    std::vector<size_t> lengths_to_test = {100, 1000};
    for (auto len : lengths_to_test) {
        auto random_str = generate_random_string(len);
        char target_char = random_str[rand() % random_str.size()];

        struct TestCase test_cases[] = {
            {random_str.c_str(), target_char, len},
            {random_str.c_str(), 'Z' + 1, len}
        };
        size_t num_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);
        run_test_cases("Long Random String", test_cases, num_test_cases);
    }
}