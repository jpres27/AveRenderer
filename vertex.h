#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

typedef struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
    VkVertexInputBindingDescription binding_desc_gen();
    std::array<VkVertexInputAttributeDescription, 2> attribute_desc_gen();
} Vertex;

extern std::vector<Vertex> vertices;

