# API参考

## 函数说明

kpglibc库已优化函数如[**表 1** kpglibc库已优化函数列表](#kpglibc库已优化函数列表)所示。

**表 1** kpglibc库已优化函数列表<a id="kpglibc库已优化函数列表"></a>

|名称|说明|
|--|--|
|strcpy|将源字符串复制到目标字符串，包括结尾的空字符（\0） 。|
|strcmp|比较两个字符串的内容，返回它们的字典顺序差异（小于、等于或大于零）。|
|memchr|在内存块中查找首次出现的指定字符，返回指向该字符的指针。|
|memcmp|比较两块内存区域的内容，返回它们的差异。|
|memcpy|将一块内存区域的内容复制到另一块内存区域，不允许源和目标区域重叠。|
|memmove|将一块内存区域的内容复制到另一块内存区域，允许源和目标区域重叠。|
|memset|将一块内存区域的所有字节设置为指定的值。|
|gettimeofday|获取当前系统时间，以秒和微秒表示。|
|clock_gettime|获取当前系统时间，以秒和纳秒表示。|

>![](public_sys-resources/icon-notice.gif) **须知：** 
>仅支持鲲鹏平台下使用。函数接口遵从Glibc原函数的入参校验逻辑，接口内部不做完整入参校验，入参合法性及合理性由调用方业务来保证。

## 函数定义

### strcpy

**函数功能**

将源字符串复制到目标字符串，包括结尾的空字符（\\0）。复制过程中不会检查目标缓冲区的大小，因此如果目标缓冲区不足以容纳源字符串，将会导致缓冲区溢出。

**函数定义**

```c
char *strcpy(char *dest, const char *src);
```

**参数说明**

|参数名|描述|取值范围|输入/输出|
|--|--|--|--|
|dest|目标内存地址指针，接收源字符串的内容。|非空指针，指向的内存空间必须可写且足够大（能容纳src所有字符+\0）|输入/输出|
|src|指向要复制的数据源内存指针。|非空指针，指向源字符串地址，必须以\0结尾（否则行为未定义，可能导致崩溃或溢出）|输入|

**返回值**

- 成功：返回目标字符串的起始地址。
- 失败：遵循开源Glibc，不返回特殊异常值。

>![](public_sys-resources/icon-note.gif) **说明：** 
>必须确保目标字符串有足够的空间来容纳源字符串（包括结尾的\\0），否则会引起内存溢出。

**示例**

```c
#include <stdio.h>
#include <string.h>

int main() {
    char src[] = "Hello, World!";
    char dest[50];

    strcpy(dest, src);
    printf("Source: %s\n", src);
    printf("Destination: %s\n", dest);
    return 0;
}
```

运行结果：

```text
Source: Hello, World!
Destination: Hello, World!
```

### strcmp

**函数功能**

比较两个字符串的字典顺序。它逐个字符比较两个字符串，直到发现不同的字符或者到达字符串结束符（\\0）。返回结果依据比较结果，返回正负值或0。

**函数定义**

```c
int strcmp(const char *str1, const char *str2);
```

**参数说明**

|参数名|描述|取值范围|输入/输出|
|--|--|--|--|
|str1|第一个字符串的指针。|非空指针，指向以\0结尾的字符串指针|输入|
|str2|第二个字符串的指针。|非空指针，指向以\0结尾的字符串指针|输入|

**返回值**

- 成功：返回值为整数。
    - 如果str1小于str2，返回负值。
    - 如果str1等于str2，返回0。
    - 如果str1大于str2，返回正值。

- 失败：遵循开源Glibc，不返回特殊异常值。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>比较区分字符的大小写，因此字符串“abc”和“ABC”是不同的。

**示例**

```c
#include <stdio.h>
#include <string.h>

int main() {
    const char *str1 = "apple";
    const char *str2 = "banana";
    const char *str3 = "apple";
    const char *str4 = "Banana";
    const char *str5 = "APPLE";
    int result1 = strcmp(str1, str2);
    printf("apple compared to banana: %d\n", result1);
    int result2 = strcmp(str1, str3);
    printf("apple compared to apple: %d\n", result2);
    int result3 = strcmp(str1, str4);
    printf("apple compared to Banana: %d\n", result3);
    int result4 = strcmp(str5, str1);
    printf("APPLE compared to apple: %d\n", result4);

    return 0;
}
```

运行结果：

```text
apple compared to banana: -60
apple compared to apple: 0
apple compared to Banana: 124
APPLE compared to apple: -128
```

### memchr

**函数功能**

在给定的内存块中查找首次出现的指定字符。该函数返回指向内存中找到字符的指针，若未找到则返回NULL。

**函数定义**

```c
void *memchr(const void *ptr, int value, size_t num);
```

**参数说明**

|参数名|描述|取值范围|输入/输出|
|--|--|--|--|
|ptr|指向内存区域的指针，搜索将在此内存区域进行。|非空指针，指向有效内存，至少有num字节可访问。|输入|
|value|要查找的字符，传入的值会转换为unsigned char类型。|0-255，超出部分会被截断|输入|
|num|要查找的字节数。|非负数，不超过实际内存大小。|输入|

**返回值**

- 成功：如果找到了字符，返回指向该字符的指针。
- 失败：遵循开源Glibc，不返回特殊异常值。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>查找是逐字节的，严格检查num个字节，不管中间是否有空字符，即该函数查找value在内存中的位置，与strchr不同。

**示例**

```c
#include <stdio.h>
#include <string.h>

int main() {
    char str[] = "Hello, World!";
    char *result;
    result = (char *)memchr(str, 'o', sizeof(str));
    if (result != NULL) {
        printf("Found 'o' at index: %ld\n", (long)(result - str));
    } else {
        printf("Did not find 'o'\n");
    }
    result = (char *)memchr(str, 'x', sizeof(str));
    if (result != NULL) {
        printf("Found 'x' at index: %ld\n", (long)(result - str));
    } else {
        printf("Did not find 'x'\n");
    }
    return 0;
}
```

运行结果：

```text
Found 'o' at index: 4
Did not find 'x'
```

### memcmp

**函数功能**

比较两块内存区域的内容，逐字节进行比较。如果两块内存的内容不同，返回它们的差异；如果相同，则返回0。

**函数定义**

```c
int memcmp(const void *ptr1, const void *ptr2, size_t num);
```

**参数说明**

|参数名|描述|取值范围|输入/输出|
|--|--|--|--|
|ptr1|指向第一块内存的指针。|非空指针，指向有效内存，至少有num字节可访问。|输入|
|ptr2|指向第二块内存的指针。|非空指针，指向有效内存，至少有num字节可访问。|输入|
|num|要比较的字节数。|非负数，不超过实际内存大小。|输入|

**返回值**

- 成功：
    - 如果两块内存相同，返回0。
    - 如果ptr1小于ptr2，返回负值。
    - 如果ptr1大于ptr2，返回正值。

- 失败：遵循开源Glibc，不返回特殊异常值。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>比较是逐字节进行的，直到发现不同字节或比较完num字节。

**示例**

```c
#include <stdio.h>
#include <string.h>

int main() {
    char str1[] = "Hello, World!";
    char str2[] = "Hello, World!";
    char str3[] = "Hello, World";

    int result1 = memcmp(str1, str2, sizeof(str1));
    if (result1 == 0) {
        printf("str1 and str2 are equal\n");
    } else {
        printf("str1 and str2 are not equal\n");
    }

    int result2 = memcmp(str1, str3, sizeof(str1));
    if (result2 != 0) {
        printf("str1 and str3 are not equal\n");
    } else {
        printf("str1 and str3 are equal\n");
    }
    return 0;
}
```

运行结果：

```text
str1 and str2 are equal
str1 and str3 are not equal
```

### memcpy

**函数功能**

将源内存区域的内容复制到目标内存区域。两块内存区域不允许重叠，否则会导致未定义行为。

**函数定义**

```c
void *memcpy(void *dest, const void *src, size_t num);
```

**参数说明**

|参数名|描述|取值范围|输入/输出|
|--|--|--|--|
|dest|指向用于存储复制内容的目标内存指针。|非空指针，指向有效内存，至少有num字节可访问。|输入/输出|
|src|指向要复制的数据源内存指针。|非空指针，指向有效内存，至少有num字节可访问。|输入|
|num|被复制的字节数。|非负数，不超过实际可分配内存大小。|输入|

**返回值**

- 成功：返回指向目标存储区内存的指针。
- 失败：遵循开源Glibc，不返回特殊异常值。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>如果源和目标内存区域有重叠，应该使用memmove，因为memcpy无法安全处理重叠内存。

**示例**

```c
#include <stdio.h>
#include <string.h>

int main() {
    char source[] = "Hello, World!";
    char destination[20];
    memcpy(destination, source, strlen(source) + 1);

    printf("Source: %s\n", source);
    printf("Destination: %s\n", destination);
    return 0;
}
```

运行结果：

```text
Source: Hello, World!
Destination: Hello, World!
```

### memmove

**函数功能**

将源内存区域的内容复制到目标内存区域。两块内存区域允许重叠。

**函数定义**

```c
void *memmove(void *dest, const void *src, size_t num);
```

**参数说明**

|参数名|描述|取值范围|输入/输出|
|--|--|--|--|
|dest|指向用于存储复制内容的目标内存指针。|非空指针，指向有效内存，至少有num字节可访问。|输入/输出|
|src|指向要复制的数据源内存指针。|非空指针，指向有效内存，至少有num字节可访问。|输入|
|num|被复制的字节数。|非负数，不超过实际可分配内存大小。|输入|

**返回值**

- 成功：返回指向目标存储区内存的指针。
- 失败：遵循开源Glibc，不返回特殊异常值。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>memmove允许源和目标内存区域有重叠，相比memcpy更安全。

**示例**

```c
#include <stdio.h>
#include <string.h>

int main() {
    char source[20] = "Hello, World!";
    memmove(source + 1, source, strlen(source) + 1);

    printf("Source: %s\n", source);
    return 0;
}
```

运行结果：

```text
Source: HHello, World!
```

### memset

**函数功能**

将指定值填充到内存区域的每个字节。通常用于初始化或清空内存区域。

**函数定义**

```C
void *memset(void *ptr, int value, size_t num);
```

**参数说明**

|参数名|描述|取值范围|输入/输出|
|--|--|--|--|
|ptr|指向用于存储重置内容的目标内存指针。|非空指针，指向有效内存，至少有num字节可访问。|输入/输出|
|value|重置的目标值。|实际会被转换为unsigned char，有效范围为0~255（超过则截断）|输入|
|num|被重置的字节数。|非负数，不超过ptr指向的内存大小。|输入|

**返回值**

- 成功：返回指向目标存储区内存的指针，返回值一般被忽略。
- 失败：遵循开源Glibc，不返回特殊异常值。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>主要用于初始化内存（如将内存设置为0或其他值）或清空数据。由于使用ZVA指令优化清零操作，因此需要注意系统ZVA块的大小。

对于memset内存清零操作，目前仅支持系统DC ZVA块大小为64B的场景，验证方法请参见[ZVA块测试](#ZVA_TEST)。

**ZVA块测试**<a id="ZVA_TEST"></a>

```C
#include <stdio.h>
#include <stdint.h>
int main() {
    uint64_t dczid;
    // 使用内联汇编读取dczid_el0
    asm volatile("mrs %0, dczid_el0" : "=r"(dczid));
    if (dczid & 0x10) {
        printf("DC ZVA not supported.\n");
    } else {
        uint32_t bs = dczid & 0xF;
        uint32_t size = 1 << (bs + 2);  // 块大小 = 2^(bs+2) 字节
        printf("DC ZVA block size: %u bytes\n", size);
    }
    return 0;
}
```

编译并执行上述内容。

```bash
gcc test_zva.c -o test_zva
./test_zva
```

运行结果如下所示。

```bash
DC ZVA block size: 64 bytes
```

**示例**

```C
#include <stdio.h>
#include <string.h>

int main() {
    char byteArr[10];
    memset(byteArr, 'A', sizeof(byteArr));
    for(int i = 0; i < sizeof(byteArr); i++) {
        printf("%c ", byteArr[i]);
    }
    printf("\n");
    return 0;
}
```

运行结果：

```text
A A A A A A A A A A 
```

### gettimeofday

**函数功能**

获取当前系统的时间，返回自1970年1月1日以来的秒数和微秒数。该函数适用于获取高精度的时间戳。

**函数定义**

```c
int gettimeofday(struct timeval *tv, struct timezone *tz);
```

**参数说明**

|参数名|描述|取值范围|输入/输出|
|--|--|--|--|
|tv|指向struct timeval的指针，返回当前时间的秒数和微秒数。|非空timeval结构体|输入/输出|
|tz|指向struct timezone的指针，通常在现代系统中不再使用，因此通常设为NULL。|NULL|输入|

**返回值**

- 成功：返回0。
- 失败：遵循开源Glibc，不返回特殊异常值。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>gettimeofday提供的时间精度为微秒级。

**示例**

```c
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

int main() {
    struct timeval tv;
    struct timezone *tz;
    int seconds;
    float micros;
    tz = NULL;
    gettimeofday(&tv, tz);
    seconds = tv.tv_sec;
    micros = tv.tv_usec;
  
    printf("当前时间（秒数）：%ld\n", seconds);
    double timestamp = seconds + micros / 1.0e6;
    printf("高精度时间戳（秒数和微秒数）：%f\n", timestamp);
    return 0;
}
```

运行结果：

```text
结果：
当前时间（秒数）：***
高精度时间戳（秒数和微秒数）：***
```

### clock\_gettime

**函数功能**

获取纳秒级精度时间，CLOCK\_REALTIME时钟源返回自1970年1月1日以来的秒数和纳秒数。该函数适用于获取高精度的时间戳。

**函数定义**

```c
int clock_gettime(clockid_t clk_id,struct timespec *tp);
```

**参数说明**

|参数名|描述|取值范围|输入/输出|
|--|--|--|--|
|clk_id|时钟源模式|目前仅优化支持CLOCK_REALTIME|输入|
|tp|指向struct timespec的指针，用于存储获取的时间值。|非空timespec结构体|输入/输出|

**返回值**

- 成功：返回0。
- 失败：若tp为空，返回KPGLIBC\_STS\_PARAMETER\_ERR错误码。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>clock\_gettime提供的时间精度为纳秒级。

**示例**

```c
#include <stdio.h>
#include <time.h>

int main() 
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts)  != 0) {
        perror("clock_gettime error");
        return 1;
    }
    printf("当前时间：%ld 秒 %ld 纳秒\n", ts.tv_sec, ts.tv_nsec);
    return 0;
}
```

运行结果：

```text
当前时间：***秒 ***纳秒
```
