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
void* handle_memcmp = nullptr;
int n_memcmp = 0;
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

void test_memcmp_sys(char *str1, char *str2, int len)
{
    for (int i = 0; i < NUM_TESTS; ++i) {
        n_memcmp = memcmp(str1, str2, len);
    }
}

void test_memcmp_opt(char *str1, char *str2, int len)
{
    for (int i = 0; i < NUM_TESTS; ++i) {
        n_memcmp = my_memcmp(str1, str2, len);
    }
}

TEST(MemcmpPerfTest, TimeTest)
{
    init_mymemcmp();
    double opt_ratio = 0, total_opt_ratio = 0;
    size_t lengths[] = {64, 65, 2048, 4096, 65536, 2*1024*1024};
    int num_lengths = sizeof(lengths) / sizeof(lengths[0]);
    char *str1 = nullptr, *str2 = nullptr;
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

        memset(str1, 'F', len);
        memcpy(str2, str1, len);
        
        clock_gettime(CLOCK_REALTIME, &start_sys);
        test_memcmp_sys(str1, str2, len);
        clock_gettime(CLOCK_REALTIME, &end_sys);
        time_sys = calculate_time_ms(start_sys, end_sys);

        clock_gettime(CLOCK_REALTIME, &start_opt);
        test_memcmp_opt(str1, str2, len);
        clock_gettime(CLOCK_REALTIME, &end_opt);
        time_opt = calculate_time_ms(start_opt, end_opt);
        printf("Performance of memcmp for length %zuB, ident str:sys: %.6f ms, opt: %.6f ms\n", lengths[i], time_sys, time_opt);

        opt_ratio = (double)(time_sys - time_opt) / std::min(time_sys, time_opt);
        total_opt_ratio += opt_ratio;

        free(str1);
        free(str2);
    }
    double avg_opt_ratio = total_opt_ratio / num_lengths;
    std::cout << "Average performance improvement ratio: " << avg_opt_ratio << std::endl;
    my_memcmp = nullptr;
    if (dlclose(handle_memcmp) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}