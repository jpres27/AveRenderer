#include "render.h"

#include <cstdio>

struct TeWindow te_window;

void render_loop(TeWindow *te_window) {
    while(!glfwWindowShouldClose(te_window->window)) {
        glfwPollEvents();
    }
}

void render() {
    init_window(HEIGHT, WIDTH, "Ave!", &te_window);
    if(create_instance() < 0) {
        printf("Failed to create instance \n");
    }
    render_loop(&te_window);
    destroy_window(&te_window);
}