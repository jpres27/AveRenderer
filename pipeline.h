#pragma once

#include <string>
#include <vector>
#include "device.h"

struct pipeline_config_info {
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
    VkPipelineRasterizationStateCreateInfo rasterization_info;
    VkPipelineMultisampleStateCreateInfo multisample_info;
    VkPipelineColorBlendAttachmentState color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo color_blend_info;
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
    VkPipelineLayout pipeline_layout = nullptr;
    VkRenderPass render_pass = nullptr;
    uint32_t subpass = 0;
};

    // TePipeline(TeDevice &device, const std::string& vert_filepath, const std::string& frag_filepath, const pipeline_config_info pipe_ci);


    static std::vector<char> read_file(std::string& filepath);
    void bind(VkCommandBuffer command_buffer);
    static pipeline_config_info default_pipeline_config(uint32_t width, uint32_t height);
    void create_graphics_pipeline(std::string& vert_filepath, std::string& frag_filepath, pipeline_config_info pipe_ci);
    void create_shader_module(std::vector<char>& shader_program, VkShaderModule* shader_module);

    VkPipeline graphics_pipeline;
    VkShaderModule vert_shader_module;
    VkShaderModule frag_shader_module;