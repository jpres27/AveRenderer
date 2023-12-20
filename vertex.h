#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

typedef struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
} Vertex;

extern std::vector<Vertex> vertices;
extern VkBuffer vertex_buffer;
extern VkDeviceMemory vertex_buffer_memory;

void create_vertex_buffer();
uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);

