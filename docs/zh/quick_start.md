# 快速入门

## 简介

kpglibc（Kunpeng GNU C Library）基于鲲鹏920/950系列处理器，利用向量化指令集对其中的字符串操作函数、内存操作函数和时间操作函数进行性能优化。已优化的函数列表参见《[API参考](./api_reference.md)》。

适用场景：字符串操作函数、内存操作函数和时间操作函数。

## 安装

编译安装请参见《[安装指南](./installation_guide.md)》。

## 使用方式

- 使用方式一：LD\_PRELOAD覆盖原Glibc函数调用实现。

  ```bash
  LD_PRELOAD=/xxx/output/libkpglibc.so <需要运行的程序>
  ```

- 使用方式二：使用gcc命令编译时使用-L和-l链接上libkpglibc.so。

  ```bash
  gcc -O3 -o test_memcmp test_memcmp.c -L/xxx/output -lkpglibc
  ```

  >![](public_sys-resources/icon-note.gif) **说明：** 
  >编译测试代码时需要启用O3编译选项。如果需要使用gettimeofday或clock\_gettime函数，请LD\_PRELOAD覆盖加载libkpglibc\_time.so。
