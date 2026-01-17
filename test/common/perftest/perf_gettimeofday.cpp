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

typedef int (*gettimeofday_t)(struct timeval*, struct timezone*);
gettimeofday_t my_gettimeofday = nullptr;
void* handle_gettime = nullptr;

void init_mygettimeofday()
{
    if (my_gettimeofday != nullptr) {
        return;
    }
    handle_gettime = dlopen(LIB_GLIBC_TIME_NAME, RTLD_LAZY);
    if (!handle_gettime) {
        std::cerr << "Error loading libkpglibc_time.so: " << dlerror() << std::endl;
        exit(EXIT_FAILURE);
    }
    my_gettimeofday = (gettimeofday_t)dlsym(handle_gettime, "gettimeofday");
    if (!my_gettimeofday) {
        std::cerr << "Error finding gettimeofday: " << dlerror() << std::endl;
        dlclose(handle_gettime);
        exit(EXIT_FAILURE);
    }
}

void time_test_sys()
{
    struct timeval tv;
    for (int i = 0; i < NUM_TESTS; ++i)
        gettimeofday(&tv, NULL);
}
void time_test_opt()
{
    struct timeval tv;
    for (int i = 0; i < NUM_TESTS; ++i)
        my_gettimeofday(&tv, NULL);
}

TEST(GettimeofdayPerfTest, TimeTest)
{
    init_mygettimeofday();
    struct timespec start_sys, end_sys;
    struct timespec start_opt, end_opt;
    double used_time_sys, used_time_opt;

    clock_gettime(CLOCK_REALTIME, &start_sys);
    time_test_sys();
    clock_gettime(CLOCK_REALTIME, &end_sys);
    used_time_sys = (end_sys.tv_sec - start_sys.tv_sec) * 1000.0 + (end_sys.tv_nsec - start_sys.tv_nsec) / 1000000.0;
    printf("test sys_func, time spent:%.6f ms\n", used_time_sys);

    clock_gettime(CLOCK_REALTIME, &start_opt);
    time_test_opt();
    clock_gettime(CLOCK_REALTIME, &end_opt);
    used_time_opt = (end_opt.tv_sec - start_opt.tv_sec) * 1000.0 + (end_opt.tv_nsec - start_opt.tv_nsec) / 1000000.0;
    printf("test opt_func, time spent:%.6f ms\n", used_time_opt);

    struct timeval tv, tv_tmp;
    gettimeofday(&tv, NULL);
    my_gettimeofday(&tv_tmp, NULL);
    printf("sys Time: %ld seconds, %ld us\n", tv.tv_sec, tv.tv_usec);
    printf("opt Time: %ld seconds, %ld us\n", tv_tmp.tv_sec, tv_tmp.tv_usec);
    EXPECT_LE(used_time_opt, used_time_sys);

    double avg_opt_ratio = (double)(used_time_sys - used_time_opt) / std::min(used_time_sys, used_time_opt);
    std::cout << "Average performance improvement ratio: " << avg_opt_ratio << std::endl;
    my_gettimeofday = nullptr;
    if (dlclose(handle_gettime) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}