# Installation Guide

## Verified Environments

To use kpglibc smoothly and securely, ensure that your environment is one of the verified environments.

|OS|CPU|Compiler|
|--|--|--|
|openEuler 22.03 LTS SP3|Kunpeng 920 series|GCC 10.3.1|
|openEuler 24.03 LTS SP2|Kunpeng 950|GCC 12.3.1|
|Debian 12|Kunpeng 950|GCC 12.2.0|

Download a required openEuler version from [Download Center](https://www.openeuler.openatom.cn/en/download/?archive=true).

>![](public_sys-resources/icon-notice.gif) **NOTICE:**
>
>- kpglibc supports only Kunpeng series processors and will exit abnormally for any other processors. If a program dynamically loads kpglibc, the program exits when kpglibc is linked and an undefined result will be generated.
>- The time function in kpglibc is available only in environments where the VVAR page size is 8 KB or 64 KB. In other environments, do not install **libkpglibc\_time.so**. You can run the **cat /proc/self/maps | grep vvar** command to query the VVAR page size.
>- kpglibc is a primitive library whose computing process involves memory read and write operations. kpglibc does not provide or release an OS, or take responsibility for the OS security. You need to install an OS by yourself and perform security hardening, such as installing only necessary applications or uninstalling unnecessary applications that have been installed.

## Installation

This section describes how to install kpglibc by compiling the source code and verify kpglibc after the installation.

**Compilation and Installation**

1. Obtain the kpglibc code.

    ```bash
    git clone https://gitcode.com/boostkit/kpglibc.git
    ```

2. Compile kpglibc.

    ```bash
    cd kpglibc
    sh build.sh
    ``` 

**Verification After Compilation**

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

## Uninstallation

If kpglibc is no longer needed, run the **rm** command to delete it.

```bash
rm -rf ./kpglibc
```
