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
    if(create_instance() > 0) {
        printf("Failed to create instance \n");
    }
    setup_debug_messenger();
    render_loop(&te_window);
    if (enable_validation_layers) {
        destroy_debug_utils_messenger_EXT(instance, debug_messenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
    destroy_window(&te_window);
}