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

void test_strcmp_sys(char *str1, char *str2)
{
    for (int i = 0; i < NUM_TESTS; ++i) {
        n_strcmp = strcmp(str1, str2);
    }
}

void test_strcmp_opt(char *str1, char *str2)
{
    for (int i = 0; i < NUM_TESTS; ++i) {
        n_strcmp = my_strcmp(str1, str2);
    }
}

TEST(StrcmpPerfTest, TimeTest)
{
    init_mystrcmp();
    double opt_ratio = 0, total_opt_ratio = 0;
    size_t lengths[] = {64, 65, 2048, 4096, 65536, 2*1024*1024};
    int num_lengths = sizeof(lengths) / sizeof(lengths[0]);
    char *str1, *str2;
    for (int i = 0; i < num_lengths; i++) {
        size_t len = lengths[i];
        str1 = (char *)malloc(len);
        str2 = (char *)malloc(len);
        if (str1 == NULL || str2 == NULL) {
            printf("Memory allocation failed!\n");
            return;
        }
        struct timespec start_sys, end_sys;
        struct timespec start_opt, end_opt;
        double time_sys, time_opt;

        memset(str1, 'A', len-1);
        str1[len-1] = '\0';
        memcpy(str2, str1, len);
        
        clock_gettime(CLOCK_REALTIME, &start_sys);
        test_strcmp_sys(str1, str2);
        clock_gettime(CLOCK_REALTIME, &end_sys);
        time_sys = calculate_time_ms(start_sys, end_sys);

        clock_gettime(CLOCK_REALTIME, &start_opt);
        test_strcmp_opt(str1, str2);
        clock_gettime(CLOCK_REALTIME, &end_opt);
        time_opt = calculate_time_ms(start_opt, end_opt);
        printf("Performance of strcmp for length %zuB, ident str:sys: %.6f ms, opt: %.6f ms\n", lengths[i], time_sys, time_opt);

        opt_ratio = (double)(time_sys - time_opt) / std::min(time_sys, time_opt);
        total_opt_ratio += opt_ratio;

        free(str1);
        free(str2);
    }
    double avg_opt_ratio = total_opt_ratio / num_lengths;
    std::cout << "Average performance improvement ratio: " << avg_opt_ratio << std::endl;
    my_strcmp = nullptr;
    if (dlclose(handle_strcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}