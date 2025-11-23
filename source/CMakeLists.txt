add_executable(triangle triangle.cpp)
target_link_libraries(triangle PRIVATE glfw)
target_compile_features(triangle PRIVATE cxx_std_17)
