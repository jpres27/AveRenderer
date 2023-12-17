#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

struct TeWindow {
    int width;
    int height;
    std::string window_name;
    GLFWwindow *window;
};

extern struct TeWindow te_window;

void init_window(int height, int width, std::string window_name, TeWindow *te_window);
void framebuffer_resize_callback(GLFWwindow *window, int width, int height);
void destroy_window(TeWindow *te_window);
int create_window_surface(VkInstance instance, TeWindow *window, VkSurfaceKHR *surface);
VkExtent2D getExtent(TeWindow *te_window);