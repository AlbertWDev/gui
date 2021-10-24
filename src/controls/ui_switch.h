#pragma once

#include "controls/ui_obj.h"

typedef struct {
    bool checked : 1;
    bool enabled : 1;
} ui_switch_data_t;

ui_obj_t* ui_switch_create(ui_obj_t* parent);

bool ui_switch_handle_input(ui_obj_t* sw, ui_input_type_t type, ui_key_t key);

void ui_switch_set_checked(ui_obj_t* sw, bool checked);
bool ui_switch_is_checked(ui_obj_t* sw);
void ui_switch_set_enabled(ui_obj_t* sw, bool enabled);
bool ui_switch_is_enabled(ui_obj_t* sw);
