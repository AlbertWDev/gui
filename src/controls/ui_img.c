#include "controls/ui_img.h"

#include "gui.h"

#include <esp_log.h>
static const char* TAG = "UI_IMG";

static void _ui_img_draw(ui_obj_t* img, const g_region_t* region);

void _ui_img_updater(void *pvParameter)
{
    /// TODO: null checks
    ui_obj_t* img_obj = (ui_obj_t*)pvParameter;
    ui_img_data_t* img_data = (ui_img_data_t*)img_obj->ext;
    g_img_t* img = img_data->img;

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(200));
        if(img->current_frame < img->header.frame_count)
            g_img_load_next(img);
        else
            g_img_load_first(img);
        
        ui_invalidate_region(img_obj->region);
    }
    vTaskDelete(NULL);
}

ui_obj_t* ui_img_create(ui_obj_t* parent) {
    ui_obj_t* img = ui_obj_create(parent);
    if(!img) return NULL;

    ui_img_data_t* img_data = malloc(sizeof(ui_img_data_t));
    if(!img_data) { ui_obj_delete(img); return NULL; }
    img->ext = img_data;

    img_data->img = NULL;
    img_data->anim_task = NULL;
    img->draw = _ui_img_draw;

    return img;
}

void ui_img_set_src(ui_obj_t* img, const char* filename) {
    ui_img_data_t* img_data = img->ext;
    
    if(img_data->anim_task) {
        vTaskDelete(img_data->anim_task);
        img_data->anim_task = NULL;
    }

    if(img_data->img) {
        g_img_close(img_data->img);
        img_data->img = NULL;
    }

    if(!filename) return;


    img_data->img = g_img_open(filename);
    if(!img_data->img) {
        ESP_LOGE(TAG, "Unable to read image");
        return;
    }

    ui_obj_set_size(img, img_data->img->header.width, img_data->img->header.height);
    
    /// TODO: Store task handler and destroy when object is removed
    xTaskCreate(_ui_img_updater, "img", 2048, img, 5, &img_data->anim_task);
}


static void _ui_img_draw(ui_obj_t* img, const g_region_t* region) {
    ui_img_data_t* img_data = img->ext;
    if(!img_data->img) return;

    g_img_draw(img->region.x0, img->region.y0, img_data->img);
}
