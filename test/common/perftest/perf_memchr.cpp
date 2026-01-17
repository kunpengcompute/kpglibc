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
 
typedef void* (*memchr_t)(const void *s, int c, size_t n);
memchr_t my_memchr = nullptr;
void* handle_memchr = nullptr;
 
void init_memchr() {
    if (my_memchr != nullptr) {
        return;
    }
    handle_memchr = dlopen(LIB_GLIBC_NAME, RTLD_LAZY);
    if (!handle_memchr) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
 
    my_memchr = (memchr_t)dlsym(handle_memchr, "memchr");
    if (!my_memchr) {
        fprintf(stderr, "Error locating memchr: %s\n", dlerror());
        dlclose(handle_memchr);
        exit(EXIT_FAILURE);
    }
}
 
void test_memchr_performance(const size_t sizes[], size_t num_sizes)
{
    double total_opt_ratio = 0;
    char *src1 = nullptr;
    char *src2 = nullptr;
    struct timespec start_sys, end_sys;
    struct timespec start_opt, end_opt;
    long long total_time_ns;
    const unsigned char search_char = 'A';
 
    for (size_t i = 0; i < num_sizes; ++i) {
        size_t size = sizes[i];
        src1 = (char*)malloc(size);
        src2 = (char*)malloc(size);
        if (src1 == NULL || src2 == NULL) {
            fprintf(stderr, "Memory allocation failed for size: %zu\n", size);
            exit(EXIT_FAILURE);
        }
 
        memset(src1, 'B', size - 1);
        memset(src2, 'B', size - 1);
        src1[size - 1] = search_char;
        src2[size - 1] = search_char;

        static volatile char *result_sys = nullptr;
        clock_gettime(CLOCK_REALTIME, &start_sys);
        for (int j = 0; j < NUM_TESTS; ++j) {
            result_sys =(char *) memchr(src1, search_char, size);
        }
        clock_gettime(CLOCK_REALTIME, &end_sys);
        total_time_ns = (end_sys.tv_sec - start_sys.tv_sec) * 1000000000LL + (end_sys.tv_nsec - start_sys.tv_nsec);
        double avg_sys = (double)total_time_ns / NUM_TESTS;

        static volatile char *result_opt = nullptr;
        clock_gettime(CLOCK_REALTIME, &start_opt);
        for (int j = 0; j < NUM_TESTS; ++j) {
            result_opt = (char *)my_memchr(src2, search_char, size);
        }
        clock_gettime(CLOCK_REALTIME, &end_opt);
        if (result_sys != nullptr && result_opt != nullptr && *result_opt != *result_sys)
        {
            printf("search failed!, opt res:%c,sys res:%c \n",*result_opt,*result_sys);
        }
        total_time_ns = (end_opt.tv_sec - start_opt.tv_sec) * 1000000000LL + (end_opt.tv_nsec - start_opt.tv_nsec);
        double avg_opt = (double)total_time_ns / NUM_TESTS;

        printf("Performance of memchr for length: %zu B, sys: %.3f ns, opt: %.3f ns\n", size, avg_sys, avg_opt);
        double opt_ratio = (double)(avg_sys - avg_opt) / std::min(avg_sys, avg_opt);
        total_opt_ratio += opt_ratio;
        free(src1);
        free(src2);
    }
    double avg_opt_ratio = total_opt_ratio / num_sizes;
    std::cout << "Average performance improvement ratio: " << avg_opt_ratio << std::endl;
}
 
TEST(MemchrPerfTest, TimeTest)
{
    size_t sizes[] = {64, 65, 2048, 4096, 65536, 2*1024*1024};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    init_memchr();
    test_memchr_performance(sizes, num_sizes);
    my_memchr = nullptr;
    if (dlclose(handle_memchr) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}