#include "controls/ui_switch.h"

#include "gui.h"

#include <esp_log.h>
static const char* TAG = "UI_SWITCH";

static void _ui_switch_draw(ui_obj_t* sw, const g_region_t* region);

bool ui_switch_handle_input(ui_obj_t* sw, ui_input_type_t type, ui_key_t key) {
    ui_switch_data_t* sw_data = sw->ext;

    if(type == UI_INPUT_KEY) {
        if(key == UI_KEY_ENTER && sw_data->enabled) {
            //ESP_LOGI(TAG, "KEY - ENTER (consumed event)");
            sw_data->checked = !sw_data->checked;
            ui_obj_dispatch_event(sw, UI_EVENT_VALUE_CHANGED);
            ui_invalidate_region(sw->region);
            return true;
        }
    }

    //ESP_LOGI(TAG, "Event not consumed");
    return false;
}

ui_obj_t* ui_switch_create(ui_obj_t* parent) {
    ui_obj_t* sw = ui_obj_create(parent);
    if(!sw) return NULL;

    ui_switch_data_t* sw_data = malloc(sizeof(ui_switch_data_t));
    if(!sw_data) { ui_obj_delete(sw); return NULL; }
    sw->ext = sw_data;

    sw->focusable = true;
    sw->draw = _ui_switch_draw;
    sw->handle_input = ui_switch_handle_input;
    sw->focusable = true;
    sw_data->checked = false;
    sw_data->enabled = true;
    
    return sw;
}

void ui_switch_set_checked(ui_obj_t* sw, bool checked) {
    ui_switch_data_t* sw_data = sw->ext;
    sw_data->checked = checked;
}

bool ui_switch_is_checked(ui_obj_t* sw) {
    ui_switch_data_t* sw_data = sw->ext;
    return sw_data->checked;
}

void ui_switch_set_enabled(ui_obj_t* sw, bool enabled) {
    ui_switch_data_t* sw_data = sw->ext;
    sw_data->enabled = enabled;
}

bool ui_switch_is_enabled(ui_obj_t* sw) {
    ui_switch_data_t* sw_data = sw->ext;
    return sw_data->enabled;
}

static void _ui_switch_draw(ui_obj_t* sw, const g_region_t* region) {
    ui_switch_data_t* sw_data = sw->ext;

    g_color_t bg_color = sw_data->enabled ? HEX_TO_COLOR(0xFFFF) : HEX_TO_COLOR(0x8410);
    g_color_t fg_color = sw_data->enabled ? (sw_data->checked ? HEX_TO_COLOR(0x07E0) : HEX_TO_COLOR(0xF800)) : HEX_TO_COLOR(0x8410);

    g_coord_t half_h = g_region_height(&sw->region)/2;

    // Draw background as two circles and a rectangle
    g_region_t bg_rect = {
        .x0 = sw->region.x0 + half_h,
        .y0 = sw->region.y0 + half_h/2,
        .x1 = sw->region.x1 - half_h + 1,
        .y1 = sw->region.y1 - half_h/2 + 1
    };
    g_draw_rect(&bg_rect, bg_color, G_FILLED);
    if(sw_data->checked)   // Optimization to avoid two concentric circles overlapping
        g_draw_circle(bg_rect.x0, sw->region.y0 + half_h, half_h/2, bg_color, G_FILLED);
    else
        g_draw_circle(bg_rect.x1, sw->region.y0 + half_h, half_h/2, bg_color, G_FILLED);
    
    // Draw big dot
    g_draw_circle(sw_data->checked ? bg_rect.x1 : bg_rect.x0, sw->region.y0 + half_h, half_h, fg_color, G_FILLED);
}
