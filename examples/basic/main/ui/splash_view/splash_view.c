#include "ui/splash_view/splash_view.h"

#include <controls/ui_label.h>
#include <controls/ui_bar.h>
#include <controls/ui_img.h>

#include "ui/menu_view/menu_view.h"

#include <esp_log.h>
static const char* TAG = "SPLASH_VIEW";

ui_view_t* new_splash_view() {
    ui_view_t* v = new_view(
        splash_view_onStart,
        splash_view_onPause,
        splash_view_onResume,
        splash_view_onDestroy);
    if(!v) return NULL;
    
    v->ctx = malloc(sizeof(splash_view_t));
    if(!v->ctx) { /* TODO */ free(v); return NULL; }
    memset(v->ctx, 0, sizeof(splash_view_t));
        
    return v;
}

void splashTask(void *pvParameter)
{
    splash_view_t* v = (splash_view_t*)pvParameter;

    for(uint8_t i = 0; i <= 100; i += 10) {
        vTaskDelay(pdMS_TO_TICKS(200));
        ui_bar_set_value(v->progress_bar, i);
        ui_invalidate_region(v->progress_bar->region);
    }

    ui_view_add(new_menu_view());

    vTaskDelete(NULL);
}

esp_err_t splash_view_onStart(ui_obj_t** view, void* ctx) {
    splash_view_t* v = (splash_view_t*)ctx;

    v->root = ui_obj_create(NULL);
    if(v->root == NULL) return ESP_ERR_NO_MEM;
    *view = v->root;

    {
        v->progress_bar = ui_bar_create(v->root);
        if(!v->progress_bar) return ESP_ERR_NO_MEM;
        size_t progress_bar_width = DISP_WIDTH * .6;
        g_coord_t progress_bar_y = DISP_HEIGHT * 0.7;
        ui_obj_set_pos(v->progress_bar, (DISP_WIDTH - progress_bar_width)/2, progress_bar_y);
        ui_obj_set_size(v->progress_bar, progress_bar_width, 5);
        ui_obj_set_focusable(v->progress_bar, false);
        ui_bar_set_editable(v->progress_bar, false);

        v->loading_label = ui_label_create(v->root);
        if(!v->loading_label) return ESP_ERR_NO_MEM;
        /// TODO: Add text measuring (34px is an estimation of "Loading" length)
        ui_obj_set_pos(v->loading_label, (DISP_WIDTH - 34)/2, progress_bar_y - 12);
        ui_label_set_text(v->loading_label, "Loading");

        v->loading_img = ui_img_create(v->root);
        if(!v->loading_img) return ESP_ERR_NO_MEM;
        ui_img_set_src(v->loading_img, "/data/loading.ebg");
        g_coord_t label_y = ui_obj_get_y(v->loading_label);
        /// TODO: Get size of images (24px is the hardcoded known size of loading.ebg, plus 10px margin)
        ui_obj_set_pos(v->loading_img, (DISP_WIDTH - 24)/2, label_y - 34);
    }

    xTaskCreate(&splashTask, "splash_view", 2048, v, 5, NULL);

    return ESP_OK;
}

void splash_view_onPause(void* ctx) {
    splash_view_t* v = (splash_view_t*)ctx;

    if(v->loading_img)
        ui_img_set_src(v->loading_img, NULL);
}

void splash_view_onResume(void* ctx) {}

void splash_view_onDestroy(void* ctx) {
    // TODO
}
