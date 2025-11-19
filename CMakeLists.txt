cmake_minimum_required(VERSION 3.29)
project(
  vulkan-tutorials
  VERSION 0.0.0
  DESCRIPTION "My implementations of the Vulkan tutorials"
  HOMEPAGE_URL "https://github.com/brobeson/vulkan-tutorials"
  LANGUAGES CXX
)

option(vulkan-tutorials_DEV_BUILD "Configure the build system for developing vulkan-tutorials" off)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

if(vulkan-tutorials_DEV_BUILD)
  enable_testing()
  add_subdirectory(tests)
endif()

add_subdirectory(source)
