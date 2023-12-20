#include "vertex.h"
#include "device.h"
#include <string.h>
#include <vector>
#include <cstdio>

std::vector<Vertex> vertices = {
    {{0.0f, -0.5f,}, {1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

// Buffer is destroyed and memory is freed within swapchain_destroy()
VkBuffer vertex_buffer;
VkDeviceMemory vertex_buffer_memory;

void create_vertex_buffer() {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = sizeof(vertices[0]) * vertices.size();
    buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &buffer_info, nullptr, &vertex_buffer) != VK_SUCCESS) {
        printf("Failed to create vertex buffer!");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device, vertex_buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, 
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    if (vkAllocateMemory(device, &alloc_info, nullptr, &vertex_buffer_memory) != VK_SUCCESS) {
        printf("Failed to allocate vertex buffer memory!\n");
    }

    vkBindBufferMemory(device, vertex_buffer, vertex_buffer_memory, 0);

    void* data;
    vkMapMemory(device, vertex_buffer_memory, 0, buffer_info.size, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(buffer_info.size));
    vkUnmapMemory(device, vertex_buffer_memory);
}

// TODO: come back and fully understand what is happening with the bit shifting
uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    printf("Failed to find suitable memory type!\n");
    return -1;
}