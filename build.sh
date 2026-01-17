#! /bin/bash
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# @Filename: build.sh
# @Usage: sh build.sh or sh build.sh clean or sh build.sh install or sh build.sh uninstall

set -e

LIB_ROOT_PATH=$(cd "$(dirname "${0}")"; pwd -P)
source ${LIB_ROOT_PATH}/function.sh
OBJ_DIR=${LIB_ROOT_PATH}/build
OUTPUT_DIR=${LIB_ROOT_PATH}/output
INSTALL_LIB_PATH=/usr/local/lib/KPGLIBC/
INSTALL_INCLUDE_PATH=/usr/local/include/KPGLIBC/
DEBPACK_ROOT_PATH=${LIB_ROOT_PATH}/package/debpack
DEBPACK_INCLUDE_PATH=${DEBPACK_ROOT_PATH}/KPGLIBC/include
DEBPACK_LIB_PATH=${DEBPACK_ROOT_PATH}/KPGLIBC/lib

KPGLIBC_INFO_VERSION=$(xmllint --xpath "//KPGLIBC_INFO_VERSION/text()" ${LIB_ROOT_PATH}/dependence.xml)
KPGLIBC_INFO_COMPONENT_NAME=$(xmllint --xpath "//KPGLIBC_INFO_COMPONENT_NAME/text()" ${LIB_ROOT_PATH}/dependence.xml)
KPGLIBC_INFO_SOFTWARE_NAME=$(xmllint --xpath "//KPGLIBC_INFO_SOFTWARE_NAME/text()" ${LIB_ROOT_PATH}/dependence.xml)
KPGLIBC_INFO_PRODUCT_VERSION=$(xmllint --xpath "//KPGLIBC_INFO_PRODUCT_VERSION/text()" ${LIB_ROOT_PATH}/dependence.xml)
cpulimit=$(xmllint --xpath "//KPGLIBC_CPU_CHECK_ENABLE/text()" ${LIB_ROOT_PATH}/dependence.xml)
KPGLIBC_C_TOOLCHAIN=$(xmllint --xpath "//KPGLIBC_C_TOOLCHAIN/text()" ${LIB_ROOT_PATH}/dependence.xml)
export CC=${KPGLIBC_C_TOOLCHAIN}
export KPGLIBC_CPU_CHECK_ENABLE=${cpulimit}

function kpglibc_clean()
{
    if [ -d "${OBJ_DIR}" ]; then
        rm -rf ${OBJ_DIR}
    fi
    if [ -d "${OUTPUT_DIR}" ]; then
        rm -rf ${OUTPUT_DIR}
    fi
}

function kpglibc_install()
{
    if [ ! -d "${INSTALL_LIB_PATH}" ]; then
        mkdir -p ${INSTALL_LIB_PATH}
    fi
    if [ ! -d "${INSTALL_INCLUDE_PATH}" ]; then
        mkdir -p ${INSTALL_INCLUDE_PATH}
    fi
    cp -rf ${LIB_ROOT_PATH}/include/* ${INSTALL_INCLUDE_PATH}
    cp -rf ${OUTPUT_DIR}/* ${INSTALL_LIB_PATH}
   
    # 检查是否以超级用户权限运行
    if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root"
    exit 1
    fi

    MARK="# Added by KPGLIBC install script"
    if ! grep -q "$MARK" /etc/profile; then
    {
        echo "$MARK"
        echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:${INSTALL_LIB_PATH}"
        echo "export C_INCLUDE_PATH=\$C_INCLUDE_PATH:${INSTALL_INCLUDE_PATH}"
        echo "export CPLUS_INCLUDE_PATH=\$CPLUS_INCLUDE_PATH:${INSTALL_INCLUDE_PATH}"
    } >> /etc/profile
        echo "Added environment variables to /etc/profile."
    else
        echo "The marker already exists in /etc/profile."
    fi
    # 使修改立即生效
    source /etc/profile
}

function kpglibc_uninstall()
{
    kpglibc_clean
    rm -rf ${INSTALL_INCLUDE_PATH}/*.h ${INSTALL_LIB_PATH}/*
    # 删除标记和相关环境变量
    LINES_TO_DELETE=3
    # 循环删除所有标记及其后续行
    while grep -q "# Added by KPGLIBC install script" /etc/profile; do
        sed -i "/# Added by KPGLIBC install script/{N;N;N;d}" /etc/profile
    done
}

if [ "$1" == "" ] ;then
    COVERAGE_OPTION=""
    kpglibc_build
elif [ "$1" == "-coverage" ]; then
    COVERAGE_OPTION="-DENABLE_TEST_COVERAGE=ON"
    kpglibc_build
elif [ "$1" == "clean" ] ;then
    kpglibc_clean
elif [ "$1" == "install" ] ;then
    kpglibc_install
elif [ "$1" == "uninstall" ] ;then
    kpglibc_uninstall
elif [ "$1" == "deb" ] ;then
    kpglibc_clean
    kpglibc_build
    deb_pack
elif [ "$1" == "util" ] ;then
    kpglibc_util_conf
fi