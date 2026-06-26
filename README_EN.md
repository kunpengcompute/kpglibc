# kpglibc

## Latest Updates

- [2026-06-30] Released the version v1.0.4, in which the memcmp function is further optimized for the Kunpeng 950 processor.
- [2026-03-30] Released the version v1.0.3, in which the memcpy, memmove, memcmp, and memset functions optimized for the Kunpeng 950 processor are added, providing acceleration in most scenarios.

## Project Introduction

kpglibc is a repository contributed by Huawei Kunpeng to the glibc open-source community. It leverages a vectorized instruction set to enhance the performance of string, memory, and time operation functions for the new Kunpeng 920 processor model and Kunpeng 950 processor. The version optimized with SVE is recommended for better performance. The function interfaces remain consistent with the open-source glibc, and internal input parameter validation is not performed. The validity of input parameters must be ensured by the service that calls the interfaces.

## Directory Structure

```text
kpglibc/
├── cmake/                          # CMake configuration file
├── docs/                           # Project documents
│   ├── en/                         # English documentation
│   │   ├── api_reference.md        # API reference
│   │   ├── installation_guide.md   # Installation guide
│   │   ├── quick_start.md          # Quick start
│   │   ├── release_notes.md        # Release notes
│   └── LICENSE                     # Document license
├── include/                        # Header file of external interfaces
├── src/                            # Source code
│   ├── basic/                      # Public basic code
│   ├── core_neon/                  # NEON optimization implementation
│   ├── core_sve/                   # SVE optimization implementation
│   ├── time/                       # Time function optimization implementation
│   └── CMakeLists.txt              # CMake file
├── test/                           # Test code
│   ├── build/                      # Intermediate build artifacts
│   ├── common/                     # Test case code 
│   ├── gtest-download/             # Script for fetching gtest
│   ├── test_tool_bins/             # Test tool binaries
│   └── kp_coverage.py              # Coverage statistics script
├── CMakeLists.txt                  # Project CMake file
├── LICENSES                        # Project license
├── README_EN.md                    # Project introduction
├── build.sh                        # Build script
├── config.cmake.in                 # CMake input file
├── function.sh                     # Build support script
└── kpglibcver.h                    # kpglibc version control file
```

## Release Notes

| kpglibc | Open-source glibc | Feature |
| ------------ | ------------ | ------------ |
| v1.0.4 | Aligned with glibc 2.31, but not limited to glibc 2.31. It can also be enabled for other glibc versions. | The memcmp function is further optimized for the Kunpeng 950 processor. |
| v1.0.3 | Aligned with glibc 2.31, but not limited to glibc 2.31. It can also be enabled for other glibc versions. | The memcpy, memmove, memcmp, and memset functions optimized for the Kunpeng 950 processor are added, providing acceleration in most scenarios. |
| v1.0.2 | Aligned with glibc 2.34, but not limited to glibc 2.34. It can also be enabled for other glibc versions. | The memcpy and memset functions optimized with SVE are added, providing acceleration for small-batch memcpy and memset operations. |
| v1.0.1 | Aligned with glibc 2.34, but not limited to glibc 2.34. It can also be enabled for other glibc versions. | Optimized the performance of the string, memory, and time operation functions for Kunpeng processors. |

## Environment Deployment

To install and enable kpglibc, refer to [Installation Guide](docs/en/installation_guide.md).

## Quick Start

After installing kpglibc, refer to [Quick Start](docs/en/quick_start.md) to verify kpglibc acceleration.

## Documents

| Name | Description |
| --- | --- |
| [Release Notes](docs/en/release_notes.md) | Provides basic information and feature updates of each kpglibc version. |
| [Installation Guide](docs/en/installation_guide.md) | Describes how to compile and install kpglibc. |
| [Quick Start](docs/en/quick_start.md) | Provides guidance on how to quickly enable and verify the kpglibc feature. |
| [API Reference](docs/en/api_reference.md)| Provides API descriptions and API calling examples. |

## Disclaimer

This code repository contributes to the glibc open-source project solely for performance optimization of certain glibc functions on Kunpeng processors. It strictly adheres to the coding style and methods, as well as security design of the native open-source software. Any vulnerability and security issues of the software shall be resolved by the corresponding upstream communities according to their response mechanisms. Please pay attention to the notifications and version updates released by the upstream communities. The Kunpeng computing community does not assume any responsibility for software vulnerabilities and security issues.

## License

kpglibc is licensed under LGPL-2.1 and Apache-2.0. For details, see [LICENSES](https://gitcode.com/boostkit/kpglibc/blob/master/LICENSES).

The documents of this project are licensed under CC-BY 4.0. For details, see [LICENSE](docs/LICENSE).

## Contributions

If you have any questions or want to provide feedback on feature requirements and bug reports, you can submit issues. For details, see the contribution guideline.

## Suggestions and Feedback

You are welcome to contribute to the community. If you have any questions or suggestions, submit issues. We will reply to you as soon as possible. Thank you for your support.

## Acknowledgments

kpglibc is jointly developed by the following Huawei department:

 - Kunpeng Computing BoostKit Development Dept

Thank you to everyone in the community for your PRs. We warmly welcome contributions to kpglibc!
