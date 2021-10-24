#pragma once

#include <gui.h>

typedef struct {
    
} menu_view_t;

ui_view_t* new_menu_view();

esp_err_t menu_view_onStart(ui_obj_t** view, void* ctx);

void menu_view_onPause(void* ctx);

void menu_view_onResume(void* ctx);

void menu_view_onDestroy(void* ctx);
