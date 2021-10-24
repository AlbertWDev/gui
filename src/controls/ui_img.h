#pragma once

#include "controls/ui_obj.h"

#include <freertos/task.h>
#include "img.h"

typedef struct {
    g_img_t* img;
    TaskHandle_t anim_task;
} ui_img_data_t;

ui_obj_t* ui_img_create(ui_obj_t* parent);

void ui_img_set_src(ui_obj_t* img, const char* filename);
