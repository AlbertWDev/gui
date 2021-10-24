#pragma once

#include <gui.h>

typedef struct {
    ui_obj_t* root;
    ui_obj_t* progress_bar;
    ui_obj_t* loading_label;
    ui_obj_t* loading_img;
} splash_view_t;

ui_view_t* new_splash_view();

esp_err_t splash_view_onStart(ui_obj_t** view, void* ctx);

void splash_view_onPause(void* ctx);

void splash_view_onResume(void* ctx);

void splash_view_onDestroy(void* ctx);
