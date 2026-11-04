// SPDX-License-Identifier: MIT
//
// Copyright 2026 brobeson

#define GLFW_INCLUDE_VULKAN
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
  #include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

constexpr uint32_t WIDTH{800};
constexpr uint32_t HEIGHT{600};

class HelloTriangleApplication
{
private:
  void init_window()
  {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void init_vulkan() {}

  void main_loop()
  {
    while (!glfwWindowShouldClose(m_window)) { glfwPollEvents(); }
  }

  void cleanup()
  {
    glfwDestroyWindow(m_window);
    glfwTerminate();
  }

  GLFWwindow* m_window{nullptr};

public:
  void run()
  {
    init_window();
    init_vulkan();
    main_loop();
    cleanup();
  }
};

int main()
{
  HelloTriangleApplication app;

  try
  {
    app.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
