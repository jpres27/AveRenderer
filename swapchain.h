#pragma once

#include "device.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

extern VkExtent2D swapchain_extent;
extern VkRenderPass render_pass;
extern std::vector<VkImage> swapchain_images;
extern std::vector<VkFramebuffer> swapchain_framebuffers;
extern std::vector<VkFence> in_flight_fences;
extern std::vector<VkFence> images_in_flight;
extern VkSwapchainKHR swapchain;
extern std::vector<VkSemaphore> image_available_semaphores;
extern std::vector<VkSemaphore> render_finished_semaphores;
extern int MAX_FRAMES_IN_FLIGHT;

void swapchain_destroy();
void swapchain_cleanup();
void recreate_swapchain();

float extent_aspect_ratio();
  
void create_swapchain();
void create_image_views();
void create_render_pass();
void create_framebuffers();
void create_sync_objects();

VkSurfaceFormatKHR choose_swap_surface_format(std::vector<VkSurfaceFormatKHR> &available_formats);
VkPresentModeKHR choose_swap_present_mode(std::vector<VkPresentModeKHR> &available_present_modes);
VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR &capabilities);