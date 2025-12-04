#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define DATA_SIZE (10 * 1024)
#define MEMCPY_ITERATIONS (1000 * 10000)
#define MEMSET_ITERATIONS (1000 * 10000)
#define MEMCMP_ITERATIONS (1000 * 1000)

volatile int prevent_optimization = 0;

double get_time_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void test_memcpy() {
    printf("\n=== memcpy性能测试 ===\n");
    
    char *src = malloc(DATA_SIZE);
    char *dst = malloc(DATA_SIZE);
    
    if (!src || !dst) {
        printf("内存分配失败\n");
        return;
    }
    
    memset(src, 0xAA, DATA_SIZE);
    
    printf("数据大小: %d KB, 迭代次数: %d\n", DATA_SIZE / 1024, MEMCPY_ITERATIONS);
    
    double start_time = get_time_sec();
    
    for (int i = 0; i < MEMCPY_ITERATIONS; i++) {
        memcpy(dst, src, DATA_SIZE);
    }
    
    double end_time = get_time_sec();
    double total_time = end_time - start_time;
    
    size_t total_bytes = (size_t)DATA_SIZE * MEMCPY_ITERATIONS;
    double throughput = total_bytes / (1024.0 * 1024.0 * total_time);
    
    printf("测试完成!\n");
    printf("总时间: %.2f 秒\n", total_time);
    printf("吞吐量: %.2f MB/s\n", throughput);

    if (memcmp(dst, src, DATA_SIZE) == 0) {
        printf("验证: memcpy结果正确\n");
    } else {
        printf("验证: memcpy结果错误!\n");
    }
    
    free(src);
    free(dst);
}

void test_memset() {
    printf("\n=== memset性能测试 ===\n");
    
    char *buf = malloc(DATA_SIZE);
    
    if (!buf) {
        printf("内存分配失败\n");
        return;
    }
    
    printf("数据大小: %d KB, 迭代次数: %d\n", DATA_SIZE / 1024, MEMSET_ITERATIONS);
    
    double start_time = get_time_sec();
    
    for (int i = 0; i < MEMSET_ITERATIONS; i++) {
        // 交替使用不同的填充值，测试不同分支
        char fill_value = (i % 4 == 0) ? 0x00 : 
                         (i % 4 == 1) ? 0xFF : 
                         (i % 4 == 2) ? 0xAA : 0x55;
        memset(buf, fill_value, DATA_SIZE);
        
        // 防止编译器优化
        if (buf[0] == 0x77) {
            prevent_optimization++;
        }
    }
    
    double end_time = get_time_sec();
    double total_time = end_time - start_time;
    
    size_t total_bytes = (size_t)DATA_SIZE * MEMSET_ITERATIONS;
    double throughput = total_bytes / (1024.0 * 1024.0 * total_time);
    
    printf("测试完成!\n");
    printf("总时间: %.2f 秒\n", total_time);
    printf("吞吐量: %.2f MB/s\n", throughput);
    
    if (buf[0] == 0x55 && buf[DATA_SIZE-1] == 0x55) {
        printf("验证: memset结果正确\n");
    } else {
        printf("验证: memset结果错误!\n");
    }
    
    free(buf);
}

void test_memcmp() {
    printf("\n=== memcmp性能测试 (长时间运行版) ===\n");
    
    char *buf1 = malloc(DATA_SIZE);
    char *buf2 = malloc(DATA_SIZE);
    
    if (!buf1 || !buf2) {
        printf("内存分配失败\n");
        return;
    }

    memset(buf1, 0xAA, DATA_SIZE);
    memset(buf2, 0xAA, DATA_SIZE);
    
    printf("数据大小: %d KB, 迭代次数: %d\n", DATA_SIZE / 1024, MEMCMP_ITERATIONS);
    printf("测试将运行较长时间，请耐心等待...\n");
    
    double start_time = get_time_sec();
    int result = 0;
    
    for (int i = 0; i < MEMCMP_ITERATIONS; i++) {
        // 交替使用相同和不同的比较，测试不同分支
        if (i % 2 == 0) {
            result += memcmp(buf1, buf2, DATA_SIZE);
        } else {
            // 创建不同的比较场景
            if (i % 1000 == 0) {
                buf2[DATA_SIZE/2] = 0xBB;
            } else {
                buf2[DATA_SIZE/2] = 0xAA;
            }
            result += memcmp(buf1, buf2, DATA_SIZE);
        }
        
        // 防止循环被优化掉
        if (result > 1000000) {
            prevent_optimization = result;
            result = 0;
        }
    }
    
    double end_time = get_time_sec();
    double total_time = end_time - start_time;
    
    printf("测试完成! 总时间: %.2f 秒\n", total_time);
    printf("防止优化变量: %d\n", prevent_optimization);
    
    free(buf1);
    free(buf2);
}

int main() {
    printf("=== 内存函数性能测试套件 ===\n");
    printf("用于perf分析memcpy、memset、memcmp的分支执行情况\n\n");
    
    // 运行memcpy测试
    test_memcpy();
    
    // 运行memset测试  
    test_memset();
    
    // 运行memcmp测试
    test_memcmp();
    
    printf("\n=== 所有测试完成 ===\n");
    return 0;
}