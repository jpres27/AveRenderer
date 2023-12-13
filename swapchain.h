#pragma once

#include "device.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

// max amt of command buffers that can be in submission at once
static int MAX_FRAMES_IN_FLIGHT = 2;

VkFormat swapchain_image_format;
VkExtent2D swapchain_extent;

std::vector<VkFramebuffer> swapchain_framebuffers;
VkRenderPass render_pass;

std::vector<VkImage> depth_images;
std::vector<VkDeviceMemory> depth_image_memorys;
std::vector<VkImageView> depth_image_views;
std::vector<VkImage> swapchain_images;
std::vector<VkImageView> swapchain_image_views;

VkExtent2D window_extent;

VkSwapchainKHR swapchain;

std::vector<VkSemaphore> image_available_semaphores;
std::vector<VkSemaphore> render_finished_semaphores;
std::vector<VkFence> in_flight_fences;
std::vector<VkFence> images_in_flight;
size_t current_frame = 0;

void swapchain_init(VkExtent2D extent);
void swapchain_destroy();

// Use these for refactoring reference but probably get rid of all of them and just directly deal with data
VkFramebuffer getFrameBuffer(int index) { return swapchain_framebuffers[index]; }
VkImageView getImageView(int index) { return swapchain_image_views[index]; }
size_t imageCount() { return swapchain_images.size(); }
uint32_t width() { return swapchain_extent.width; }
uint32_t height() { return swapchain_extent.height; }

float extent_aspect_ratio() {
    return static_cast<float>(swapchain_extent.width) / static_cast<float>(swapchain_extent.height);
}
  
VkFormat find_depth_format();
VkResult acquire_next_image(uint32_t *image_index);
VkResult submit_command_buffers(VkCommandBuffer *buffers, uint32_t *image_index);

void create_swapchain();
void create_image_views();
void create_depth_resources();
void create_render_pass();
void create_framebuffers();
void create_sync_objects();

VkSurfaceFormatKHR choose_swap_surface_format(std::vector<VkSurfaceFormatKHR> &available_formats);
VkPresentModeKHR choose_swap_present_mode(std::vector<VkPresentModeKHR> &available_present_modes);
VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR &capabilities);