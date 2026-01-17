#!/bin/bash
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

set -e

function kpglibc_util_conf()
{
    mkdir -p ${OBJ_DIR}
    mkdir -p ${OUTPUT_DIR}
}

function kpglibc_build()
{
    kpglibc_util_conf
    cd ${OBJ_DIR};\
    cmake ${LIB_ROOT_PATH} -DKPGLIBC_TOOLCHAIN_INFO:STRING=${KPGLIBC_C_TOOLCHAIN} -DKPGLIBC_VERSION_INFO=${KPGLIBC_INFO_VERSION}\
        -DKPGLIBC_COMPONENT_NAME:STRING=${KPGLIBC_INFO_COMPONENT_NAME} -DKPGLIBC_SOFTWARE_NAME:STRING=${KPGLIBC_INFO_SOFTWARE_NAME}\
        -DKPGLIBC_PRODUCT_VERSION:STRING=${KPGLIBC_INFO_PRODUCT_VERSION} ${COVERAGE_OPTION};\
    make -C ${OBJ_DIR} -j16
    echo "build success"
}

function deb_pack()
{
    rm -rf ${DEBPACK_ROOT_PATH}/KPGLIBC
    mkdir -p ${DEBPACK_INCLUDE_PATH}
    mkdir -p ${DEBPACK_LIB_PATH}

    cp -rf ${LIB_ROOT_PATH}/include/* ${DEBPACK_INCLUDE_PATH}
    cp -rf ${OUTPUT_DIR}/* ${DEBPACK_LIB_PATH}

    sed -i "5c Standards-Version:${KPGLIBC_INFO_VERSION}" ${DEBPACK_ROOT_PATH}/DEBIAN/control
    sed -i "6c Version:${KPGLIBC_INFO_VERSION}" ${DEBPACK_ROOT_PATH}/DEBIAN/control
    sed -i "7c Package:${KPGLIBC_INFO_SOFTWARE_NAME}" ${DEBPACK_ROOT_PATH}/DEBIAN/control
    chmod a+x ${DEBPACK_ROOT_PATH}/DEBIAN/*inst ${DEBPACK_ROOT_PATH}/DEBIAN/*rm
    dpkg-deb -b ${LIB_ROOT_PATH}/package/debpack/\
        ${LIB_ROOT_PATH}/package/output/${KPGLIBC_INFO_SOFTWARE_NAME}-${KPGLIBC_INFO_VERSION}.aarch64.deb
}