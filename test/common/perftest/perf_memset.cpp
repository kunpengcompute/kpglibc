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

typedef void* (*memset_t)(void *s, int c, size_t n);
memset_t my_memset = nullptr;
void* handle_memset = nullptr;

void init_memset()
{
    if (my_memset != nullptr) {
        return;
    }
    handle_memset = dlopen(LIB_GLIBC_NAME, RTLD_LAZY);
    if (!handle_memset) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    my_memset = (memset_t)dlsym(handle_memset, "memset");
    if (!my_memset) {
        fprintf(stderr, "Error locating memset: %s\n", dlerror());
        dlclose(handle_memset);
        exit(EXIT_FAILURE);
    }
}

TEST(MemsetPerfTest, TimeTest)
{
    init_memset();
    size_t sizes[] = {16, 32, 64, 65, 128, 180, 256, 512, 1024, 2048, 4096, 65536, 2*1024*1024};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    double total_opt_ratio = 0;
    struct timespec start_sys, end_sys;
    struct timespec start_opt, end_opt;
    long long total_time_ns;

    for (size_t i = 0; i < num_sizes; ++i) {
        size_t size = sizes[i];
        char *dst1 = (char*)malloc(size);

        if (dst1 == nullptr) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        drop_cache();
        clock_gettime(CLOCK_REALTIME, &start_sys);
        for (int j = 0; j < NUM_TESTS; ++j) {
            memset(dst1, 0, size);
        }
        clock_gettime(CLOCK_REALTIME, &end_sys);
        total_time_ns = (end_sys.tv_sec - start_sys.tv_sec) * 1000000000LL + (end_sys.tv_nsec - start_sys.tv_nsec);
        double avg_sys = (double)total_time_ns / NUM_TESTS;

        drop_cache();
        clock_gettime(CLOCK_REALTIME, &start_opt);
        for (int j = 0; j < NUM_TESTS; ++j) {
            my_memset(dst1, 0, size);
        }
        clock_gettime(CLOCK_REALTIME, &end_opt);
        total_time_ns = (end_opt.tv_sec - start_opt.tv_sec) * 1000000000LL + (end_opt.tv_nsec - start_opt.tv_nsec);
        double avg_opt = (double)total_time_ns / NUM_TESTS;

        printf("Performance of memset for length: %zu B, sys: %.3f ns, opt: %.3f ns\n", size, avg_sys, avg_opt);
        double opt_ratio = (double)(avg_sys - avg_opt) / std::min(avg_sys, avg_opt);
        total_opt_ratio += opt_ratio;
        free(dst1);
    }
    double avg_opt_ratio = total_opt_ratio / num_sizes;
    std::cout << "Average performance improvement ratio: " << avg_opt_ratio << std::endl;
    my_memset = nullptr;
    if (dlclose(handle_memset) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}