/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
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

typedef void* (*memmove_t)(void *dest, const void *src, size_t n);
static memmove_t my_memmove = nullptr;
static void* handle_memmove = nullptr;

void init_mymemmove() {
    if (my_memmove != nullptr) return;
    handle_memmove = dlopen(LIB_GLIBC_NAME, RTLD_LAZY);
    if (!handle_memmove) {
        fprintf(stderr, "Error loading kpglibc: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    my_memmove = (memmove_t)dlsym(handle_memmove, "memmove");
    if (!my_memmove) {
        fprintf(stderr, "Error locating my_memmove: %s\n", dlerror());
        dlclose(handle_memmove);
        exit(EXIT_FAILURE);
    }
}

void fill_unique_data(uint8_t* buf, size_t size, uint32_t seed) {
    for (size_t i = 0; i < size; ++i) {
        buf[i] = static_cast<uint8_t>((i + seed) & 0xFF);
    }
}

TEST(MemmoveFuncTest, ComprehensiveOverlapAndLength) {
    init_mymemmove();

    const size_t MAX_LEN = 2048;
    const size_t MAX_OFF = 32;
    const size_t BUF_SIZE = MAX_LEN + MAX_OFF*2 + 64;

    uint8_t* test_buf = (uint8_t*)malloc(BUF_SIZE);
    uint8_t* golden_buf = (uint8_t*)malloc(BUF_SIZE);
    ASSERT_TRUE(test_buf && golden_buf);

    for (size_t n = 0; n <= MAX_LEN; ++n) {
        for (size_t s_off = 0; s_off <= MAX_OFF; ++s_off) {
            for (size_t d_off = 0; d_off <= MAX_OFF; ++d_off) {
                uint32_t seed = (uint32_t)(n + s_off + d_off);
                fill_unique_data(test_buf, BUF_SIZE, seed);
                memcpy(golden_buf, test_buf, BUF_SIZE);

                my_memmove(test_buf + d_off, test_buf + s_off, n);
                memmove(golden_buf + d_off, golden_buf + s_off, n);

                if (memcmp(test_buf, golden_buf, BUF_SIZE) != 0) {
                    printf("FAILED: n=%zu, s_off=%zu, d_off=%zu\n", n, s_off, d_off);
                    for(size_t i=0; i<BUF_SIZE; ++i) {
                        if(test_buf[i] != golden_buf[i]) {
                            printf("First mismatch at index %zu: Expected 0x%02X, Got 0x%02X\n", 
                                   i, golden_buf[i], test_buf[i]);
                            break;
                        }
                    }
                    FAIL();
                }
            }
        }
    }

    free(test_buf);
    free(golden_buf);
    my_memmove = nullptr;
    if (dlclose(handle_memmove) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}

TEST(MemmoveFuncTest, BoundaryProtection) {
    init_mymemmove();
    const size_t page_size = sysconf(_SC_PAGESIZE);
    const size_t MAX_LEN = 2048;
    const size_t MAX_OFF = 32;

    // [Guard1] [Data Pages] [Guard2]
    size_t data_pages_count = ((MAX_LEN + MAX_OFF) / page_size) + 2;
    size_t total_size = (data_pages_count + 2) * page_size;

    uint8_t* base = (uint8_t*)mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(base, MAP_FAILED);

    uint8_t* guard1 = base;
    uint8_t* data_zone = base + page_size;
    uint8_t* guard2 = base + total_size - page_size;

    mprotect(guard1, page_size, PROT_NONE);
    mprotect(guard2, page_size, PROT_NONE);

    for (size_t n = 0; n <= MAX_LEN; ++n) {
        for (size_t s_off = 0; s_off <= MAX_OFF; ++s_off) {
            for (size_t d_off = 0; d_off <= MAX_OFF; ++d_off) {

                // right overflow test
                size_t tail_reach = (s_off > d_off ? s_off : d_off) + n;
                uint8_t* edge_base = guard2 - tail_reach;
                uint8_t* cur_src = edge_base + s_off;
                uint8_t* cur_dst = edge_base + d_off;
                my_memmove(cur_dst, cur_src, n);

                // left overflow test
                size_t head_start = (s_off < d_off ? s_off : d_off);
                uint8_t* start_base = data_zone - head_start;
                cur_src = start_base + s_off;
                cur_dst = start_base + d_off;
                my_memmove(cur_dst, cur_src, n);
            }
        }
    }

    munmap(base, total_size);
    my_memmove = nullptr;
    if (dlclose(handle_memmove) != 0) {
        std::cerr << "dlclose failed: " << dlerror() << std::endl;
    }
}