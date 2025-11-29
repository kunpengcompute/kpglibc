# kpglibc基于开源glibc2.31的编译指南
本文介绍如何编译基于 glibc-2.31 的 kpglibc，该版本针对鲲鹏950处理器进行了优化。

## 重要版本说明
当前补丁基于 glibc 的 glibc-2.31 tag 版本开发，请注意：
- 官方 GitHub 仓库还有一个持续更新的 release/2.31/master分支
- 两个版本的代码存在差异，必须使用 git checkout glibc-2.31切换到正确的 tag 版本
- 推荐使用 Debian 11 或相同 glibc 版本的环境，在更高版本的 glibc 环境下编译可能会遇到兼容性问题

## 环境准备
安装以下编译所需的依赖组件：
~~~shell
apt update
apt install bison gawk build-essential gettext python3
~~~

## 开源glibc代码下载
下载开源 glibc 代码并切换到正确的 2.31 tag：
~~~shell
git clone https://github.com/bminor/glibc.git
cd glibc
git checkout glibc-2.31
~~~

设置环境变量（请根据实际路径修改）：
~~~shell
export GLIBC_SRC=/path/to/your/glibc
~~~

## 应用kpglibc补丁
在 glibc 源码目录中执行以下命令：
~~~shell
cd $GLIBC_SRC
wget https://raw.gitcode.com/boostkit/kpglibc/raw/dev_for_BD_glibc2.31/memcpy_kunpeng_glibc_2_31.patch
git apply patch1_memcpy_kunpeng950_glibc_2_31.patch
git apply patch2_memset_kunpeng950_glibc_2_31.patch
git apply patch3_memcmp_kunpeng950_glibc_2_31.patch
~~~

## 源码编译
执行以下命令编译带有 _memcpy_kunpeng950优化的 glibc 代码：
~~~shell
cd $GLIBC_SRC
mkdir build && cd build
../configure --prefix=/usr
make -j$(nproc)
~~~

## 安全安装说明

**重要提示**：如果要执行 make install，需指定安装路径，直接安装到系统默认路径可能导致系统崩溃。
~~~shell
cd $GLIBC_SRC/build
mkdir -p ../install
make install DESTDIR=$GLIBC_SRC/install/
~~~

# 测试代码编译

编译memcpy/memset/memcmp测试程序（使用调试符号以便perf分析）：
~~~shell
gcc -o test_mem test_mem.c -g -O2
~~~

## perf验证分支

设置环境变量指向编译安装的glibc so库（请根据实际路径修改）：
~~~shell
export GLIBC_INSTALL=$GLIBC_SRC/install
~~~

使用perf工具验证memcpy分支调用情况，验证在鲲鹏950服务器上是否进入相应的_kunpeng950分支：

~~~shell
perf record -g -F 99 $GLIBC_INSTALL/lib64/ld-linux-aarch64.so.1 --library-path $GLIBC_INSTALL/lib64 ./test_mem

# 生成报告
perf report -g --stdio
~~~