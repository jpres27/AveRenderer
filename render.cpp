#include "render.h"

struct TeWindow te_window;

void render_loop(TeWindow *te_window) {
    while(!glfwWindowShouldClose(te_window->window)) {
        glfwPollEvents();
    }
}

void render() {
    init_window(HEIGHT, WIDTH, "Ave!", &te_window);
    //initVulkan()
    render_loop(&te_window);
    destroy_window(&te_window);
}