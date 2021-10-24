#pragma once

#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <display_driver.h>
#include <graphics.h>

#include "view.h"

#include <esp_log.h>

esp_err_t ui_init();
esp_err_t ui_view_add(ui_view_t* view);
esp_err_t ui_view_remove();
void* ui_view_get_current_context();

typedef enum {
    UI_LAYER_NONE = 0,
    UI_LAYER_OVERLAY,
    UI_LAYER_VIEW,
    UI_LAYER_MAX
} ui_layer_type_t;

typedef struct {
    ui_layer_type_t type;
    union {
        ui_view_t* view;
        ui_overlay_t* overlay;
    };
} ui_layer_t;

ui_layer_t* ui_layer_sys();
ui_layer_t* ui_layer_top();
ui_layer_t* ui_layer_user();

ui_obj_t* ui_layer_get_root(ui_layer_t* layer);

void ui_invalidate();
void ui_invalidate_region(g_region_t region);
//void ui_handle_input(input_source_t source, input_action_t action, void* data);
void ui_input_handler(ui_input_type_t type, ui_key_t key);

void ui_handle_deleted_obj(ui_obj_t* obj);

esp_err_t ui_set_focus(ui_layer_t* layer);
void ui_refocus();
