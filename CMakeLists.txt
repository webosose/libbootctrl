# Copyright (c) 2022 LG Electronics, Inc.
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
#
# SPDX-License-Identifier: Apache-2.0

#
# sam/CMakeLists.txt
#

cmake_minimum_required(VERSION 2.8.7)

project(bootctrl C)

include(webOS/webOS)

webos_modules_init(1 0 0 QUALIFIER RC4)
webos_component(1 0 0)
webos_distro_dep()

include(FindPkgConfig)

# dependencies
pkg_check_modules(LIBUBOOT REQUIRED libubootenv)
include_directories(${LIBUBOOT_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${LIBUBOOT_CFLAGS_OTHER})

# add additional flags
webos_add_linker_options(ALL --no-undefined)
include_directories(include/)

# files
file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE INCLUDES include/*.h)
file(GLOB_RECURSE TEST_SOURCES test/*.c)

# for lib
set(LIB_NAME ${CMAKE_PROJECT_NAME})
add_library(${LIB_NAME} STATIC ${SOURCES})
target_link_libraries(${LIB_NAME} ${LIBUBOOT_LDFLAGS})

# for test bin
set(BIN_NAME ${CMAKE_PROJECT_NAME}.test)
add_executable(${BIN_NAME} ${TEST_SOURCES})
target_link_libraries(${BIN_NAME} ${LIBUBOOT_LDFLAGS} ${LIB_NAME})

# install
install(FILES ${INCLUDES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bootctrl)
install(TARGETS ${LIB_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS ${BIN_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
