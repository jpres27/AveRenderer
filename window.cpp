#include "window.h"
#include "swapchain.h"

void init_window(int width, int height, std::string window_name, TeWindow *te_window) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    te_window->width = width;
    te_window->height = height;
    te_window->window_name = window_name;
    te_window->window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
}

void destroy_window(TeWindow *te_window){
    glfwDestroyWindow(te_window->window);
    glfwTerminate();
}

int create_window_surface(VkInstance instance, TeWindow *te_window, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, te_window->window, nullptr, surface) != VK_SUCCESS) {
        return 1;
    }
    return 0;
}

VkExtent2D getExtent(TeWindow *te_window) { 
    return {static_cast<uint32_t>(te_window->width), static_cast<uint32_t>(te_window->height)}; 
}
