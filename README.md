# 项目介绍
kpglibc是鲲鹏参与glibc开源社区的仓库，基于华为鲲鹏处理器利用向量化指令集对glibc中的字符串操作函数、内存操作函数和时间操作函数进行性能优化，适用于鲲鹏920新型号处理器，推荐使用SVE优化的版本获得更好的性能。函数接口与开源glibc保持一致，接口内部不做完整入参校验，入参合法性及合理性由调用方业务来保证。
# 2 版本说明
| kpglibc | 开源glibc |  特性 |
| ------------ | ------------ | ------------ |
|  v1.0.2 |  对标glibc 2.34，不局限于2.34版本，系统glibc为其他版本也可以使能 |新增sve优化的memcpy、memset函数，在小批量的memcpy，memset有加速效果|
|  v1.0.1 |  对标glibc 2.34，不局限于2.34版本，系统glibc为其他版本也可以使能 |针对鲲鹏处理器增加字符串操作函数、内存操作函数和时间操作函数性能优化|


# 3 环境部署
已验证的硬件配置：鲲鹏架构下的openEuler系（例如openEuler 22.03 LTS）操作系统。
- 编译说明

```
#只编译源码 
sh build.sh 
#编译源码并开启覆盖率统计编译选项 
sh build.sh -coverage
```
编译产物在output文件夹下。

# 4 快速上手
PRELOAD覆盖加载方式
```
LD_PRELOAD=xxx/output/libkpglibc_sve.so <需要运行的程序> 
```
2. 编译链接方式
```
gcc -o test_memcmp test_memcmp.c -Lxxx/output/ -lkpglibc_sve 
```
详细开发指南请参见hikunpeng KSL文档kpglibc小节：https://www.hikunpeng.com/document/detail/zh/kunpengaccel/system-lib/dg-avx2ki/kunpengaccel_ksl_16_0001.html

# 5 安装后验证
执行ll output命令查看so，回显如下结果说明编译成功
```
lrwxrwxrwx 1 root root    24 Sep 17 09:16 libkpglibc_neon.so -> libkpglibc_neon.so.1.0.2
-rwxr-xr-x 1 root root 67640 Sep 17 09:16 libkpglibc_neon.so.1.0.2
lrwxrwxrwx 1 root root    23 Sep 17 09:16 libkpglibc_sve.so -> libkpglibc_sve.so.1.0.2
-rwxr-xr-x 1 root root 67640 Sep 17 09:16 libkpglibc_sve.so.1.0.2
lrwxrwxrwx 1 root root    24 Sep 17 09:16 libkpglibc_time.so -> libkpglibc_time.so.1.0.2
-rwxr-xr-x 1 root root 67640 Sep 17 09:16 libkpglibc_time.so.1.0.2
```
# 6 贡献指南
如果使用过程中有任何问题，或者需要反馈特性需求和bug报告，可以提交isssues联系我们，具体贡献方法可参考[这里](https://gitcode.com/boostkit/community/blob/master/docs/contributor/contributing.md)。

# 7 免责声明
此代码仓计划参与glibc软件开源，仅对glibc部分函数在鲲鹏处理器上进行性能优化，编码风格遵照原生开源软件，继承原生开源软件安全设计，不破坏原生开源软件设计及编码风格和方式，软件的任何漏洞与安全问题，均由相应的上游社区根据其漏洞和安全响应机制解决。请密切关注上游社区发布的通知和版本更新。鲲鹏计算社区对软件的漏洞及安全问题不承担任何责任。