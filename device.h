#pragma once

#include "window.h"

#include <vector>

struct swapchain_support_details {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct queue_family_indices {
  uint32_t graphicsFamily;
  uint32_t presentFamily;
  bool graphicsFamilyHasValue = false;
  bool presentFamilyHasValue = false;
  bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

extern bool enableValidationLayers;
extern VkInstance instance;
extern VkSurfaceKHR surface;
extern VkDebugUtilsMessengerEXT debugMessenger;
extern VkDevice device;
extern VkCommandPool commandPool;

VkResult create_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);
void destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkAllocationCallbacks *pAllocator);
void has_gflw_required_instance_extensions();
std::vector<const char *> get_required_extensions();
bool check_validation_layer_support();
void setup_debug_messenger();
void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
int create_instance();
int pick_physical_device();
bool is_device_suitable(VkPhysicalDevice device);
queue_family_indices find_queue_families(VkPhysicalDevice device);
bool check_device_extension_support(VkPhysicalDevice device);
swapchain_support_details query_swapchain_support(VkPhysicalDevice device);
void create_logical_device();
void create_command_pool();