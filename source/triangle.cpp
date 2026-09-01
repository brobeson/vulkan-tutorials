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
#include <map>
#include <stdexcept>

constexpr uint32_t WIDTH{800};
constexpr uint32_t HEIGHT{600};

const std::vector<const char*> validationLayers{"VK_LAYER_KHRONOS_validation"};
std::vector<const char*> requiredDeviceExtension
  = {vk::KHRSwapchainExtensionName};

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

  void initVulkan() {
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
  }

  void pickPhysicalDevice() {
    std::vector<const char*> deviceExtensions = {
      vk::KHRSwapchainExtensionName,
      vk::KHRSpirv14ExtensionName,
      vk::KHRSynchronization2ExtensionName,
      vk::KHRCreateRenderpass2ExtensionName};
    auto devices{instance.enumeratePhysicalDevices()};
    const auto devIter{std::find_if(
      std::begin(devices),
      std::end(devices),
      [&](const vk::raii::PhysicalDevice& device) {
        auto queueFamilies{device.getQueueFamilyProperties()};
        bool isSuitable
          = device.getProperties().apiVersion >= VK_API_VERSION_1_3;
        const auto qfpIter = std::find_if(
          std::begin(queueFamilies),
          std::end(queueFamilies),
          [](const vk::QueueFamilyProperties& qfp) {
            return (qfp.queueFlags & vk::QueueFlagBits::eGraphics)
                != static_cast<vk::QueueFlags>(0);
          });
        isSuitable = isSuitable && (qfpIter != queueFamilies.end());
        auto extensions{device.enumerateDeviceExtensionProperties()};
        bool found{true};
        for (const auto& extension : deviceExtensions) {
          auto extensionIter = std::find_if(
            std::begin(extensions),
            std::end(extensions),
            [extension](const auto& ext) {
              return std::strcmp(ext.extensionName, extension) == 0;
            });
          found = found && extensionIter != extensions.end();
        }
        isSuitable = isSuitable && found;
        if (isSuitable) { physicalDevice = device; }
        return isSuitable;
      })};
    if (devIter == devices.end()) {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
    /*
    if (devices.empty()) {
      throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::multimap<int, vk::raii::PhysicalDevice> candidates;

    for (const auto& device : devices) {
      auto deviceProperties{device.getProperties()};
      auto deviceFeatures{device.getFeatures()};
      uint32_t score{0U};

      // Discrete GPUs have a significant performance advantage
      if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000U;
      }

      // Maximum possible size of textures affects graphics quality
      score += deviceProperties.limits.maxImageDimension2D;

      // Application can't function without geometry shaders
      if (!deviceFeatures.geometryShader) { continue; }
      candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
      physicalDevice = candidates.rbegin()->second;
    } else {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
      */
  }

  uint32_t findQueueFamilies(vk::raii::PhysicalDevice physicalDevice) {
    // find the index of the first queue family that supports graphics
    auto queueFamilyProperties{physicalDevice.getQueueFamilyProperties()};

    // get the first index into queueFamilyProperties which supports graphics
    for (auto i{0}; i < queueFamilyProperties.size(); ++i) {
      if (
        (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
        && physicalDevice.getSurfaceSupportKHR(i, *surface)) {
        // found a queue family that supports both graphics and presentation
        return i;
      }
    }
    throw std::runtime_error(
      "could not find a queue for graphics and presentation");
    // auto graphicsQueueFamilyProperty{std::find_if(
    //   std::begin(queueFamilyProperties),
    //   std::end(queueFamilyProperties),
    //   [&physicalDevice](vk::QueueFamilyProperties& qfp) {
    //     return (qfp.queueFlags & vk::QueueFlagBits::eGraphics)
    //         && physicalDevice.getSurfaceSupport(graphicsIndex, *surface);
    //   })};
    // return static_cast<uint32_t>(std::distance(
    //   queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
  }

  bool isDeviceSuitable(vk::raii::PhysicalDevice physicalDevice) {
    auto deviceProperties{physicalDevice.getProperties()};
    auto deviceFeatures{physicalDevice.getFeatures()};
    if (
      deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu
      && deviceFeatures.geometryShader) {
      return true;
    }
    return false;
  }

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

  void createLogicalDevice() {
    // find the index of the first queue family that supports graphics
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties
      = physicalDevice.getQueueFamilyProperties();

    // get the first index into queueFamilyProperties which supports both
    // graphics and present
    uint32_t queueIndex = ~0;
    for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size();
         qfpIndex++) {
      if (
        (queueFamilyProperties[qfpIndex].queueFlags
         & vk::QueueFlagBits::eGraphics)
        && physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface)) {
        // found a queue family that supports both graphics and present
        queueIndex = qfpIndex;
        break;
      }
    }
    if (queueIndex == ~0) {
      throw std::runtime_error(
        "Could not find a queue for graphics and present -> terminating");
    }

    // query for Vulkan 1.3 features
    vk::StructureChain<
      vk::PhysicalDeviceFeatures2,
      vk::PhysicalDeviceVulkan11Features,
      vk::PhysicalDeviceVulkan13Features,
      vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
      featureChain = {
        {},                              // vk::PhysicalDeviceFeatures2
        {.shaderDrawParameters = true},  // vk::PhysicalDeviceVulkan11Features
        {.dynamicRendering = true},      // vk::PhysicalDeviceVulkan13Features
        {.extendedDynamicState
         = true}  // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
      };

    // create a Device
    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
      .queueFamilyIndex = queueIndex,
      .queueCount = 1,
      .pQueuePriorities = &queuePriority};
    vk::DeviceCreateInfo deviceCreateInfo{
      .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &deviceQueueCreateInfo,
      .enabledExtensionCount
      = static_cast<uint32_t>(requiredDeviceExtension.size()),
      .ppEnabledExtensionNames = requiredDeviceExtension.data()};

    device = vk::raii::Device(physicalDevice, deviceCreateInfo);
    queue = vk::raii::Queue(device, queueIndex, 0);
  }

  void createSurface() {
    VkSurfaceKHR _surface{nullptr};
    if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
      throw std::runtime_error{"failed to create window surface!"};
    }
    surface = vk::raii::SurfaceKHR{instance, _surface};
  }

  GLFWwindow* window{nullptr};
  vk::raii::Context context;
  vk::raii::Instance instance{nullptr};
  vk::raii::PhysicalDevice physicalDevice{nullptr};
  vk::raii::Device device{nullptr};
  vk::raii::Queue graphicsQueue{nullptr};
  vk::raii::Queue presentQueue{nullptr};
  vk::raii::Queue queue{nullptr};
  vk::raii::SurfaceKHR surface{nullptr};
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