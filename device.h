#pragma once

#include "window.h"

#include <vector>

extern bool enable_validation_layers;
extern VkInstance instance;
extern VkDebugUtilsMessengerEXT debug_messenger;

VkResult create_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);
void destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkAllocationCallbacks *pAllocator);
void has_gflw_required_instance_extensions();
std::vector<const char *> get_required_extensions();
bool check_validation_layer_support();
void setup_debug_messenger();
void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
int create_instance();