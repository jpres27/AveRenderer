#include "swapchain.h"
#include "window.h"
#include "vertex.h"
#include <limits>
#include <array>
#include <cstdio>

// max amt of command buffers that can be in submission at once
int MAX_FRAMES_IN_FLIGHT = 2;

VkFormat swapchain_image_format;
VkExtent2D swapchain_extent;

std::vector<VkFramebuffer> swapchain_framebuffers;
VkRenderPass render_pass;

std::vector<VkImage> swapchain_images;
std::vector<VkImageView> swapchain_image_views;

VkExtent2D window_extent;

VkSwapchainKHR swapchain;

std::vector<VkSemaphore> image_available_semaphores;
std::vector<VkSemaphore> render_finished_semaphores;
std::vector<VkFence> in_flight_fences;
std::vector<VkFence> images_in_flight;

void swapchain_destroy() {
  swapchain_cleanup();

  vkDestroyRenderPass(device, render_pass, nullptr);

  vkDestroyBuffer(device, vertex_buffer, nullptr);
  vkFreeMemory(device, vertex_buffer_memory, nullptr);

  // cleanup synchronization objects
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device, render_finished_semaphores[i], nullptr);
    vkDestroySemaphore(device, image_available_semaphores[i], nullptr);
    vkDestroyFence(device, in_flight_fences[i], nullptr);
  }
}

void swapchain_cleanup() {
  for (auto framebuffer : swapchain_framebuffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  for (auto image_view : swapchain_image_views) {
      vkDestroyImageView(device, image_view, nullptr);
  }
  swapchain_image_views.clear();

  if (swapchain != nullptr) {
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    swapchain = nullptr;
  }
}

void recreate_swapchain() {
  int width = 0, height = 0;
  glfwGetFramebufferSize(te_window.window, &width, &height);
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(te_window.window, &width, &height);
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(device);

  swapchain_cleanup();

  create_swapchain();
  create_image_views();
  create_framebuffers();
}

float extent_aspect_ratio() {
    return static_cast<float>(swapchain_extent.width) / static_cast<float>(swapchain_extent.height);
}

void create_swapchain() {
  swapchain_support_details swapchain_support = query_swapchain_support(physicalDevice);

  VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapchain_support.formats);
  VkPresentModeKHR presentMode = choose_swap_present_mode(swapchain_support.presentModes);
  VkExtent2D extent = choose_swap_extent(swapchain_support.capabilities);

  uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
  if (swapchain_support.capabilities.maxImageCount > 0 &&
      image_count > swapchain_support.capabilities.maxImageCount) {
    image_count = swapchain_support.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;

  createInfo.minImageCount = image_count;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  queue_family_indices indices = find_queue_families(physicalDevice);
  uint32_t queue_family_indices_array[] = {indices.graphicsFamily, indices.presentFamily};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queue_family_indices_array;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;      // Optional
    createInfo.pQueueFamilyIndices = nullptr;  // Optional
  }

  createInfo.preTransform = swapchain_support.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
    printf("failed to create swap chain!");
  }

  // we only specified a minimum number of images in the swap chain, so the implementation is
  // allowed to create a swap chain with more. That's why we'll first query the final number of
  // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
  // retrieve the handles.
  vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);
  swapchain_images.resize(image_count);
  vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());

  swapchain_image_format = surfaceFormat.format;
  swapchain_extent = extent;
}

void create_image_views() {
  swapchain_image_views.resize(swapchain_images.size());
  for (size_t i = 0; i < swapchain_images.size(); i++) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = swapchain_images[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = swapchain_image_format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &viewInfo, nullptr, &swapchain_image_views[i]) != VK_SUCCESS) {
      printf("failed to create texture image view!");
    }
  }
}

void create_render_pass() {
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = swapchain_image_format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.srcAccessMask = 0;
  dependency.srcStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstSubpass = 0;
  dependency.dstStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask =
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &render_pass) != VK_SUCCESS) {
    printf("failed to create render pass!");
  }
}

void create_framebuffers() {
  swapchain_framebuffers.resize(swapchain_images.size());
  for (size_t i = 0; i < swapchain_images.size(); i++) {
    VkImageView attachments[] = {swapchain_image_views[i]};

    VkExtent2D swapChainExtent = swapchain_extent;
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = render_pass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = swapChainExtent.width;
    framebufferInfo.height = swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchain_framebuffers[i]) != VK_SUCCESS) {
      printf("failed to create framebuffer!");
    }
  }
}

void create_sync_objects() {
  image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
  images_in_flight.resize(swapchain_images.size(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &in_flight_fences[i]) != VK_SUCCESS) {
      printf("failed to create one or more sync objects");
    }
  }
}

VkSurfaceFormatKHR choose_swap_surface_format(
    std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR choose_swap_present_mode(std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      printf("Present mode: Mailbox\n");
      return availablePresentMode;
    }
  }

  // for (auto &availablePresentMode : availablePresentModes) {
  //   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
  //     std::cout << "Present mode: Immediate" << std::endl;
  //     return availablePresentMode;
  //   }
  // }

  printf("Present mode: V-Sync\n");
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actual_extent = window_extent;
    actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));
    return actual_extent;
  }
}