#include "vertex.h"

#include <vector>
#include <array>

VkVertexInputBindingDescription Vertex::binding_desc_gen() {
    VkVertexInputBindingDescription binding_desc{};
    binding_desc.binding = 0;
    binding_desc.stride = sizeof(Vertex);                    // Number of bytes from one entry to the next
    binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;    // Moving to the next data entry after each vertex

    return binding_desc;
    }

std::array<VkVertexInputAttributeDescription, 2> Vertex::attribute_desc_gen() {
    static std::array<VkVertexInputAttributeDescription, 2> attribute_desc{};

    attribute_desc[0].binding = 0;
    attribute_desc[0].location = 0;
    attribute_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_desc[0].offset = offsetof(Vertex, pos); 

    attribute_desc[1].binding = 0;
    attribute_desc[1].location = 1;
    attribute_desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_desc[1].offset = offsetof(Vertex, color);

    return attribute_desc;
}

std::vector<Vertex> vertices = {
    {{0.0f, -0.5f,}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};