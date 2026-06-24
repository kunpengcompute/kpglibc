# Quick Start

## Overview

Kunpeng GNU C Library (kpglibc) leverages a vectorized instruction set to enhance the performance of string, memory, and time functions for Kunpeng 920/950 series processors. For details about the list of optimized functions, see [API Reference](./api_reference.md).

Application scenarios: string, memory, and time operations

## Installation

For details about the compilation and installation, see [Installation Guide](./installation_guide.md).

## Usage

- Method 1: Use **LD\_PRELOAD** to overwrite the original glibc function implementation.

    ```bash
    LD_PRELOAD=/xxx/output/libkpglibc.so <program to be run>
    ```

- Method 2: Add the **-L** and **-l** options to the **gcc** command to link **libkpglibc.so**.

    ```bash
    gcc -O3 -o test_memcmp test_memcmp.c -L/xxx/output -lkpglibc
    ```

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >The **-O3** compilation option needs to be enabled during test code compilation. If the **gettimeofday** or **clock\_gettime** function is required, use **LD\_PRELOAD** to load **libkpglibc\_time.so**.
