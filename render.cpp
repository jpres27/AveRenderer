#include "render.h"

#include <cstdio>

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

struct TeWindow te_window;

void render_loop(TeWindow *te_window) {
    while(!glfwWindowShouldClose(te_window->window)) {
        glfwPollEvents();
    }
}

void render() {
    init_window(WIDTH, HEIGHT, "Ave!", &te_window);
    if(create_instance() > 0) printf("Failed to create instance \n");
    setup_debug_messenger();
    create_window_surface(instance, &te_window, &surface);
    if(pick_physical_device() > 0) printf("Failed to pick physical device");
    create_logical_device();
    render_loop(&te_window);
    if (enable_validation_layers) destroy_debug_utils_messenger_EXT(instance, debugMessenger, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    destroy_window(&te_window);
}