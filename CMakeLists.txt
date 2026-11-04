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

find_package(glfw3 REQUIRED)
add_executable(triangle source/triangle.cpp)
target_link_libraries(triangle PRIVATE glfw)
