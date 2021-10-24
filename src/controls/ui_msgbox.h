#pragma once

#include "gui.h"

ui_obj_t* ui_msgbox_create(ui_layer_t* layer, const char* title, const char* content, bool modal);

void ui_msgbox_close(ui_obj_t* msg_box);
