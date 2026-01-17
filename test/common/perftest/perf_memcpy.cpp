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
 
// 编译测试代码的时候要加O2优化，优先使用preload方法覆盖系统memcpy。
#include "mytest.h"

typedef void* (*memcpy_t)(void *dest, const void *src, size_t n);
memcpy_t my_memcpy = nullptr;
void* handle_memcpy = nullptr;

void init_memcpy() {
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
        fprintf(stderr, "Error locating memcpy: %s\n", dlerror());
        dlclose(handle_memcpy);
        exit(EXIT_FAILURE);
    }
}

void test_memcpy_performance(const size_t sizes[], size_t num_sizes)
{
    double total_opt_ratio = 0;

    struct timespec start, end;
    long long total_time_ns;

    for (size_t i = 0; i < num_sizes; ++i) {
        size_t size = sizes[i];
        char* src = (char *)malloc(size);
        char* dst = (char *)malloc(size);
        if (src == NULL || dst == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        memset(src,'A',size);
        memset(dst,'B',size);

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int j = 0; j < NUM_TESTS; ++j) {
            memcpy(dst, src, size);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_time_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
        double avg_sys = (double)total_time_ns / NUM_TESTS;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int j = 0; j < NUM_TESTS; ++j) {
            my_memcpy(dst, src, size);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_time_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
        double avg_opt = (double)total_time_ns / NUM_TESTS;
        printf("Performance of memcpy for length: %zu B,sys:  %.3f ns, opt: %.3f ns\n", size, avg_sys, avg_opt);
        double opt_ratio = (double)(avg_sys - avg_opt) / std::min(avg_sys, avg_opt);
        total_opt_ratio += opt_ratio;
        free(src);
        free(dst);
    }
    double avg_opt_ratio = total_opt_ratio / num_sizes;
    std::cout << "Average performance improvement ratio: " << avg_opt_ratio << std::endl;
}

TEST(MemcpyPerfTest, TimeTest)
{
    init_memcpy();
    size_t sizes[] = {16, 32, 64, 65, 128, 180, 256, 512, 1024, 2048, 4096, 65536, 2*1024*1024};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    test_memcpy_performance(sizes, num_sizes);
    my_memcpy = nullptr;
    if (dlclose(handle_memcpy) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}