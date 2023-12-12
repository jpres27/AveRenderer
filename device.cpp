#include "device.h"

#include <vector>
#include <cstdio>
#include <unordered_set>

VkInstance instance;

std::vector<const char *> get_required_extensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

//  if (enableValidationLayers) {
//    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//  }

  return extensions;
}

int create_instance() {
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Hello Triangle";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    //Get a GLFW extension for Vulkan to be able to deal with window system
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    create_info.enabledExtensionCount = glfwExtensionCount;
    create_info.ppEnabledExtensionNames = glfwExtensions;

    // Retrieve list of supported extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::unordered_set<std::string> available;
    printf("Available extensions: \n");
    for (const auto& extension : extensions) {
        printf("%s\n", extension.extensionName);
        available.insert(extension.extensionName);
    }
    
    // See if required GLFW extensions are present
    printf("\nRequired extensions: \n");
    auto requiredExtensions = get_required_extensions();
    for (const auto &required : requiredExtensions) {
        printf("%s\n", required);
        if (available.find(required) == available.end()) {
            printf("Missing required GLFW extension");
            return -1;
        }
    }   

    create_info.enabledLayerCount = 0;
    if(vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
        return -1;
    }
    return 0;
}