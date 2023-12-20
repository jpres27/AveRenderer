#include "render.h"

#include <cstdio>
#include <array>

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

struct TeWindow te_window;

std::string VERTPATH = "shaders/test_shader.vert.spv";
std::string FRAGPATH = "shaders/test_shader.frag.spv";

VkPipelineLayout pipeline_layout;
std::vector<VkCommandBuffer> command_buffers;

void create_pipeline_layout() {
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    if(vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
        printf("Failed to create pipeline layout");
    }
}

void create_pipeline() {
    auto pipeline_config = default_pipeline_config(swapchain_extent.width, swapchain_extent.height);
    pipeline_config.render_pass = render_pass;
    pipeline_config.pipeline_layout = pipeline_layout;
    create_graphics_pipeline(VERTPATH, FRAGPATH, pipeline_config);
}

void create_command_buffers() {
    command_buffers.resize(swapchain_images.size());
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = commandPool;
    alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());
    if(vkAllocateCommandBuffers(device, &alloc_info, command_buffers.data()) != VK_SUCCESS) {
        printf("Failed to allocate command buffer");
    }

    for(int i = 0; i < command_buffers.size(); i++) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(command_buffers[i], &begin_info) != VK_SUCCESS) {
            printf("Failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = render_pass;
        render_pass_info.framebuffer = swapchain_framebuffers[i];
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = swapchain_extent;

        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {0.1f, 0.1f, 0.1f, 0.1f};
        clear_values[1].depthStencil = {1.0f, 0};

        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

        // Dynamically setting viewport and scissor
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchain_extent.width);
        viewport.height = static_cast<float>(swapchain_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffers[i], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchain_extent;
        vkCmdSetScissor(command_buffers[i], 0, 1, &scissor);

        vkCmdDraw(command_buffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(command_buffers[i]);
        if(vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS) {
            printf("Failed to record command buffer");
        }
    }

}

void draw_frame() {
    uint32_t image_index;
    auto result = acquire_next_image(&image_index);
    if(result != VK_SUCCESS) {
        printf("Failed to acquire swap chain image");
    }
    result = submit_command_buffers(&command_buffers[image_index], &image_index);
        if(result != VK_SUCCESS) printf("Failed to present swap chain image");
}

void render_loop(TeWindow *te_window) {
    while(!glfwWindowShouldClose(te_window->window)) {
        glfwPollEvents();
        draw_frame();
    }
    vkDeviceWaitIdle(device);
}

void render() {
    init_window(WIDTH, HEIGHT, "Ave!", &te_window);
    if(create_instance() > 0) printf("Failed to create instance \n");
    setup_debug_messenger();
    create_window_surface(instance, &te_window, &surface);
    if(pick_physical_device() > 0) printf("Failed to pick physical device\n");
    create_logical_device();
    create_command_pool();
    swapchain_init();
    create_pipeline_layout();
    create_pipeline();
    create_command_buffers();
    render_loop(&te_window);
    vkDestroyShaderModule(device, vert_shader_module, nullptr);
    vkDestroyShaderModule(device, frag_shader_module, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyPipeline(device, graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    swapchain_destroy();
    if (enable_validation_layers) destroy_debug_utils_messenger_EXT(instance, debugMessenger, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    destroy_window(&te_window);
    glfwTerminate();
}