#include "render.h"
#include "vertex.h"
#include <cstdio>
#include <array>
#include <limits>

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;
size_t current_frame = 0;
bool framebuffer_resized = false;


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
        printf("Failed to create pipeline layout!\n");
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
        printf("Failed to allocate command buffer!\n");
    }
}

void record_command_buffer(VkCommandBuffer command_buffer, uint32_t image_index) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
            printf("Failed to begin recording command buffer!\n");
        }

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = render_pass;
        render_pass_info.framebuffer = swapchain_framebuffers[image_index];
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = swapchain_extent;

        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {0.1f, 0.1f, 0.1f, 0.1f};
        clear_values[1].depthStencil = {1.0f, 0};

        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

        // Dynamically setting viewport and scissor
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchain_extent.width);
        viewport.height = static_cast<float>(swapchain_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchain_extent;
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);

        VkBuffer vertex_buffers[] = {vertex_buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

        vkCmdDraw(command_buffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

        vkCmdEndRenderPass(command_buffer);
        if(vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            printf("Failed to record command buffer!\n");
        }
}

void draw_frame() {
    // CPU will block here after 2 command buffers have been submitted until it executes one
    vkWaitForFences(device, 1, &in_flight_fences[current_frame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(),
                                            image_available_semaphores[current_frame],  // must be a not signaled semaphore
                                            VK_NULL_HANDLE, &image_index);
    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain();
        return;
    } else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        printf("Failed to acquire swap chain image!\n");
    }

    vkResetFences(device, 1, &in_flight_fences[current_frame]);

    vkResetCommandBuffer(command_buffers[current_frame], /*VkCommandBufferResetFlagBits*/ 0);
    record_command_buffer(command_buffers[current_frame], image_index);

    images_in_flight[image_index] = in_flight_fences[current_frame];

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {image_available_semaphores[current_frame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffers[current_frame];

    VkSemaphore signal_semaphores[] = {render_finished_semaphores[current_frame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, in_flight_fences[current_frame]) !=
        VK_SUCCESS) {
        printf("Failed to submit draw command buffer!\n");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = {swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;

    presentInfo.pImageIndices = &image_index;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized) {
        framebuffer_resized = false;
        recreate_swapchain();
    } else if(result != VK_SUCCESS) printf("Failed to present swap chain image!\n");

    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
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
    printf("Window created.\n");
    if(create_instance() > 0) printf("Failed to create instance!\n");
    printf("Instance created.\n");
    setup_debug_messenger();
    printf("Debug messenger setup.\n");
    create_window_surface(instance, &te_window, &surface);
    printf("Window surface created.\n");
    if(pick_physical_device() > 0) printf("Failed to pick physical device!\n");
    printf("Physical device picked.\n");
    create_logical_device();
    printf("Logical device created.\n");
    create_swapchain();
    printf("Swapchain created.\n");
    create_image_views();
    printf("Image views created.\n");
    create_render_pass();
    printf("Render pass created.\n");
    create_pipeline_layout();
    printf("Pipeline layout created.\n");
    create_pipeline();
    printf("Pipeline created.\n");
    create_framebuffers();
    printf("Framebuffers created.\n");
    create_command_pool();
    printf("Command pool created.\n");
    create_vertex_buffer();
    printf("Vertex buffer created.\n");
    create_command_buffers();
    printf("Command buffers created.\n");
    create_sync_objects();
    printf("Sync objects created.\nEntering render loop.\n");
    render_loop(&te_window);
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