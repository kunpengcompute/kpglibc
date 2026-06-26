# API Reference

## Functions

[**Table 1**](#optimized-functions-in-the-kpglibc-library) lists the optimized functions in the kpglibc library.

**Table 1** Optimized functions in the kpglibc library<a id="optimized-functions-in-the-kpglibc-library"></a>

|Name|Description|
|--|--|
|strcpy|Copies the source character string (including the null character (**\0**) at the end) to the destination.|
|strcmp|Compares the contents of two character strings and returns a value (positive, negative, or 0) indicating their lexicographical order difference.|
|memchr|Searches for a character that appears for the first time in a specified memory block and returns a pointer to the character.|
|memcmp|Compares the contents of two memory blocks and returns the difference.|
|memcpy|Copies the content of a memory block to another memory block. The source and destination blocks cannot overlap.|
|memmove|Copies the content of a memory block to another memory block. The source and destination blocks can overlap.|
|memset|Sets all bytes in a memory block to a specified value.|
|gettimeofday|Obtains the current system time, in seconds and microseconds.|
|clock_gettime|Obtains the current system time, in seconds and nanoseconds.|

>![](public_sys-resources/icon-notice.gif) **NOTICE**
>
>kpglibc is available only on the Kunpeng platform. The function interfaces do not verify all input parameters, and the parameter verification logic of the original glibc functions is used. The validity of input parameters needs to be ensured by the service that calls the interfaces.

## Function Description

### strcpy

**Function Usage**

Copies the source character string (including the null character (**\\0**) at the end) to the destination. The size of the destination buffer is not checked during the copy. Therefore, if the destination buffer space is insufficient, a buffer overflow occurs.

**Function Syntax**

```c
char *strcpy(char *dest, const char *src);
```

**Parameters**

|Parameter|Description|Value Range|Input/Output|
|--|--|--|--|
|dest|Pointer to the destination memory address for receiving the source character string.|Non-null pointer. The pointed memory space must be writable and large enough to contain all source characters and **\0**.|Input/Output|
|src|Pointer to the source memory from which data is to be copied.|Non-null pointer, pointing to the address that stores the source character string. The value must end with **\0**. Otherwise, the behavior is undefined, which may cause a crash or overflow.|Input|

**Return Value**

- Success: The start address of the destination character string is returned.
- Failure: See those of open source glibc. No other exception values will be returned.

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>Ensure that the destination buffer has sufficient space to contain the source character string (including **\\0** at the end). Otherwise, a memory overflow occurs.

**Example**

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

Output:

```text
Source: Hello, World!
Destination: Hello, World!
```

### strcmp

**Function Usage**

Compares the lexicographical order of two character strings by character until a difference is found or the string terminator (**\\0**) is reached. The returned result can be a positive or negative value or 0.

**Function Syntax**

```c
int strcmp(const char *str1, const char *str2);
```

**Parameters**

|Parameter|Description|Value Range|Input/Output|
|--|--|--|--|
|str1|Pointer to the first character string.|Non-null pointer, pointing to a character string that ends with **\0**.|Input|
|str2|Pointer to the second character string.|Non-null pointer, pointing to a character string that ends with **\0**.|Input|

**Return Value**

- Success: An integer is returned.
    - If **str1** is smaller than **str2**, a negative value is returned.
    - If **str1** is equal to **str2**, **0** is returned.
    - If **str1** is greater than **str2**, a positive value is returned.

- Failure: See those of open source glibc. No other exception values will be returned.

>![](public_sys-resources/icon-notice.gif) **NOTICE**
>
>Characters are case sensitive. Therefore, the character strings "abc" and "ABC" are different.

**Example**

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

Output:

```text
apple compared to banana: -60
apple compared to apple: 0
apple compared to Banana: 124
APPLE compared to apple: -128
```

### memchr

**Function Usage**

Searches for a character that appears for the first time in a specified memory block, and returns a pointer to the character found in the memory. If the character is not found, NULL is returned.

**Function Syntax**

```c
void *memchr(const void *ptr, int value, size_t num);
```

**Parameters**

|Parameter|Description|Value Range|Input/Output|
|--|--|--|--|
|ptr|Pointer to a memory block within which the search is performed.|Non-null pointer to a valid memory block, with at least **num** bytes accessible.|Input|
|value|Character to be searched for. The input value is converted to the unsigned char type.|0 to 255. The excess part will be truncated.|Input|
|num|Number of bytes to be searched for.|Non-negative number, which cannot exceed the actual memory size.|Input|

**Return Value**

- Success: A pointer to the found character is returned.
- Failure: See those of open source glibc. No other exception values will be returned.

>![](public_sys-resources/icon-notice.gif) **NOTICE**
>
>Different from **strchr**, **memchr** scans **num** bytes (byte by byte) regardless of whether there is a null character. That is, this function searches for the position of **value** in the memory.

**Example**

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

Output:

```text
Found 'o' at index: 4
Did not find 'x'
```

### memcmp

**Function Usage**

Compares the contents of two memory blocks byte by byte. If the contents of the two memory blocks are different, the difference is returned. Otherwise, **0** is returned.

**Function Syntax**

```c
int memcmp(const void *ptr1, const void *ptr2, size_t num);
```

**Parameters**

|Parameter|Description|Value Range|Input/Output|
|--|--|--|--|
|ptr1|Pointer to the first memory block.|Non-null pointer to a valid memory block, with at least **num** bytes accessible.|Input|
|ptr2|Pointer to the second memory block.|Non-null pointer to a valid memory block, with at least **num** bytes accessible.|Input|
|num|Number of bytes to be compared.|Non-negative number, which cannot exceed the actual memory size.|Input|

**Return Value**

- Success:
    - If the contents of the two memory blocks are the same, **0** is returned.
    - If **ptr1** is smaller than **ptr2**, a negative value is returned.
    - If **ptr1** is greater than **ptr2**, a positive value is returned.

- Failure: See those of open source glibc. No other exception values will be returned.

>![](public_sys-resources/icon-notice.gif) **NOTICE**
>
>The comparison is performed byte by byte until a difference is found or all the **num** bytes are compared.

**Example**

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

Output:

```text
str1 and str2 are equal
str1 and str3 are not equal
```

### memcpy

**Function Usage**

Copies content in the source memory block to the destination memory block. The two memory blocks cannot overlap. Otherwise, undefined behavior may occur.

**Function Syntax**

```c
void *memcpy(void *dest, const void *src, size_t num);
```

**Parameters**

|Parameter|Description|Value Range|Input/Output|
|--|--|--|--|
|dest|Pointer to the destination memory used to store the copied data.|Non-null pointer to a valid memory block, with at least **num** bytes accessible.|Input/Output|
|src|Pointer to the source memory from which data is to be copied.|Non-null pointer to a valid memory block, with at least **num** bytes accessible.|Input|
|num|Number of bytes to be copied.|Non-negative number, which cannot exceed the actual available memory size.|Input|

**Return Value**

- Success: A pointer to the destination memory is returned.
- Failure: See those of open source glibc. No other exception values will be returned.

>![](public_sys-resources/icon-notice.gif) **NOTICE**
>
>If the source and destination memory blocks overlap, use **memmove** instead because **memcpy** cannot securely handle the overlapped memory.

**Example**

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

Output:

```text
Source: Hello, World!
Destination: Hello, World!
```

### memmove

**Function Usage**

Copies content in the source memory block to the destination memory block. The two memory blocks can overlap.

**Function Syntax**

```c
void *memmove(void *dest, const void *src, size_t num);
```

**Parameters**

|Parameter|Description|Value Range|Input/Output|
|--|--|--|--|
|dest|Pointer to the destination memory used to store the copied data.|Non-null pointer to a valid memory block, with at least **num** bytes accessible.|Input/Output|
|src|Pointer to the source memory from which data is to be copied.|Non-null pointer to a valid memory block, with at least **num** bytes accessible.|Input|
|num|Number of bytes to be copied.|Non-negative number, which cannot exceed the actual available memory size.|Input|

**Return Value**

- Success: A pointer to the destination memory is returned.
- Failure: See those of open source glibc. No other exception values will be returned.

>![](public_sys-resources/icon-notice.gif) **NOTICE**
>
>Unlike **memcpy**, **memmove** allows the source and destination memory blocks to overlap, making it safer to use.

**Example**

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

Output:

```text
Source: HHello, World!
```

### memset

**Function Usage**

Populates a value into each byte of a memory block. It is typically used to initialize or clear a memory block.

**Function Syntax**

```C
void *memset(void *ptr, int value, size_t num);
```

**Parameters**

|Parameter|Description|Value Range|Input/Output|
|--|--|--|--|
|ptr|Pointer to the destination memory used to store the reset data.|Non-null pointer to a valid memory block, with at least **num** bytes accessible.|Input/Output|
|value|Target value.|The value will be converted into the unsigned char type. The valid range is 0 to 255. The excess part will be truncated.|Input|
|num|Number of bytes to be reset.|Non-negative number, which cannot exceed the size of the memory block pointed to by **ptr**.|Input|

**Return Value**

- Success: A pointer to the destination memory is returned. The return value is generally ignored.
- Failure: See those of open source glibc. No other exception values will be returned.

>![](public_sys-resources/icon-notice.gif) **NOTICE**
>
>This function is used to initialize a memory block (by populating it with 0 or another value) or clear the block. As the ZVA instruction is used to optimize this operation, you should be aware of the system's ZVA block size.

The **memset** memory clearing operation supports only a DC ZVA block size of 64 bytes. To verify the block size, see [ZVA Block Test](#ZVA_TEST).

**ZVA Block Test**<a id="ZVA_TEST"></a>

```C
#include <stdio.h>
#include <stdint.h>
int main() {
    uint64_t dczid;
    // Use inline assembly to read dczid_el0.
    asm volatile("mrs %0, dczid_el0" : "=r"(dczid));
    if (dczid & 0x10) {
        printf("DC ZVA not supported.\n");
    } else {
        uint32_t bs = dczid & 0xF;
        uint32_t size = 1 << (bs + 2);  // Block size = 2^(bs+2) bytes
        printf("DC ZVA block size: %u bytes\n", size);
    }
    return 0;
}
```

Compile and execute the preceding content.

```bash
gcc test_zva.c -o test_zva
./test_zva
```

The command output is as follows:

```bash
DC ZVA block size: 64 bytes
```

**Example**

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

Output:

```text
A A A A A A A A A A 
```

### gettimeofday

**Function Usage**

Obtains the current system time and returns the number of seconds and microseconds since January 1, 1970. This function is used to obtain high-precision timestamps.

**Function Syntax**

```c
int gettimeofday(struct timeval *tv, struct timezone *tz);
```

**Parameters**

|Parameter|Description|Value Range|Input/Output|
|--|--|--|--|
|tv|Pointer to **struct timeval**. The number of seconds and microseconds of the current time is returned.|Non-null **timeval** structure|Input/Output|
|tz|Pointer to **struct timezone**. Generally, it is not used in modern systems and is usually set to NULL.|NULL|Input|

**Return Value**

- Success: **0** is returned.
- Failure: See those of open source glibc. No other exception values will be returned.

>![](public_sys-resources/icon-notice.gif) **NOTICE**
>
>The precision of the time obtained using **gettimeofday** is microseconds.

**Example**

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
  
    printf("current_time (seconds): %ld\n", seconds);
    double timestamp = seconds + micros / 1.0e6;
    printf("high-precision_timestamp (seconds and microseconds): %f\n", timestamp);
    return 0;
}
```

Output:

```text
Result:
Current time (seconds): ***
High-precision timestamp (seconds and microseconds): ***
```

### clock\_gettime

**Function Usage**

Obtains the number of seconds and nanoseconds returned by CLOCK\_REALTIME since January 1, 1970. This function is used to obtain high-precision timestamps.

**Function Syntax**

```c
int clock_gettime(clockid_t clk_id,struct timespec *tp);
```

**Parameters**

|Parameter|Description|Value Range|Input/Output|
|--|--|--|--|
|clk_id|Clock source.|**CLOCK_REALTIME** only|Input|
|tp|Pointer to **struct timespec**, which stores the obtained time.|Non-null **timespec** structure|Input/Output|

**Return Value**

- Success: **0** is returned.
- Failure: If **tp** is null, the **KPGLIBC\_STS\_PARAMETER\_ERR** error code is returned.

>![](public_sys-resources/icon-notice.gif) **NOTICE**
>
>The precision of the time obtained using **clock\_gettime** is nanoseconds.

**Example**

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
    printf("current_time: %ld seconds %ld nanoseconds\n", ts.tv_sec, ts.tv_nsec);
    return 0;
}
```

Output:

```text
Current time: *** seconds *** nanoseconds
```
