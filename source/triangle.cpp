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

const std::vector<const char*> validationLayers{"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
constexpr bool enableValidationLayers{false};
#else
constexpr bool enableValidationLayers{true};
#endif

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void initVulkan() { createInstance(); }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) { glfwPollEvents(); }
  }

  void cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void createInstance() {
    constexpr vk::ApplicationInfo appInfo{
      .pApplicationName = "Hello Triangle",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = vk::ApiVersion14};

    // Get the required layers.
    std::vector<const char*> requiredLayers;
    if (enableValidationLayers) {
      requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    // Check if the required layers are supported by the Vulkan implementation.
    auto layerProperties{context.enumerateInstanceLayerProperties()};
    if (std::any_of(
          std::begin(layerProperties),
          std::end(layerProperties),
          [&layerProperties](const auto& requiredLayer) {
            return std::none_of(
              std::begin(layerProperties),
              std::end(layerProperties),
              [requiredLayer](const auto& layerProperty) {
                return std::strcmp(
                         layerProperty.layerName, requiredLayer.layerName)
                    == 0;
              });
          })) {
      throw std::runtime_error(
        "One or more required layers are not supported!");
    }

    // Get the required instance extensions from GLFW.
    uint32_t glfwExtensionCount{0U};
    auto glfwExtensions{glfwGetRequiredInstanceExtensions(&glfwExtensionCount)};

    // Check if the required GLFW extensions are supported by the Vulkan
    // implementation.
    auto extensionProperties{context.enumerateInstanceExtensionProperties()};
    for (uint32_t i{0U}; i < glfwExtensionCount; ++i) {
      if (std::none_of(
            std::begin(extensionProperties),
            std::end(extensionProperties),
            [glfwExtension = glfwExtensions[i]](const auto& extensionProperty) {
              return strcmp(extensionProperty.extensionName, glfwExtension)
                  == 0;
            })) {
        throw std::runtime_error{
          "Required GLFW extension not supported: "
          + std::string{glfwExtensions[i]}};
      }
    }

    vk::InstanceCreateInfo createInfo{
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
      .ppEnabledLayerNames = requiredLayers.data(),
      .enabledExtensionCount = glfwExtensionCount,
      .ppEnabledExtensionNames = glfwExtensions};
    instance = vk::raii::Instance{context, createInfo};
  }

  GLFWwindow* window{nullptr};

  vk::raii::Context context;
  vk::raii::Instance instance{nullptr};
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}