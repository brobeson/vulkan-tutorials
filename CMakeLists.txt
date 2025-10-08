cmake_minimum_required(VERSION 3.29)
project(
  CppStarter
  VERSION 0.0.0
  DESCRIPTION "A template C++ project."
  HOMEPAGE_URL "https://github.com/brobeson/CppStarter"
  LANGUAGES CXX)

option(CppStarter_DEV_BUILD "Configure the build system for developing CppStarter" off)

if(CppStarter_DEV_BUILD)
  enable_testing()
  add_subdirectory(tests)
endif()
