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

struct timespec get_time_clk()
{
    struct timespec ts;
    my_clock_gettime(CLOCK_REALTIME, &ts);
    return ts;
}

double TimeDiffInUs(const struct timespec& start, const struct timespec& end)
{
    long sec_diff = end.tv_sec - start.tv_sec;
    long nsec_diff = end.tv_nsec - start.tv_nsec;
    return sec_diff * 1000000.0 + nsec_diff / 1000.0;
}

// 测试用例1：验证单线程情况下时间递增性、不重复性
TEST(ClockGetTimeFuncTest, SingleThreadTimeIncrese)
{
    init_myclockgettime();
    struct timespec prev_time = get_time_clk();
    for (int i = 0; i < NUM_TESTS; ++i) {
        struct timespec current_time = get_time_clk();
        ASSERT_LE(prev_time.tv_sec, current_time.tv_sec);
        if (prev_time.tv_sec == current_time.tv_sec) {
            ASSERT_LT(prev_time.tv_nsec, current_time.tv_nsec);
        }
        prev_time = current_time;
    }
    my_clock_gettime = nullptr;
    if (dlclose(handle_clk) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

// 测试用例2：验证多线程情况下时间递增性、不重复性
TEST(ClockGetTimeFuncTest, MultiThreadTimeIncrese)
{
    init_myclockgettime();
    const int num_threads = 500;
    auto thread_test = [&]() {
        struct timespec prev_time = get_time_clk();
        for (int i = 0; i < NUM_TESTS; ++i) {
            struct timespec current_time = get_time_clk();
            ASSERT_LE(prev_time.tv_sec, current_time.tv_sec);
            if (prev_time.tv_sec == current_time.tv_sec) {
                ASSERT_LT(prev_time.tv_nsec, current_time.tv_nsec);
            }
            prev_time = current_time;
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread(thread_test));
    }
    for (auto &t : threads) {
        t.join();
    }
    my_clock_gettime = nullptr;
    if (dlclose(handle_clk) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

// 测试用例3：验证优化后的 gettimeofday 获取的时间接近系统时间
TEST(ClockGetTimeFuncTest, TimeAccuracyTestWithRandomWorkload)
{
    init_myclockgettime();
    struct timespec start_system, end_system;
    struct timespec start_optimized, end_optimized;
    for (int i = 0; i < NUM_TESTS / 1000; i++)
    {
        // 生成随机的循环次数，模拟不同的计算时间1us 到 100us
        srand(time(nullptr));
        int sleep_ns = rand() % 100000 + 1000;
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = sleep_ns;
        
        clock_gettime(CLOCK_REALTIME, &start_system);
        nanosleep(&ts, nullptr);
        clock_gettime(CLOCK_REALTIME, &end_system);

        my_clock_gettime(CLOCK_REALTIME, &start_optimized);
        nanosleep(&ts, nullptr);
        my_clock_gettime(CLOCK_REALTIME, &end_optimized);

        double system_diff = TimeDiffInUs(start_system, end_system);
        double optimized_diff = TimeDiffInUs(start_optimized, end_optimized);

        ASSERT_NEAR(system_diff, optimized_diff, 5000);  // 需要综合考虑系统调度、负载带来的误差，设置为5ms误差
    }
    my_clock_gettime = nullptr;
        if (dlclose(handle_clk) != 0) {
            std::cerr << "dlclose failed: " << dlerror() << std::endl;
        }
}

TEST(ClockGetTimeFuncTest, NULLTest)
{
    init_myclockgettime();
    int ret = my_clock_gettime(CLOCK_REALTIME, nullptr);
    EXPECT_EQ(ret, KPGLIBC_STS_PARAMETER_ERR);
    my_clock_gettime = nullptr;
    if (dlclose(handle_clk) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(ClockGetTimeFuncTest, OtherClockSourceTest)
{
    init_myclockgettime();
    struct timespec ts, ts2;
    int ret = my_clock_gettime(CLOCK_MONOTONIC, &ts);
    EXPECT_EQ(ret, clock_gettime(CLOCK_MONOTONIC, &ts2));
    EXPECT_EQ(ts.tv_sec, ts2.tv_sec);
    
    my_clock_gettime = nullptr;
    if (dlclose(handle_clk) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}