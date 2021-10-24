#pragma once

#include "controls/ui_obj.h"

typedef struct {
    char* text;
    //g_color_t fg_color;
} ui_label_data_t;

ui_obj_t* ui_label_create(ui_obj_t* parent);

void ui_label_set_text(ui_obj_t* label, const char* text);
