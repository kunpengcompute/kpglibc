# kpglibc

## Latest Updates

- 2026.03.30: Released the version v1.0.3, in which the memcpy, memmove, memcmp, and memset functions optimized for the Kunpeng 950 processor are added, providing acceleration in most scenarios.

## Project Introduction

kpglibc is a repository contributed by Huawei Kunpeng to the glibc open-source community. It leverages a vectorized instruction set to enhance the performance of string, memory, and time operation functions for the new Kunpeng 920 processor model and Kunpeng 950 processor. The version optimized with SVE is recommended for better performance. The function interfaces remain consistent with the open-source glibc, and internal input parameter validation is not performed. The validity of input parameters must be ensured by the service that calls the interfaces.

## Directory Structure

```text
kpglibc/
├── cmake/                    # CMake configuration file
├── docs/                     # Project documents
│   ├── en/                   # English documentation
│   │   ├── api_reference.md        # API Reference
│   │   ├── installation_guide.md   # Installation Guide
│   │   ├── quick_start.md          # Quick Start
│   │   ├── release_notes.md        # Release Nodes
│   └── LICENSE
├── include/                  # Header file
├── src/                      # Source code
│   ├── basic/                # Basic code
│   │   ├── include/
│   │   └── src/
│   ├── core_neon/            # NEON optimization implementation
│   │   ├── src/
│   │   └── CMakeLists.txt
│   ├── core_sve/             # SVE optimization implementation
│   │   ├── src/
│   │   └── CMakeLists.txt
│   ├── time/                 # Time function optimization implementation
│   │   ├── src/
│   │   └── CMakeLists.txt
│   └── CMakeLists.txt
├── test/                     # Test code
│   ├── build/
│   ├── common/               # Test case code
│   ├── gtest-download/
│   ├── test_tool_bins/
│   └── kp_coverage.py
├── .gitignore
├── CMakeLists.txt
├── COPYING
├── LICENSES
├── README.md
├── build.sh                  # Build script
├── config.cmake.in
├── function.sh
├── kpglibcver.h
└── kpglibcver.h.in
```

## Release Notes

| kpglibc | Open-source glibc|  Feature|
| ------------ | ------------ | ------------ |
|  v1.0.3 |  Aligned with glibc 2.31, but not limited to glibc 2.31. It can also be enabled for other glibc versions.|The memcpy, memmove, memcmp, and memset functions optimized for the Kunpeng 950 processor are added, providing acceleration in most scenarios.|
|  v1.0.2 |  Aligned with glibc 2.34, but not limited to glibc 2.34. It can also be enabled for other glibc versions.|The memcpy and memset functions optimized with SVE are added, providing acceleration for small-batch memcpy and memset operations.|
|  v1.0.1 |  Aligned with glibc 2.34, but not limited to glibc 2.34. It can also be enabled for other glibc versions.|Optimized the performance of the string, memory, and time operation functions for Kunpeng processors.|

## Environment Deployment

Verified hardware configuration: openEuler OSs (for example, openEuler 22.03 LTS) running on the Kunpeng architecture.

- Compilation

```bash
sh build.sh 
```

The compilation files are stored in the **output** folder.

## Quick Start

1. Enable PRELOAD.

```bash
LD_PRELOAD=xxx/output/libkpglibc_sve.so <program to be run>
```

2. Enable compilation and linking.

```bash
gcc -o test_memcmp test_memcmp.c -Lxxx/output/ -lkpglibc_sve 
```

For details about the list of optimized functions, see [API Reference](docs/en/api_reference.md).

## Post-Installation Verification

Run the **ll output** command to view the .so file. If the following information is displayed, the compilation is successful:

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

## Documents

| Name| Description|
| --- | --- |
| [Quick Start](docs/en/quick_start.md)| Provides guidance on how to quickly enable and verify the kpglibc feature.|
| [Release Notes](docs/en/release_notes.md)| Provides basic information and feature updates of each kpglibc version.|
| [Installation Guide](docs/en/installation_guide.md)| Describes how to compile and install kpglibc.|
| [API Reference](docs/en/api_reference.md)| Provides API descriptions and API calling examples.|

## Disclaimer

This code repository contributes to the glibc open-source project solely for performance optimization of certain glibc functions on Kunpeng processors. It strictly adheres to the coding style and methods, as well as security design of the native open-source software. Any vulnerability and security issues of the software shall be resolved by the corresponding upstream communities according to their response mechanisms. Please pay attention to the notifications and version updates released by the upstream communities. The Kunpeng computing community does not assume any responsibility for software vulnerabilities and security issues.

## License

kpglibc is licensed under LGPL-2.1 and Apache-2.0. For details, see [LICENSES](https://gitcode.com/boostkit/kpglibc/blob/master/LICENSES).

The documents of this project are licensed under CC-BY 4.0. For details, see [LICENSE](docs/LICENSE).

## Contributions

If you have any questions or want to provide feedback on feature requirements and bug reports, you can submit issues. For details, see the [contribution guideline](https://gitcode.com/boostkit/community/blob/master/docs/contributor/contributing.md).

## Suggestions and Feedback

You are welcome to contribute to the community. If you have any questions or suggestions, submit [issues](https://gitcode.com/boostkit/community/blob/master/docs/contributor/issue-submit.md). We will reply to you as soon as possible. Thank you for your support.

## Acknowledgments

kpglibc is jointly developed by the following Huawei department:

 - Kunpeng Computing BoostKit Development Dept

Thank you to everyone in the community for your PRs. We warmly welcome contributions to kpglibc!
