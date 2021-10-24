#pragma once

#include <stddef.h>
#include "controls/ui_obj.h"

ui_obj_t* ui_list_create(ui_obj_t* parent);

ui_obj_t* ui_list_add_item(ui_obj_t* list);
