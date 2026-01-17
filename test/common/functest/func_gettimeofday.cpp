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

struct timeval get_time()
{
    struct timeval tv;
    my_gettimeofday(&tv, nullptr);
    return tv;
}

double TimeDiffInUs(const struct timeval& start, const struct timeval& end)
{
    long sec_diff = end.tv_sec - start.tv_sec;
    long usec_diff = end.tv_usec - start.tv_usec;
    return sec_diff * 1000000.0 + usec_diff;
}

// 测试用例1：验证单线程情况下时间递增性
TEST(GetTimeOfDayFuncTest, SingleThreadTimeIncrese)
{
    init_mygettimeofday();
    struct timeval prev_time = get_time();
    for (int i = 0; i < NUM_TESTS; ++i) {
        struct timeval current_time = get_time();
        ASSERT_LE(prev_time.tv_sec, current_time.tv_sec);
        if (prev_time.tv_sec == current_time.tv_sec) {
            ASSERT_LE(prev_time.tv_usec, current_time.tv_usec);
        }
        prev_time = current_time;
    }
    my_gettimeofday = nullptr;
    if (dlclose(handle_gettime) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

// 测试用例2：验证多线程情况下时间递增性
TEST(GetTimeOfDayFuncTest, MultiThreadTimeIncrese)
{
    init_mygettimeofday();
    const int num_threads = 500;
    auto thread_test = [&]() {
        struct timeval prev_time = get_time();
        for (int i = 0; i < NUM_TESTS; ++i) {
            struct timeval current_time = get_time();
            ASSERT_LE(prev_time.tv_sec, current_time.tv_sec);
            if (prev_time.tv_sec == current_time.tv_sec) {
                ASSERT_LE(prev_time.tv_usec, current_time.tv_usec);
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
    my_gettimeofday = nullptr;
    if (dlclose(handle_gettime) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

// 测试用例3：验证优化后的 gettimeofday 获取的时间接近系统时间
TEST(GetTimeOfDayFuncTest, TimeAccuracyTestWithRandomWorkload)
{
    init_mygettimeofday();
    struct timeval start_system, end_system;
    struct timeval start_optimized, end_optimized;
    for (int i = 0; i < NUM_TESTS / 1000; i++)
    {
        // 生成随机的循环次数，模拟不同的计算时间1us 到 100us
        srand(time(nullptr));
        int sleep_ns = rand() % 100000 + 1000;
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = sleep_ns;
        gettimeofday(&start_system, nullptr);
        nanosleep(&ts, nullptr);
        gettimeofday(&end_system, nullptr);

        my_gettimeofday(&start_optimized, nullptr);
        nanosleep(&ts, nullptr);
        my_gettimeofday(&end_optimized, nullptr);

        double system_diff = TimeDiffInUs(start_system, end_system);
        double optimized_diff = TimeDiffInUs(start_optimized, end_optimized);

        ASSERT_NEAR(system_diff, optimized_diff, 5000);  // 需要综合考虑系统调度、负载带来的误差，设置为5ms误差
    }
    my_gettimeofday = nullptr;
    if (dlclose(handle_gettime) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}