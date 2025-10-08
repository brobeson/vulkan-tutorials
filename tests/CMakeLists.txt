# cspell:words fetchcontent makeavailable isystem

include(FetchContent)
fetchcontent_declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.6.0
)
fetchcontent_makeavailable(Catch2)
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)

# Exclude Catch2 from tooling like Cppcheck and clang-tidy.
# 1. Don't write the build commands to compile_commands.json.
# 2. Make the header search path a system path (-isystem) instead of
#    non-system (-I).
set_target_properties(Catch2 Catch2WithMain PROPERTIES EXPORT_COMPILE_COMMANDS false)
get_target_property(_include_directories Catch2 INCLUDE_DIRECTORIES)
if(_include_directories)
  set_target_properties(Catch2 PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_include_directories}")
endif()

add_executable(CppStarterTest test.cpp)
target_link_libraries(CppStarterTest PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(CppStarterTest)