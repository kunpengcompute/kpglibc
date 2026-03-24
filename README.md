# kpglibc介绍

## 最新消息

- 2026-3-30 发布v1.0.3版本，新增针对950优化的memcpy/memmove/memcmp/memset函数，在大部分场景有加速效果。

## 项目简介

kpglibc是鲲鹏参与glibc开源社区的仓库，基于华为鲲鹏处理器利用向量化指令集对glibc中的字符串操作函数、内存操作函数和时间操作函数进行性能优化，适用于鲲鹏920新型号处理器、950处理器，推荐使用SVE优化的版本获得更好的性能。函数接口与开源glibc保持一致，接口内部不做完整入参校验，入参合法性及合理性由调用方业务来保证。

## 目录结构

```text
kpglibc/
├── cmake/                    # CMake配置文件
├── docs/                     # 项目文档
│   ├── public_sys-resources/ # 文档资源文件
│   ├── zh/                   # 中文文档
│   └── LICENSE
├── include/                  # 头文件
├── src/                      # 源代码
│   ├── basic/                # 基础代码
│   │   ├── include/
│   │   └── src/
│   ├── core_neon/            # NEON优化实现
│   │   ├── src/
│   │   └── CMakeLists.txt
│   ├── core_sve/             # SVE优化实现
│   │   ├── src/
│   │   └── CMakeLists.txt
│   ├── time/                 # 时间函数优化实现
│   │   ├── src/
│   │   └── CMakeLists.txt
│   └── CMakeLists.txt
├── test/                     # 测试代码
│   ├── build/
│   ├── common/               # 测试用例代码 
│   ├── gtest-download/
│   ├── test_tool_bins/
│   └── kp_coverage.py
├── .gitignore
├── CMakeLists.txt
├── COPYING
├── LICENSES
├── README.md
├── build.sh                  # 编译构建脚本
├── config.cmake.in
├── function.sh
├── kpglibcver.h
└── kpglibcver.h.in
```

## 版本说明

| kpglibc | 开源glibc |  特性 |
| ------------ | ------------ | ------------ |
|  v1.0.3 |  对标glibc 2.31，不局限于2.31版本，系统glibc为其他版本也可以使能 |新增针对950优化的memcpy/memmove/memcmp/memset函数，在大部分场景有加速效果|
|  v1.0.2 |  对标glibc 2.34，不局限于2.34版本，系统glibc为其他版本也可以使能 |新增sve优化的memcpy、memset函数，在小批量的memcpy，memset有加速效果|
|  v1.0.1 |  对标glibc 2.34，不局限于2.34版本，系统glibc为其他版本也可以使能 |针对鲲鹏处理器增加字符串操作函数、内存操作函数和时间操作函数性能优化|

## 环境部署

已验证的硬件配置：鲲鹏架构下的openEuler系（例如openEuler 22.03 LTS）操作系统。

- 编译说明

```bash
sh build.sh 
```

编译产物在output文件夹下。

## 快速上手

1、PRELOAD覆盖加载使能

```bash
LD_PRELOAD=xxx/output/libkpglibc_sve.so <需要运行的程序> 
```

2、编译链接使能

```bash
gcc -o test_memcmp test_memcmp.c -Lxxx/output/ -lkpglibc_sve 
```

已优化的函数列表请参见[API参考文档](docs/zh/api.md)

## 安装后验证

执行ll output命令查看so，回显如下结果说明编译成功

```text
lrwxrwxrwx 1 root root    24 Mar  5 11:27 libkpglibc_neon.so -> libkpglibc_neon.so.1.0.3
-rwxr-xr-x 1 root root 67376 Mar  5 11:27 libkpglibc_neon.so.1.0.3
lrwxrwxrwx 1 root root    27 Mar  5 11:27 libkpglibc_sve_950.so -> libkpglibc_sve_950.so.1.0.3
-rwxr-xr-x 1 root root 67376 Mar  5 11:27 libkpglibc_sve_950.so.1.0.3
lrwxrwxrwx 1 root root    23 Mar  5 11:27 libkpglibc_sve.so -> libkpglibc_sve.so.1.0.3
-rwxr-xr-x 1 root root 67376 Mar  5 11:27 libkpglibc_sve.so.1.0.3
lrwxrwxrwx 1 root root    24 Mar  5 11:27 libkpglibc_time.so -> libkpglibc_time.so.1.0.3
-rwxr-xr-x 1 root root 67376 Mar  5 11:27 libkpglibc_time.so.1.0.3
```

## 学习文档

| 资源名称 | 资源简介 |
| --- | --- |
| [快速入门](docs/zh/quick_start.md) | 提供kpglibc使能并验证加速能力的快速入门指导。 |
| [版本说明书](docs/zh/release_notes.md) | 提供kpglibc每个发布版本的基础信息和特性更新信息。 |
| [安装指南](docs/zh/installation_guide.md) | 提供kpglibc编译安装的详细指导。 |
| [API参考](docs/zh/api.md) | 提供接口说明、接口调用示例等。 |

## 免责声明

此代码仓计划参与glibc软件开源，仅对glibc部分函数在鲲鹏处理器上进行性能优化，编码风格遵照原生开源软件，继承原生开源软件安全设计，不破坏原生开源软件设计及编码风格和方式，软件的任何漏洞与安全问题，均由相应的上游社区根据其漏洞和安全响应机制解决。请密切关注上游社区发布的通知和版本更新。鲲鹏计算社区对软件的漏洞及安全问题不承担任何责任。

## License

kpglibc遵循 LGPL-2.1许可证与Apache-2.0许可证，具体请参见[LICENSE文件](LICENSES)。

本项目的文档适用CC-BY 4.0许可证，具体请参见[LICENSE](docs/LICENSE)文件。

## 贡献指南

如果使用过程中有任何问题，或者需要反馈特性需求和bug报告，可以提交issues联系我们，具体贡献方法可参考[这里](https://gitcode.com/boostkit/community/blob/master/docs/contributor/contributing.md)。

## 建议与交流

欢迎大家为社区做贡献。如果有任何疑问或建议，请提交[Issues](https://gitcode.com/boostkit/community/blob/master/docs/contributor/issue-submit.md)，我们会尽快回复。感谢您的支持。

## 致谢

kpglibc由华为公司的下列部门联合贡献：

 - 鲲鹏计算Boostkit开发部

感谢来自社区的每一个PR，欢迎贡献kpglibc！
