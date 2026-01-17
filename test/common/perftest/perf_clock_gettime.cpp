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

typedef int (*clock_gettime_t)(clockid_t clk_id, struct timespec *tp);
clock_gettime_t my_clock_gettime = nullptr;
void* handle_clk = nullptr;

void init_myclockgettime()
{
    if (my_clock_gettime != nullptr) {
        return;
    }
    handle_clk = dlopen(LIB_GLIBC_TIME_NAME, RTLD_LAZY);
    if (!handle_clk) {
        std::cerr << "Error loading libkpglibc_time.so: " << dlerror() << std::endl;
        exit(EXIT_FAILURE);
    }
    my_clock_gettime = (clock_gettime_t)dlsym(handle_clk, "clock_gettime");
    if (!my_clock_gettime) {
        std::cerr << "Error finding clock_gettime: " << dlerror() << std::endl;
        dlclose(handle_clk);
        exit(EXIT_FAILURE);
    }
}

void clock_test_sys()
{
    struct timespec ts;
    for (int i = 0; i < NUM_TESTS; ++i)
        clock_gettime(CLOCK_REALTIME, &ts);
}

void clock_test_opt()
{
    struct timespec ts;
    for (int i = 0; i < NUM_TESTS; ++i)
        my_clock_gettime(CLOCK_REALTIME, &ts);
}

TEST(ClockGettimePerfTest, TimeTest)
{
    init_myclockgettime();
    struct timespec start_sys, end_sys;
    struct timespec start_opt, end_opt;
    double used_time_sys, used_time_opt;

    clock_gettime(CLOCK_REALTIME, &start_sys);
    clock_test_sys();
    clock_gettime(CLOCK_REALTIME, &end_sys);
    used_time_sys = (end_sys.tv_sec - start_sys.tv_sec) * 1000.0 + (end_sys.tv_nsec - start_sys.tv_nsec) / 1000000.0;
    printf("test sys_func, time spent:%.6f ms\n", used_time_sys);

    clock_gettime(CLOCK_REALTIME, &start_opt);
    clock_test_opt();
    clock_gettime(CLOCK_REALTIME, &end_opt);
    used_time_opt = (end_opt.tv_sec - start_opt.tv_sec) * 1000.0 + (end_opt.tv_nsec - start_opt.tv_nsec) / 1000000.0;
    printf("test opt_func, time spent:%.6f ms\n", used_time_opt);

    struct timespec ts, ts_tmp;
    clock_gettime(CLOCK_REALTIME, &ts);
    int i = 0;
    i++;
    my_clock_gettime(CLOCK_REALTIME, &ts_tmp);
    printf("sys Time: %ld seconds, %ld nanoseconds\n", ts.tv_sec, ts.tv_nsec);
    printf("opt Time: %ld seconds, %ld nanoseconds\n", ts_tmp.tv_sec, ts_tmp.tv_nsec);
    EXPECT_LE(used_time_opt, used_time_sys);
    
    double avg_opt_ratio = (double)(used_time_sys - used_time_opt) / std::min(used_time_sys, used_time_opt);
    std::cout << "Average performance improvement ratio: " << avg_opt_ratio << std::endl;
    my_clock_gettime = nullptr;
    if (dlclose(handle_clk) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}