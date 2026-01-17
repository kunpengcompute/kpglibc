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

# clean modules info which needs to be recalculated
set(KPGLIBC_MODULES_PUBLIC         "" CACHE INTERNAL "List of KPGLIBC modules marked for export")
set(KPGLIBC_MODULES_BUILD          "" CACHE INTERNAL "List of KPGLIBC modules included into the build")
set(KPGLIBC_MODULES_DISABLED_USER  "" CACHE INTERNAL "List of KPGLIBC modules explicitly disabled by user")
set(KPGLIBC_MODULES_DISABLED_AUTO  "" CACHE INTERNAL "List of KPGLIBC modules implicitly disabled due to dependencies")
set(KPGLIBC_MODULES_DISABLED_FORCE "" CACHE INTERNAL "List of KPGLIBC modules which can not be build in current configuration")

# adds dependencies to KPGLIBC module
# Usage:
#   add_dependencies(KPGLIBC_<name> [REQUIRED] [<list of dependencies>] [OPTIONAL <list of modules>])
# Notes:
# * <list of dependencies> - can include full names of modules or full pathes to shared/static libraries or cmake targets

macro(KPGLIBC_add_dependencies full_modname)
    #we don't clean the dependencies here to allow this macro several times for every module
    foreach(d "REQUIRED" ${ARGN})
        if(d STREQUAL "REQUIRED")
            set(__depsvar KPGLIBC_MODULE_${full_modname}_REQ_DEPS)
        elseif(d STREQUAL "OPTIONAL")
            set(__depsvar KPGLIBC_MODULE_${full_modname}_OPT_DEPS)
        else()
            list(APPEND ${__depsvar} "KPGLIBC_${d}")
        endif()
    endforeach()
    unset(__depsvar)

    KPGLIBC_list_unique(KPGLIBC_MODULE_${full_modname}_REQ_DEPS)
    KPGLIBC_list_unique(KPGLIBC_MODULE_${full_modname}_OPT_DEPS)

    set(KPGLIBC_MODULE_${full_modname}_REQ_DEPS ${KPGLIBC_MODULE_${full_modname}_REQ_DEPS} CACHE INTERNAL "Required dependencies of ${full_modname} module")
    set(KPGLIBC_MODULE_${full_modname}_OPT_DEPS ${KPGLIBC_MODULE_${full_modname}_OPT_DEPS} CACHE INTERNAL "Optional dependencies of ${full_modname} module")

endmacro()

