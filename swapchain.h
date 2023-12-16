#pragma once

#include "device.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

extern VkExtent2D swapchain_extent;
extern VkRenderPass render_pass;
extern std::vector<VkImage> swapchain_images;
extern std::vector<VkFramebuffer> swapchain_framebuffers;
extern bool framebuffer_resized;

void swapchain_init();
void swapchain_destroy();

float extent_aspect_ratio();
  
VkFormat find_depth_format();
VkResult acquire_next_image(uint32_t *image_index);
VkResult submit_command_buffers(VkCommandBuffer *buffers, uint32_t *image_index);

void create_swapchain();
void create_image_views();
void create_depth_resources();
void create_render_pass();
void create_framebuffers();
void create_sync_objects();
void recreate_swapchain();

VkSurfaceFormatKHR choose_swap_surface_format(std::vector<VkSurfaceFormatKHR> &available_formats);
VkPresentModeKHR choose_swap_present_mode(std::vector<VkPresentModeKHR> &available_present_modes);
VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR &capabilities);