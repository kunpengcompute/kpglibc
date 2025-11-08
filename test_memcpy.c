#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define DATA_SIZE (10 * 1024)  // 10KB
#define ITERATIONS (1000 * 10000)

double get_time_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main() {
    printf("=== memcpy性能测试Demo ===\n");
    
    char *src = malloc(DATA_SIZE);
    char *dst = malloc(DATA_SIZE);
    
    if (!src || !dst) {
        printf("内存分配失败\n");
        return 1;
    }
    
    memset(src, 0xAA, DATA_SIZE);
    
    printf("数据大小: %d KB, 迭代次数: %d\n", DATA_SIZE / 1024, ITERATIONS);
    
    double start_time = get_time_sec();
    
    for (int i = 0; i < ITERATIONS; i++) {
        memcpy(dst, src, DATA_SIZE);
    }
    
    double end_time = get_time_sec();
    double total_time = end_time - start_time;
    
    size_t total_bytes = (size_t)DATA_SIZE * ITERATIONS;
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
    return 0;
}
