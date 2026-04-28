# 安装指南

## 已验证环境

为保证您可以顺利安全地使用kpglibc，请确保所使用的环境信息在已验证环境范围内。

|操作系统|CPU类型|编译器|
|--|--|--|
|openEuler 22.03 LTS SP3|鲲鹏920系列处理器|GCC 10.3.1|
|openEuler 24.03 LTS SP2|鲲鹏950处理器|GCC 12.3.1|
|Debian 12|鲲鹏950处理器|GCC 12.2.0|

openEuler操作系统[获取链接](https://www.openeuler.openatom.cn/zh/download/?archive=true)。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>
>- kpglibc目前只支持鲲鹏系列处理器，其他处理器使用kpglibc时会异常退出。如果以动态的方式加载kpglibc库，程序会在链接kpglibc时退出，造成未定义的结果。
>- kpglibc的time函数仅支持vvar数据页大小为8K/64K的环境，其他环境不可安装libkpglibc\_time.so。vvar数据页大小可通过**cat /proc/self/maps | grep vvar**命令查询。
>- kpglibc为底层原语库，计算流程涉及内存读写等操作。kpglibc不提供和发布操作系统，操作系统须用户自行安装，kpglibc不承担操作系统的安全责任，用户需要结合自身应用对操作系统安全加固，包括不安装或者剔除不必要的应用等。

## 安装

本文提供源码编译安装kpglibc的方法，请在安装后进行验证。

**编译安装步骤**

1. 获取kpglibc代码。

    ```bash
    git clone https://gitcode.com/boostkit/kpglibc.git
    ```

2. 编译kpglibc。

    ```bash
    cd kpglibc
    sh build.sh
    ```
    
    编译产物在`output`文件夹下。

**编译后验证**

执行**ll output**命令查看so，回显如下结果说明编译成功。

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

## 卸载

若不再需要使用kpglibc，可使用rm命令删除kpglibc即可。

```bash
rm -rf ./kpglibc
```
