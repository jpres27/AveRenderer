#pragma once

#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"

extern uint32_t WIDTH;
extern uint32_t HEIGHT;

void create_pipeline_layout();
void create_pipeline();
void create_command_buffers();
void draw_frame();
void render_loop(TeWindow *te_window);
void render();