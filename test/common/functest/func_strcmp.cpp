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

typedef int (*strcmp_t)(const char*, const char*);
strcmp_t my_strcmp = nullptr;
void* handle_strcmp = nullptr;
int n_strcmp = 0;
void init_mystrcmp()
{
    if (my_strcmp != nullptr) {
        return;
    }

    handle_strcmp = dlopen(LIB_GLIBC_NAME, RTLD_LAZY);
    if (!handle_strcmp) {
        std::cerr << "Error loading libkpglibc.so strcmp: " << dlerror() << std::endl;
        exit(EXIT_FAILURE);
    }

    my_strcmp = (strcmp_t)dlsym(handle_strcmp, "strcmp");
    if (!my_strcmp) {
        std::cerr << "Error locating libkpglibc.so strcmp: " << dlerror() << std::endl;
        dlclose(handle_strcmp);
        exit(EXIT_FAILURE);
    }
}

void verify_strcmp_sign(const char* s1, const char* s2)
{
    const int opt_ret = my_strcmp(s1, s2);
    const int std_ret = strcmp(s1, s2);
    EXPECT_EQ(opt_ret > 0, std_ret > 0) << "Positive sign mismatch";
    EXPECT_EQ(opt_ret < 0, std_ret < 0) << "Negative sign mismatch";
    EXPECT_EQ(opt_ret == 0, std_ret == 0) << "Zero equality mismatch";
}

TEST(StrcmpFuncTest, IdenticalStrings)
{
    init_mystrcmp();
    const char* str1 = "hello";
    const char* str2 = "hello";
    verify_strcmp_sign(str1, str2);
    verify_strcmp_sign(str2, str1);
    my_strcmp = nullptr;
    if (dlclose(handle_strcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(StrcmpFuncTest, HalfIdenticalStrings)
{
    init_mystrcmp();
    const char* str1 = "hello world";
    const char* str2 = "hello there";
    verify_strcmp_sign(str1, str2);
    verify_strcmp_sign(str2, str1);
    my_strcmp = nullptr;
    if (dlclose(handle_strcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(StrcmpFuncTest, DifferentStrings)
{
    init_mystrcmp();
    const char* str1 = "apple";
    const char* str2 = "banana";
    verify_strcmp_sign(str1, str2);
    verify_strcmp_sign(str2, str1);
    my_strcmp = nullptr;
    if (dlclose(handle_strcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(StrcmpFuncTest, NullTerminatedInMiddle) {
    init_mystrcmp();
    const char str1[] = {'a', 'p', 'p', '\0', 'x', 'y'};  // "app\0xy"
    const char str2[] = {'a', 'p', 'p', '\0', 'a', 'b'};  // "app\0ab"
    const char str3[] = {'a', 'p', '\0', 'q', 'r', 's'};   // "ap\0qrs"
    const char str4[] = {'a', 'p', 'q', '\0', 's', 't'};   // "apq\0st"

    verify_strcmp_sign(str1, str2);
    verify_strcmp_sign(str1, str3);
    verify_strcmp_sign(str3, str1);
    verify_strcmp_sign(str3, str4);
    
    my_strcmp = nullptr;
    if (dlclose(handle_strcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(StrcmpFuncTest, EmptyStrings)
{
    init_mystrcmp();
    const char* str1 = "";
    const char* str2 = "";
    verify_strcmp_sign(str1, str2);
    verify_strcmp_sign(str2, str1);
    my_strcmp = nullptr;
    if (dlclose(handle_strcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(StrcmpFuncTest, NonEmptyVsEmpty)
{
    init_mystrcmp();
    const char* str1 = "hello";
    const char* str2 = "";
    verify_strcmp_sign(str1, str2);
    verify_strcmp_sign(str2, str1);
    my_strcmp = nullptr;
    if (dlclose(handle_strcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}