// SPDX-License-Identifier: MIT
//
// Copyright 2026 brobeson

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
  #include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#define GLFW_INCLUDE_VULKAN // REQUIRED only for GLFW CreateWindowSurface.
#include <GLFW/glfw3.h>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

const std::vector<const char*> g_validationLayers = {
  "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif

class HelloTriangleApplication
{
public:
  void run()
  {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  GLFWwindow* m_window = nullptr;

  vk::raii::Context m_context;
  vk::raii::Instance m_instance = nullptr;
  vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;

  vk::raii::PhysicalDevice m_physicalDevice = nullptr;
  vk::raii::Device m_device = nullptr;

  vk::raii::Queue m_graphicsQueue = nullptr;

  std::vector<const char*> requiredDeviceExtension = {
    vk::KHRSwapchainExtensionName};

  void initWindow()
  {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void initVulkan()
  {
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
  }

  void mainLoop()
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

    // Get the required layers
    std::vector<const char*> requiredLayers;
    if (ENABLE_VALIDATION_LAYERS)
    {
      requiredLayers.assign(g_validationLayers.begin(),
                            g_validationLayers.end());
    }

    // Check if the required layers are supported by the Vulkan implementation.
    auto layerProperties = m_context.enumerateInstanceLayerProperties();
    auto unsupportedLayerIt = std::ranges::find_if(
      requiredLayers,
      [&layerProperties](const auto& requiredLayer)
      {
        return std::ranges::none_of(
          layerProperties,
          [requiredLayer](const auto& layerProperty)
          { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
      });
    if (unsupportedLayerIt != requiredLayers.end())
    {
      throw std::runtime_error("Required layer not supported: "
                               + std::string(*unsupportedLayerIt));
    }

    // Get the required extensions.
    auto requiredExtensions = getRequiredInstanceExtensions();

    // Check if the required extensions are supported by the Vulkan
    // implementation.
    auto extensionProperties = m_context.enumerateInstanceExtensionProperties();
    auto unsupportedPropertyIt = std::ranges::find_if(
      requiredExtensions,
      [&extensionProperties](const auto& requiredExtension)
      {
        return std::ranges::none_of(
          extensionProperties,
          [requiredExtension](const auto& extensionProperty)
          {
            return strcmp(extensionProperty.extensionName, requiredExtension)
                == 0;
          });
      });
    if (unsupportedPropertyIt != requiredExtensions.end())
    {
      throw std::runtime_error("Required extension not supported: "
                               + std::string(*unsupportedPropertyIt));
    }

    vk::InstanceCreateInfo createInfo{
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
      .ppEnabledLayerNames = requiredLayers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
      .ppEnabledExtensionNames = requiredExtensions.data()};
    m_instance = vk::raii::Instance(m_context, createInfo);
  }

  void setupDebugMessenger()
  {
    if (!ENABLE_VALIDATION_LAYERS) { return; }

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
      | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
      | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
      | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
      .messageSeverity = severityFlags,
      .messageType = messageTypeFlags,
      .pfnUserCallback = &debugCallback};
    m_debugMessenger =
      m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
  }

  bool isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice)
  {
    // Check if the physicalDevice supports the Vulkan 1.3 API version
    bool supportsVulkan1_3 =
      physicalDevice.getProperties().apiVersion >= VK_API_VERSION_1_3;

    // Check if any of the queue families support graphics operations
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    bool supportsGraphics = std::ranges::any_of(
      queueFamilies,
      [](const auto& qfp)
      { return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); });

    // Check if all required physicalDevice extensions are available
    auto availableDeviceExtensions =
      physicalDevice.enumerateDeviceExtensionProperties();
    bool supportsAllRequiredExtensions = std::ranges::all_of(
      requiredDeviceExtension,
      [&availableDeviceExtensions](const auto& requiredDeviceExtension)
      {
        return std::ranges::any_of(
          availableDeviceExtensions,
          [requiredDeviceExtension](const auto& availableDeviceExtension)
          {
            return strcmp(availableDeviceExtension.extensionName,
                          requiredDeviceExtension)
                == 0;
          });
      });

    // Check if the physicalDevice supports the required features
    auto features = physicalDevice.template getFeatures2<
      vk::PhysicalDeviceFeatures2,
      vk::PhysicalDeviceVulkan11Features,
      vk::PhysicalDeviceVulkan13Features,
      vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool supportsRequiredFeatures =
      features.template get<vk::PhysicalDeviceVulkan11Features>()
        .shaderDrawParameters
      && features.template get<vk::PhysicalDeviceVulkan13Features>()
           .dynamicRendering
      && features
           .template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
           .extendedDynamicState;

    // Return true if the physicalDevice meets all the criteria
    return supportsVulkan1_3 && supportsGraphics
        && supportsAllRequiredExtensions && supportsRequiredFeatures;
  }

  void pickPhysicalDevice()
  {
    std::vector<vk::raii::PhysicalDevice> physicalDevices =
      m_instance.enumeratePhysicalDevices();
    const auto devIter =
      std::ranges::find_if(physicalDevices,
                           [&](const auto& physicalDevice)
                           { return isDeviceSuitable(physicalDevice); });
    if (devIter == physicalDevices.end())
    {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
    m_physicalDevice = *devIter;
  }

  void createLogicalDevice()
  {
    // find the index of the first queue family that supports graphics
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
      m_physicalDevice.getQueueFamilyProperties();

    // get the first index into queueFamilyProperties which supports graphics
    auto graphicsQueueFamilyProperty = std::ranges::find_if(
      queueFamilyProperties,
      [](const auto& qfp)
      {
        return (qfp.queueFlags & vk::QueueFlagBits::eGraphics)
            != static_cast<vk::QueueFlags>(0);
      });
    assert(graphicsQueueFamilyProperty != queueFamilyProperties.end()
           && "No graphics queue family found!");

    auto graphicsIndex = static_cast<uint32_t>(std::distance(
      queueFamilyProperties.begin(), graphicsQueueFamilyProperty));

    // query for Vulkan 1.3 features
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan11Features,
                       vk::PhysicalDeviceVulkan13Features,
                       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
      featureChain = {
        {},                             // vk::PhysicalDeviceFeatures2
        {.shaderDrawParameters = true}, // vk::PhysicalDeviceVulkan11Features
        {.dynamicRendering = true},     // vk::PhysicalDeviceVulkan13Features
        {.extendedDynamicState =
           true} // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
      };

    // create a Device
    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
      .queueFamilyIndex = graphicsIndex,
      .queueCount = 1,
      .pQueuePriorities = &queuePriority};
    vk::DeviceCreateInfo deviceCreateInfo{
      .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &deviceQueueCreateInfo,
      .enabledExtensionCount =
        static_cast<uint32_t>(requiredDeviceExtension.size()),
      .ppEnabledExtensionNames = requiredDeviceExtension.data()};

    m_device = vk::raii::Device(m_physicalDevice, deviceCreateInfo);
    m_graphicsQueue = vk::raii::Queue(m_device, graphicsIndex, 0);
  }

  std::vector<const char*> getRequiredInstanceExtensions()
  {
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (ENABLE_VALIDATION_LAYERS)
    {
      extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
  }

  static VKAPI_ATTR vk::Bool32 VKAPI_CALL
  debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                vk::DebugUtilsMessageTypeFlagsEXT type,
                const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void*)
  {
    if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        || severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
      std::cerr << "validation layer: type " << to_string(type)
                << " msg: " << pCallbackData->pMessage << std::endl;
    }

    return vk::False;
  }
};

int main()
{
  try
  {
    HelloTriangleApplication app;
    app.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
