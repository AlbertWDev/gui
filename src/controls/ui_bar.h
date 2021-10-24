#pragma once

#include <stddef.h>
#include "controls/ui_obj.h"

typedef struct {
    int16_t value;
    int16_t min_value;
    int16_t max_value;
    bool editable : 1;
    bool edit_mode : 1;
} ui_bar_data_t;

ui_obj_t* ui_bar_create(ui_obj_t* parent);

void ui_bar_set_value(ui_obj_t* bar, int16_t value);

void ui_bar_set_range(ui_obj_t* bar, int16_t min, int16_t max);

void ui_bar_set_editable(ui_obj_t* bar, bool editable);
