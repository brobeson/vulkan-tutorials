// SPDX-License-Identifier: MIT
//
// Copyright 2026 brobeson

#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

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

  void init_vulkan() { createInstance(); }

  void main_loop()
  {
    while (!glfwWindowShouldClose(m_window)) { glfwPollEvents(); }
  }

  void cleanup()
  {
    glfwDestroyWindow(m_window);
    glfwTerminate();
  }

  void createInstance()
  {
    constexpr vk::ApplicationInfo appInfo{
      .pApplicationName = "Hello Triangle",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = vk::ApiVersion14};

    // Get the required instance extensions from GLFW.
    uint32_t glfwExtensionCount{0U};
    auto glfwExtensions{glfwGetRequiredInstanceExtensions(&glfwExtensionCount)};

    // Check if the required GLFW extensions are supported by the Vulkan
    // implementation.
    auto extensionProperties{m_context.enumerateInstanceExtensionProperties()};
    for (uint32_t i{0U}; i < glfwExtensionCount; ++i)
    {
      if (std::none_of(
            std::begin(extensionProperties),
            std::end(extensionProperties),
            [glfwExtension = glfwExtensions[i]](const auto& extensionProperty)
            {
              return strcmp(extensionProperty.extensionName, glfwExtension)
                  == 0;
            }))
      {
        throw std::runtime_error{"Required GLFW extension not supported: "
                                 + std::string{glfwExtensions[i]}};
      }
    }

    vk::InstanceCreateInfo createInfo{
      .pApplicationInfo = &appInfo,
      .enabledExtensionCount = glfwExtensionCount,
      .ppEnabledExtensionNames = glfwExtensions};
    m_instance = vk::raii::Instance{m_context, createInfo};
  }

  GLFWwindow* m_window{nullptr};
  vk::raii::Context m_context;
  vk::raii::Instance m_instance{nullptr};

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
