# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2020, Intel Corporation

cmake_minimum_required(VERSION 3.3)
project(pmemkv_basic CXX)

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
	pkg_check_modules(LIBPMEMKV REQUIRED libpmemkv)
else()
	message(FATAL_ERROR "pkg-config not found")
endif()

include_directories(${LIBPMEMKV_INCLUDE_DIRS})
link_directories(${LIBPMEMKV_LIBRARY_DIRS})
add_executable(pmemkv_comparator_cpp pmemkv_comparator.cpp)
target_link_libraries(pmemkv_comparator_cpp ${LIBPMEMKV_LIBRARIES})