# declare new KPGLIBC module in current folder
# Usage:
#   KPGLIBC_add_module(<name> [INTERNAL|BINDINGS] [REQUIRED] [<list of dependencies>] [OPTIONAL <list of optional dependencies>])
# Example:
#   KPGLIBC_add_module(yaom INTERNAL KPGLIBC_core KPGLIBC_highgui KPGLIBC_flann OPTIONAL KPGLIBC_gpu)
macro(KPGLIBC_add_module _name)
    string(TOLOWER "${_name}" name)
    set(the_module     ${name})
    set(the_module_res ${name})

    # the first pass - collect modules info, the second pass - create targets
    if(KPGLIBC_INITIAL_PASS)
        #guard agains redefinition
        if(";${KPGLIBC_MODULES_BUILD};${KPGLIBC_MODULES_DISABLED_USER};" MATCHES ";${the_module};")
            message(FATAL_ERROR "Redefinition of the ${the_module} module.
            at:                    ${CMAKE_CURRENT_SOURCE_DIR}
            previously defined at: ${KPGLIBC_MODULE_${the_module}_LOCATION}
            ")
        endif()

        if(NOT DEFINED the_description)
            set(the_description "The ${name} KPGLIBC module")
        endif()

        if(NOT DEFINED BUILD_${the_module}_INIT)
            set(BUILD_${the_module}_INIT ON)
        endif()


        # create option to enable/disable this module
        option(BUILD_${the_module} "Include ${the_module} module into the KPGLIBC build" ${BUILD_${the_module}_INIT})

        # remember the module details
        set(KPGLIBC_MODULE_${the_module}_DESCRIPTION "${the_description}"          CACHE INTERNAL "Brief description of ${the_module} module")
        set(KPGLIBC_MODULE_${the_module}_LOCATION    "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Location of ${the_module} module sources")

        # parse list of dependencies
        if("${ARGV1}" STREQUAL "INTERNAL" OR "${ARGV1}" STREQUAL "BINDINGS")
            set(KPGLIBC_MODULE_${the_module}_CLASS "${ARGV1}" CACHE INTERNAL "The cathegory of the module")
            set(__KPGLIBC_argn__ ${ARGN})
            list(REMOVE_AT __KPGLIBC_argn__ 0)
            KPGLIBC_add_dependencies(${the_module} ${__KPGLIBC_argn__})
            unset(__KPGLIBC_argn__)
        else()
            set(KPGLIBC_MODULE_${the_module}_CLASS "PUBLIC" CACHE INTERNAL "The cathegory of the module")
            KPGLIBC_add_dependencies(${the_module} ${ARGN})
            if(BUILD_${the_module})
                set(KPGLIBC_MODULES_PUBLIC ${KPGLIBC_MODULES_PUBLIC} "${the_module}" CACHE INTERNAL "List of KPGLIBC modules marked for export")
            endif()
        endif()

        if(BUILD_${the_module})
            set(KPGLIBC_MODULES_BUILD ${KPGLIBC_MODULES_BUILD} "${the_module}" CACHE INTERNAL "List of KPGLIBC modules included into the build")
        else()
            set(KPGLIBC_MODULES_DISABLED_USER ${KPGLIBC_MODULES_DISABLED_USER} "${the_module}" CACHE INTERNAL "List of KPGLIBC modules explicitly disabled by user")
        endif()

        return()
    else(KPGLIBC_INITIAL_PASS) #
        if(NOT BUILD_${the_module})
            return() # extra protection from redefinition
        endif()
        project(${the_module})
    endif(KPGLIBC_INITIAL_PASS)
endmacro()

# excludes module from current configuration
macro(KPGLIBC_disable_module module)
    set(__modname ${module})
    if(NOT __modname MATCHES "^KPGLIBC_")
        set(__modname KPGLIBC_${module})
    endif()
    list(APPEND KPGLIBC_MODULES_DISABLED_FORCE "${__modname}")
    set(HAVE_${__modname} OFF CACHE INTERNAL "Module ${__modname} can not be built in current configuration")
    set(KPGLIBC_MODULE_${__modname}_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Location of ${__modname} module sources")
    set(KPGLIBC_MODULES_DISABLED_FORCE "${KPGLIBC_MODULES_DISABLED_FORCE}" CACHE INTERNAL "List of KPGLIBC modules which can not be build in current configuration")
    if(BUILD_${__modname})
        # touch variable controlling build of the module to suppress "unused variable" CMake warning
    endif()
    unset(__modname)
    return() # leave the current folder
endmacro()

macro(__KPGLIBC_module_turn_off the_module)
  list(REMOVE_ITEM KPGLIBC_MODULES_DISABLED_AUTO "${the_module}")
  list(APPEND      KPGLIBC_MODULES_DISABLED_AUTO "${the_module}")
  list(REMOVE_ITEM KPGLIBC_MODULES_BUILD "${the_module}")
  list(REMOVE_ITEM KPGLIBC_MODULES_PUBLIC "${the_module}")
  set(HAVE_${the_module} OFF CACHE INTERNAL "Module ${the_module} can not be built in current configuration")
endmacro()

macro(__KPGLIBC_flatten_module_required_dependencies the_module)
    set(__flattened_deps "")
    set(__resolved_deps "")
    set(__req_depends ${KPGLIBC_MODULE_${the_module}_REQ_DEPS})

    while(__req_depends)
        KPGLIBC_list_pop_front(__req_depends __dep)

        #depends on self
        if(__dep STREQUAL the_module)
            __KPGLIBC_module_turn_off(${the_module}) 
            break()

            #depends on disabled module
        elseif(";${KPGLIBC_MODULES_DISABLED_USER};${KPGLIBC_MODULES_DISABLED_AUTO};" MATCHES ";${__dep};")

            __KPGLIBC_module_turn_off(${the_module}) # depends on disabled module
            #list(APPEND __flattened_deps "${__dep}")
            break() #TODO

            #depends on build-enabled module
        elseif(";${KPGLIBC_MODULES_BUILD};" MATCHES ";${__dep};")

            if(";${__resolved_deps};" MATCHES ";${__dep};")
                list(APPEND __flattened_deps "${__dep}") # all dependencies of this module are already resolved
            else()
                # put all required subdependencies before this dependency and mark it as resolved
                list(APPEND __resolved_deps "${__dep}")
                list(INSERT __req_depends 0 ${KPGLIBC_MODULE_${__dep}_REQ_DEPS} ${__dep})
            endif()

        elseif(__dep MATCHES "^KPGLIBC_")
            __KPGLIBC_module_turn_off(${the_module}) # depends on missing module
            message(WARNING "Unknown \"${__dep}\" module is listened in the dependencies of \"${the_module}\" module")
            break()
        else()
            # skip non-modules
        endif()
    endwhile()

    if(__flattened_deps)
        list(REMOVE_DUPLICATES __flattened_deps)
        set(KPGLIBC_MODULE_${the_module}_DEPS ${__flattened_deps})
    else()
        set(KPGLIBC_MODULE_${the_module}_DEPS "")
    endif()

    KPGLIBC_clear_vars(__resolved_deps __flattened_deps __req_depends __dep)
endmacro()

macro(__KPGLIBC_flatten_module_optional_dependencies the_module)
  set(__flattened_deps "")
  set(__resolved_deps "")
  set(__opt_depends ${KPGLIBC_MODULE_${the_module}_REQ_DEPS} ${KPGLIBC_MODULE_${the_module}_OPT_DEPS})

  while(__opt_depends)
    KPGLIBC_list_pop_front(__opt_depends __dep)
    if(__dep STREQUAL the_module)
        __KPGLIBC_module_turn_off(${the_module}) 
      break()

    elseif(";${KPGLIBC_MODULES_BUILD};" MATCHES ";${__dep};")
      if(";${__resolved_deps};" MATCHES ";${__dep};")
        list(APPEND __flattened_deps "${__dep}") # all dependencies of this module are already resolved
      else()
        # put all subdependencies before this dependency and mark it as resolved
        list(APPEND __resolved_deps "${__dep}")
        list(INSERT __opt_depends 0 ${KPGLIBC_MODULE_${__dep}_REQ_DEPS} ${KPGLIBC_MODULE_${__dep}_OPT_DEPS} ${__dep})
      endif()
    else()
      # skip non-modules or missing modules
    endif()
  endwhile()

  if(__flattened_deps)
    list(REMOVE_DUPLICATES __flattened_deps)
    set(KPGLIBC_MODULE_${the_module}_DEPS ${__flattened_deps})
  else()
    set(KPGLIBC_MODULE_${the_module}_DEPS "")
  endif()

  KPGLIBC_clear_vars(__resolved_deps __flattened_deps __opt_depends __dep)
endmacro()

macro(__KPGLIBC_flatten_module_dependencies)

    foreach(m ${KPGLIBC_MODULES_DISABLED_USER})
        set(HAVE_${m} OFF CACHE INTERNAL "Module ${m} will not be built in current configuration")
    endforeach()

    foreach(m ${KPGLIBC_MODULES_BUILD})
        set(HAVE_${m} ON CACHE INTERNAL "Module ${m} will be built in current configuration")
        __KPGLIBC_flatten_module_required_dependencies(${m})
        set(KPGLIBC_MODULE_${m}_DEPS ${KPGLIBC_MODULE_${m}_DEPS} CACHE INTERNAL "Flattened required dependencies of ${m} module")
    endforeach()

    foreach(m ${KPGLIBC_MODULES_BUILD})
        __KPGLIBC_flatten_module_optional_dependencies(${m})

        # save dependencies from other modules
        set(KPGLIBC_MODULE_${m}_DEPS ${KPGLIBC_MODULE_${m}_DEPS} CACHE INTERNAL "Flattened dependencies of ${m} module")
        # save extra dependencies
        set(KPGLIBC_MODULE_${m}_DEPS_EXT ${KPGLIBC_MODULE_${m}_REQ_DEPS} ${KPGLIBC_MODULE_${m}_OPT_DEPS})
        if(KPGLIBC_MODULE_${m}_DEPS_EXT AND KPGLIBC_MODULE_${m}_DEPS)
            list(REMOVE_ITEM KPGLIBC_MODULE_${m}_DEPS_EXT ${KPGLIBC_MODULE_${m}_DEPS})
        endif()

        set(KPGLIBC_MODULE_${m}_DEPS_EXT ${KPGLIBC_MODULE_${m}_DEPS_EXT} CACHE INTERNAL "Extra dependencies of ${m} module")
    endforeach()

    # order modules by dependencies
    set(KPGLIBC_MODULES_BUILD_ "")
    foreach(m ${KPGLIBC_MODULES_BUILD})
        list(APPEND KPGLIBC_MODULES_BUILD_ ${KPGLIBC_MODULE_${m}_DEPS} ${m})
    endforeach()
    KPGLIBC_list_unique(KPGLIBC_MODULES_BUILD_)

    set(KPGLIBC_MODULES_PUBLIC        ${KPGLIBC_MODULES_PUBLIC}        CACHE INTERNAL "List of KPGLIBC modules marked for export")
    set(KPGLIBC_MODULES_BUILD         ${KPGLIBC_MODULES_BUILD_}        CACHE INTERNAL "List of KPGLIBC modules included into the build")
    set(KPGLIBC_MODULES_DISABLED_AUTO ${KPGLIBC_MODULES_DISABLED_AUTO} CACHE INTERNAL "List of KPGLIBC modules implicitly disabled due to dependencies")
endmacro()
# collect modules from specified directories
# NB: must be called only once!
macro(KPGLIBC_glob_modules)

    if(DEFINED KPGLIBC_INITIAL_PASS)
        message(FATAL_ERROR "Perf_IPP has already loaded its modules. Calling KPGLIBC_glob_modules second time is not allowed.")
    endif()

    set(__directories_observed "")

    # collect modules
    set(KPGLIBC_INITIAL_PASS ON)

    foreach(__path ${ARGN})
        KPGLIBC_get_real_path(__path "${__path}")

        list(FIND __directories_observed "${__path}" __pathIdx)
        if(__pathIdx GREATER -1)
            message(FATAL_ERROR "The directory ${__path} is observed for KPGLIBC modules second time.")
        endif()

        list(APPEND __directories_observed "${__path}")
        message("--Source code path = ${__path}")

        file(GLOB __KPGLIBCmodules RELATIVE "${__path}" "${__path}/*")

        if(__KPGLIBCmodules)
            list(SORT __KPGLIBCmodules)
            foreach(mod ${__KPGLIBCmodules})

                KPGLIBC_get_real_path(__modpath "${__path}/${mod}")
                if(EXISTS "${__modpath}/CMakeLists.txt")

                    list(FIND __directories_observed "${__modpath}" __pathIdx)
                    if(__pathIdx GREATER -1)
                        message(FATAL_ERROR "The module from ${__modpath} is already loaded.")
                    endif()
                    list(APPEND __directories_observed "${__modpath}")

                    add_subdirectory("${__modpath}" "${CMAKE_CURRENT_BINARY_DIR}/${mod}/.${mod}")

                endif()
            endforeach()
        endif()
    endforeach()

    KPGLIBC_clear_vars(__KPGLIBCmodules __directories_observed __path __modpath __pathIdx)

    # resolve dependencies 
     __KPGLIBC_flatten_module_dependencies()

    # create modules
    set(KPGLIBC_INITIAL_PASS OFF PARENT_SCOPE)
    set(KPGLIBC_INITIAL_PASS OFF)

    foreach(m ${KPGLIBC_MODULES_BUILD})
        if(m MATCHES "^KPGLIBC_")
            string(REGEX REPLACE "^KPGLIBC_" "" __shortname "${m}")
            add_subdirectory("${KPGLIBC_MODULE_${m}_LOCATION}" "${CMAKE_CURRENT_BINARY_DIR}/${__shortname}")
        else()
            add_subdirectory("${KPGLIBC_MODULE_${m}_LOCATION}" "${CMAKE_CURRENT_BINARY_DIR}/${m}")
        endif()
    endforeach()
    unset(__shortname)
endmacro()

# setup include paths for the list of passed modules
macro(KPGLIBC_include_modules)
    foreach(d ${ARGN})
        if(d MATCHES "^KPGLIBC_" AND HAVE_${d})
            if (EXISTS "${KPGLIBC_MODULE_${d}_LOCATION}/include")
                KPGLIBC_include_directories("${KPGLIBC_MODULE_${d}_LOCATION}/include")
            endif()
        elseif(EXISTS "${d}")
            KPGLIBC_include_directories("${d}")
        endif()
    endforeach()
endmacro()

# setup include path for KPGLIBC headers for specified module
# KPGLIBC_module_include_directories(<extra include directories/extra include modules>)
macro(KPGLIBC_module_include_directories)
    KPGLIBC_include_directories("${KPGLIBC_MODULE_${the_module}_LOCATION}/include"
        "${KPGLIBC_MODULE_${the_module}_LOCATION}/src"
        "${CMAKE_CURRENT_BINARY_DIR}" # for precompiled headers
        )
    KPGLIBC_include_modules(${KPGLIBC_MODULE_${the_module}_DEPS} ${ARGN})
endmacro()

# sets header and source files for the current module
# NB: all files specified as headers will be installed
# Usage:
# KPGLIBC_set_module_sources([HEADERS] <list of files> [SOURCES] <list of files>)
macro(KPGLIBC_set_module_sources)
    set(KPGLIBC_MODULE_${the_module}_HEADERS "")
    set(KPGLIBC_MODULE_${the_module}_SOURCES "")

    foreach(f "HEADERS" ${ARGN})
        if(f STREQUAL "HEADERS" OR f STREQUAL "SOURCES")
            set(__filesvar "KPGLIBC_MODULE_${the_module}_${f}")
        else()
            list(APPEND ${__filesvar} "${f}")
        endif()
    endforeach()

    # the hacky way to embeed any files into the KPGLIBC without modification of its build system. This part will be rewritten
    #if(COMMAND KPGLIBC_get_module_external_sources)
    #  KPGLIBC_get_module_external_sources()
    #endif()

    # use full paths for module to be independent from the module location
    KPGLIBC_convert_to_full_paths(KPGLIBC_MODULE_${the_module}_HEADERS)

    set(KPGLIBC_MODULE_${the_module}_HEADERS ${KPGLIBC_MODULE_${the_module}_HEADERS} CACHE INTERNAL "List of header files for ${the_module}")
    set(KPGLIBC_MODULE_${the_module}_SOURCES ${KPGLIBC_MODULE_${the_module}_SOURCES} CACHE INTERNAL "List of source files for ${the_module}")
endmacro()

# creates KPGLIBC module in current folder
# creates new target, configures standard dependencies, compilers flags, install rules
# Usage:
#   KPGLIBC_create_module(<extra link dependencies>)
#   KPGLIBC_create_module(SKIP_LINK)
macro(KPGLIBC_create_module)
    ADD_LIBRARY(${the_module} ${KPGLIBC_MODULE_TYPE} ${KPGLIBC_MODULE_${the_module}_HEADERS} ${KPGLIBC_MODULE_${the_module}_SOURCES})

    if(NOT "${ARGN}" STREQUAL "SKIP_LINK")
        # 设置默认库依赖
        set(libraries ${KPGLIBC_MODULE_${the_module}_DEPS} ${KPGLIBC_MODULE_${the_module}_DEPS_EXT} ${KPGLIBC_LINKER_LIBS} ${ARGN})
        # 链接库
        TARGET_LINK_LIBRARIES(${the_module} ${libraries})
    endif()

    if(ENABLE_SOLUTION_FOLDERS)
        SET_TARGET_PROPERTIES(${the_module} PROPERTIES FOLDER "modules")
    endif()

    SET_TARGET_PROPERTIES(${the_module} PROPERTIES
        OUTPUT_NAME "${the_module}"
        DEBUG_POSTFIX "${KPGLIBC_DEBUG_POSTFIX}"
        ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
        LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
        RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
        COMPILE_FLAGS ${CMAKE_C_FLAGS}
        INSTALL_NAME_DIR lib
        )

    # For dynamic link numbering convenions
    if(NOT ANDROID)
        # Android SDK build scripts can include only .so files into final .apk
        # As result we should not set version properties for Android
        SET_TARGET_PROPERTIES(${the_module} PROPERTIES
            VERSION   ${KPGLIBC_LIBVERSION}
            )
    endif()

    if(BUILD_SHARED_LIBS)
        if(MSVC)
            SET_TARGET_PROPERTIES(${the_module} PROPERTIES DEFINE_SYMBOL CVAPI_EXPORTS)
        else()
            ADD_DEFINITIONS(-DCVAPI_EXPORTS)
        endif()
    endif()

    if(MSVC)
        if(CMAKE_CROSSCOMPILING)
            SET_TARGET_PROPERTIES(${the_module} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:secchk")
        endif()
        SET_TARGET_PROPERTIES(${the_module} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libc /DEBUG")
    endif()

    #FILE(REMOVE_RECURSE ${KPGLIBC_LIB_INSTALL_PATH})
    #FILE(REMOVE_RECURSE ${KPGLIBC_INCLUDE_INSTALL_PATH})

#    INSTALL(TARGETS ${the_module}
#        RUNTIME DESTINATION ${KPGLIBC_RUNTIME_INSTALL_PATH} 
#        LIBRARY DESTINATION ${KPGLIBC_LIB_INSTALL_PATH}     
#        ARCHIVE DESTINATION ${KPGLIBC_LIB_INSTALL_PATH}     
#        COMPONENT main
#            )

    # only "public" headers need to be installed
    if(KPGLIBC_MODULE_${the_module}_HEADERS AND ";${KPGLIBC_MODULES_PUBLIC};" MATCHES ";${the_module};")
        foreach(hdr ${KPGLIBC_MODULE_${the_module}_HEADERS})
            if(hdr MATCHES "([^/])+.h(..)?$")
                INSTALL(FILES ${hdr} DESTINATION "${KPGLIBC_INCLUDE_INSTALL_PATH}/${the_module_res}" COMPONENT main)
            endif()
        endforeach()
    endif()
endmacro()

# ensures that all passed modules are available
# sets KPGLIBC_DEPENDENCIES_FOUND variable to TRUE/FALSE
macro(KPGLIBC_check_dependencies)
    set(KPGLIBC_DEPENDENCIES_FOUND TRUE)
    foreach(d ${ARGN})
        if(d MATCHES "^KPGLIBC_[^ ]+$" AND NOT HAVE_${d})
            set(KPGLIBC_DEPENDENCIES_FOUND FALSE)
            break()
        endif()
    endforeach()
endmacro()

# finds and sets headers and sources for the standard KPGLIBC module
# Usage:
# KPGLIBC_glob_module_sources(<extra sources&headers in the same format as used in KPGLIBC_set_module_sources>)
macro(KPGLIBC_glob_module_sources)
  file(GLOB_RECURSE lib_srcs "src/*.c" "src/*.cpp")
  file(GLOB_RECURSE lib_asm "src/*.s" "src/*.S")
  file(GLOB_RECURSE lib_int_hdrs "src/*.hpp" "src/*.h")
  file(GLOB lib_hdrs "include/${name}/*.hpp" "include/${name}/*.h")

  source_group("Src" FILES ${lib_srcs} ${lib_asm} ${lib_int_hdrs})
  source_group("Include" FILES ${lib_hdrs})

  KPGLIBC_set_module_sources(${ARGN} HEADERS ${lib_hdrs} SOURCES ${lib_srcs} ${lib_asm} ${lib_int_hdrs})
endmacro()

# short command for adding simple KPGLIBC module
# see KPGLIBC_add_module for argument details
# Usage:
# KPGLIBC_define_module(module_name  [INTERNAL] [REQUIRED] [<list of dependencies>] [OPTIONAL <list of optional dependencies>])
macro(KPGLIBC_define_module module_name)
  KPGLIBC_add_module(${module_name} ${ARGN})
  KPGLIBC_glob_module_sources()
  KPGLIBC_module_include_directories()
  KPGLIBC_create_module()
endmacro()

macro(KPGLIBC_glob_module_sources_neon)
  set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
  set(CMAKE_ASM_CREATE_SHARED_LIBRARY
    "${CMAKE_ASM_COMPILER} -shared -o <TARGET> <OBJECTS>"
    )   
  file(GLOB_RECURSE lib_srcs "src/*.c" "src/*.cpp")
  file(GLOB_RECURSE lib_asm "src/*.s" "src/*.S")
  file(GLOB_RECURSE lib_int_hdrs "src/*.hpp" "src/*.h")
  set(CORE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../core_sve/src")  
  file(GLOB core_src
    "${CORE_SRC_DIR}/cpufeature.c"
    "${CORE_SRC_DIR}/version.c"
    )
  file(GLOB lib_hdrs "include/${name}/*.hpp" "include/${name}/*.h")

  source_group("Src" FILES ${lib_srcs} ${lib_asm} ${lib_int_hdrs} ${core_src})
  source_group("Include" FILES ${lib_hdrs})

  KPGLIBC_set_module_sources(${ARGN} HEADERS ${lib_hdrs} SOURCES ${lib_srcs} ${lib_asm} ${lib_int_hdrs} ${core_src})
endmacro()

macro(KPGLIBC_define_module_neon module_name)
  KPGLIBC_add_module(${module_name} ${ARGN})
  KPGLIBC_glob_module_sources_neon()
  KPGLIBC_module_include_directories()
  KPGLIBC_create_module()
endmacro()

macro(KPGLIBC_glob_module_sources_time)
  file(GLOB_RECURSE lib_srcs "src/*.c" "src/*.cpp")
  file(GLOB_RECURSE lib_asm "src/*.s" "src/*.S")
  file(GLOB_RECURSE lib_int_hdrs "src/*.hpp" "src/*.h")
  set(CORE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../core_sve/src")  
  file(GLOB core_src
    "${CORE_SRC_DIR}/cpufeature.c"
    "${CORE_SRC_DIR}/version.c"
    )
  file(GLOB lib_hdrs "include/${name}/*.hpp" "include/${name}/*.h")

  source_group("Src" FILES ${lib_srcs} ${lib_asm} ${lib_int_hdrs} ${core_src})
  source_group("Include" FILES ${lib_hdrs})

  KPGLIBC_set_module_sources(${ARGN} HEADERS ${lib_hdrs} SOURCES ${lib_srcs} ${lib_asm} ${lib_int_hdrs} ${core_src})
endmacro()

macro(KPGLIBC_define_module_time module_name)
  KPGLIBC_add_module(${module_name} ${ARGN})
  KPGLIBC_glob_module_sources_neon()
  KPGLIBC_module_include_directories()
  KPGLIBC_create_module()
endmacro()