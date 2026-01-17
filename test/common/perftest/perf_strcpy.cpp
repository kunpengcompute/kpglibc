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

void init_mystrcpy() {
    if (my_strcpy != nullptr) {
        return;
    }

    handle_strcpy = dlopen(LIB_GLIBC_NAME, RTLD_LAZY);
    if (!handle_strcpy) {
        std::cerr << "Error loading libkpglibc.so strcpy: " << dlerror() << std::endl;
        exit(EXIT_FAILURE);
    }

    my_strcpy = (strcpy_t)dlsym(handle_strcpy, "strcpy");
    if (!my_strcpy) {
        std::cerr << "Error locating libkpglibc.so strcpy: " << dlerror() << std::endl;
        dlclose(handle_strcpy);
        exit(EXIT_FAILURE);
    }
}

void test_strcpy_sys(char *destination, const char *source)
{
    for (int i = 0; i < NUM_TESTS; ++i) {
        strcpy(destination, source);
    }
}

void test_strcpy_opt(char *destination, const char *source)
{
    for (int i = 0; i < NUM_TESTS; ++i) {
        my_strcpy(destination, source);
    }
}

TEST(StrcpyPerfTest, TimeTest)
{
    init_mystrcpy();
    double total_opt_ratio = 0.0;
    size_t lengths[] = {64, 65, 2048, 4096, 65536, 2*1024*1024};
    int num_lengths = sizeof(lengths) / sizeof(lengths[0]);
    char *destination = (char *)malloc(2*1024*1024);
    if (destination == NULL) {
        perror("Failed to allocate memory for destination");
        return;
    }
    for (int i = 0; i < num_lengths; ++i) {
        drop_cache();
        char *source = (char *)malloc(lengths[i] + 1);
        if (source == NULL) {
            perror("Failed to allocate memory for source");
            free(destination);
            return;
        }
        memset(source, 'A', lengths[i]);
        source[lengths[i]] = '\0';
        struct timespec start_sys, end_sys;
        clock_gettime(CLOCK_REALTIME, &start_sys);
        test_strcpy_sys(destination, source);
        clock_gettime(CLOCK_REALTIME, &end_sys);
        double time_sys = calculate_time_ms(start_sys, end_sys);

        struct timespec start_opt, end_opt;
        clock_gettime(CLOCK_REALTIME, &start_opt);
        test_strcpy_opt(destination, source);
        clock_gettime(CLOCK_REALTIME, &end_opt);
        double time_opt = calculate_time_ms(start_opt, end_opt);
        printf("Performance of strcpy for length %zuB: sys: %.6f ms, opt: %.6f ms\n", lengths[i], time_sys, time_opt);

        double opt_ratio = (double)(time_sys - time_opt) / std::min(time_sys, time_opt);
        total_opt_ratio += opt_ratio;
        free(source);
    }
    double avg_opt_ratio = total_opt_ratio / num_lengths;
    std::cout << "Average performance improvement ratio: " << avg_opt_ratio << std::endl;
    free(destination);
    my_strcpy = nullptr;
    if (dlclose(handle_strcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}