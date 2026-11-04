# SPDX-License-Identifier: MIT
#
# Copyright 2026 brobeson

cmake_minimum_required(VERSION 4.4)
project(
  vulkan-tutorials
  VERSION 0.0.0
  DESCRIPTION "My implementation of the Vulkan tutorials"
  HOMEPAGE_URL "https://github.com/brobeson/vulkan-tutorials"
  LANGUAGES CXX
)

# By default, don't use C++ modules.
if(NOT DEFINED CMAKE_CXX_SCAN_FOR_MODULES)
  set(CMAKE_CXX_SCAN_FOR_MODULES off)
endif()

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED CONFIG)
add_executable(triangle source/triangle.cpp)
target_compile_features(triangle PRIVATE cxx_std_20)
target_link_libraries(triangle PRIVATE glfw Vulkan::Vulkan)
# Put this here so I don't have to keep re-adding every time I paste the tutorial code.
target_compile_definitions(triangle PRIVATE VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)
