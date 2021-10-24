#include "controls/ui_bar.h"

#include "gui.h"

#include <esp_log.h>
static const char* TAG = "UI_BAR";

static void _ui_bar_draw(ui_obj_t* bar, const g_region_t* region);

#define MAX(a, b) ( ((a) > (b)) ? (a) : (b) )
#define MIN(a, b) ( ((a) < (b)) ? (a) : (b) )

static bool _ui_bar_handle_input(ui_obj_t* bar, ui_input_type_t type, ui_key_t key) {
    ui_bar_data_t* bar_data = bar->ext;

    if(!bar_data->editable) return false;

    if(type == UI_INPUT_KEY) {
        if(key == UI_KEY_ENTER) {
            //ESP_LOGI(TAG, "KEY - ENTER (consumed event)");
            bar_data->edit_mode = !bar_data->edit_mode;
            ui_invalidate_region(bar->region);
            return true;
        } else if(key == UI_KEY_PREV) {
            if(bar_data->edit_mode) {
                //ESP_LOGI(TAG, "KEY - PREV (consumed event)");

                bar_data->value = MAX(bar_data->value - 1, bar_data->min_value);
                ui_obj_dispatch_event(bar, UI_EVENT_VALUE_CHANGED);
                ui_invalidate_region(bar->region);
                return true;
            }
        } else if(key == UI_KEY_NEXT) {
            if(bar_data->edit_mode) {
                //ESP_LOGI(TAG, "KEY - NEXT (consumed event)");
                
                bar_data->value = MIN(bar_data->value + 1, bar_data->max_value);
                ui_obj_dispatch_event(bar, UI_EVENT_VALUE_CHANGED);
                ui_invalidate_region(bar->region);
                return true;
            }
        }
    }
    
    //ESP_LOGI(TAG, "Event not consumed");
    return false;
}

ui_obj_t* ui_bar_create(ui_obj_t* parent) {
    ui_obj_t* bar = ui_obj_create(parent);
    if(!bar) return NULL;

    ui_bar_data_t* bar_data = malloc(sizeof(ui_bar_data_t));
    if(!bar_data) { ui_obj_delete(bar); return NULL; }
    bar->ext = bar_data;

    bar->focusable = true;
    bar->draw = _ui_bar_draw;
    bar->handle_input = _ui_bar_handle_input;
    bar_data->value = 0;
    bar_data->min_value = 0;
    bar_data->max_value = 100;
    bar_data->editable = false;
    bar_data->edit_mode = false;
    
    return bar;
}

void ui_bar_set_value(ui_obj_t* bar, int16_t value) {
    ui_bar_data_t* bar_data = bar->ext;

    value = MIN(value, bar_data->max_value);
    bar_data->value = MAX(bar_data->min_value, value);
}

void ui_bar_set_range(ui_obj_t* bar, int16_t min, int16_t max) {
    ui_bar_data_t* bar_data = bar->ext;

    bar_data->min_value = min;
    bar_data->max_value = max;

    if(bar_data->value > max)
        bar_data->value = max;
    else if(bar_data->value < min)
        bar_data->value = min;
}

void ui_bar_set_editable(ui_obj_t* bar, bool editable) {
    ui_bar_data_t* bar_data = bar->ext;
    bar_data->editable = editable;
}

static void _ui_bar_draw(ui_obj_t* bar, const g_region_t* region) {
    ui_bar_data_t* bar_data = bar->ext;
    
    g_draw_rect(&bar->region, HEX_TO_COLOR(0x8410), G_FILLED);

    g_region_t foreground = {
        .x0 = bar->region.x0,
        .y0 = bar->region.y0,
        .x1 = bar->region.x0 + (bar_data->value * ui_obj_get_width(bar))/(bar_data->max_value - bar_data->min_value),
        .y1 = bar->region.y1
    };
    g_draw_rect(&foreground, 0xFFFF, G_FILLED);

    if(bar_data->edit_mode) {
        g_draw_rect(&bar->region, HEX_TO_COLOR(0x0000), 1);
    }
}
