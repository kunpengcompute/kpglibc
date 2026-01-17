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

cmake_minimum_required(VERSION 3.16.3)
project(KPGLIBC LANGUAGES C CXX ASM)

# ----------------------------------------------------------------------------
# inclue required cmake files:
# ----------------------------------------------------------------------------
include(cmake/KPGLIBCUtil.cmake)
include(cmake/KPGLIBCModule.cmake)

string(REGEX MATCHALL "([0-9]+)" KPGLIBC_VERSION ${KPGLIBC_VERSION_INFO})

list(GET KPGLIBC_VERSION 0 KPGLIBC_VERSION_MAJOR) # Add a feature or a new library. For example, increase the value of this parameter when adding the OMP.
list(GET KPGLIBC_VERSION 1 KPGLIBC_VERSION_MINOR) # Increases when an interface is added.
list(GET KPGLIBC_VERSION 2 KPGLIBC_VERSION_PATCH) # Increases when bugs are fixed

string(TIMESTAMP BUILDDATE "%Y.%m.%d")
set(KPGLIBC_VERSION_BUILDDATE \"${BUILDDATE}\")  # Last build time
set(KPGLIBC_LIBVERSION "${KPGLIBC_VERSION_INFO}")

configure_file(
        "${PROJECT_SOURCE_DIR}/kpglibcver.h.in"
        "${PROJECT_SOURCE_DIR}/kpglibcver.h"
)

# ----------------------------------------------------------------------------
# config the output path of the library:
# ----------------------------------------------------------------------------
set(LIBRARY_OUTPUT_PATH    "${PROJECT_SOURCE_DIR}/output")
set(CMAKE_INSTALL_PREFIX    "${PROJECT_SOURCE_DIR}/output")
message("-- The output path of lib file is        ${LIBRARY_OUTPUT_PATH}")

include(config.cmake.in)

# ----------------------------------------------------------------------------
# add_modules
# ----------------------------------------------------------------------------
if(BUILD_SHARED_LIBS)
    SET(KPGLIBC_MODULE_TYPE SHARED)
else()
    SET(KPGLIBC_MODULE_TYPE STATIC)
endif()

if(OPTIMIZE)
    add_definitions(-DHS_OPTIMIZE)
endif()

ADD_SUBDIRECTORY(src)

#show KPGLIBC modules information
string(REPLACE "KPGLIBC_" "" KPGLIBC_MODULES_BUILD_ST          "${KPGLIBC_MODULES_BUILD}")
string(REPLACE "KPGLIBC_" "" KPGLIBC_MODULES_DISABLED_USER_ST  "${KPGLIBC_MODULES_DISABLED_USER}")
string(REPLACE "KPGLIBC_" "" KPGLIBC_MODULES_DISABLED_FORCE_ST "${pORTOCL_MODULES_DISABLED_FORCE}")

set(KPGLIBC_MODULES_DISABLED_AUTO_ST "")
foreach(m ${KPGLIBC_MODULES_DISABLED_AUTO})
    set(__mdeps "")
    foreach(d ${KPGLIBC_MODULE_${m}_DEPS})
        if(d MATCHES "^KPGLIBC_" AND NOT HAVE_${d})
            list(APPEND __mdeps ${d})
        endif()
    endforeach()
    if(__mdeps)
        list(APPEND KPGLIBC_MODULES_DISABLED_AUTO_ST "${m}(deps: ${__mdeps})")
    else()
        list(APPEND KPGLIBC_MODULES_DISABLED_AUTO_ST "${m}")
    endif()
endforeach()

string(REPLACE "KPGLIBC_" "" KPGLIBC_MODULES_DISABLED_AUTO_ST  "${KPGLIBC_MODULES_DISABLED_AUTO_ST}")

message("===================The modules build information as follwos:")
message("    Public modules:         ${KPGLIBC_MODULES_PUBLIC}"        )
message("    To be built:            ${KPGLIBC_MODULES_BUILD}"         )