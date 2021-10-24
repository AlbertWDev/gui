#include "controls/ui_list.h"

#include "gui.h"

#include <esp_log.h>
static const char* TAG = "UI_LIST";

static void _ui_list_draw(ui_obj_t* list, const g_region_t* region);


ui_obj_t* ui_list_create(ui_obj_t* parent) {
    ui_obj_t* list = ui_obj_create(parent);
    if(!list) return NULL;

    list->draw = _ui_list_draw;

    return list;
}

ui_obj_t* ui_list_add_item(ui_obj_t* list) {
    ui_obj_t* last_item = _ui_llist_get_last(&list->children);

    ui_obj_t* item = ui_obj_create(list);
    if(!item) return NULL;

    item->focusable = true;
    
    if(last_item)
        ui_obj_set_pos(item, 0, last_item->region.y1 - list->region.y0);
    
    ui_obj_set_width(item, g_region_width(&list->region));

    return item;
}

static void _ui_list_draw(ui_obj_t* list, const g_region_t* region) {
    //ui_list_data_t* list_data = list->ext;
    //ui_cont_data_t* cont_data = list->ext;

    g_draw_rect(&list->region, HEX_TO_COLOR(0x1084), G_FILLED);

    /*if(cont_data->selected_obj) {
        g_draw_rect(&cont_data->selected_obj->region, HEX_TO_COLOR(0xD6BA), G_FILLED);
    }*/
    
    /*if(list_data->selected_obj) {
        ESP_LOGI(TAG, "Draw - An object is selected at Y=%d", list_data->selected_obj->region.x0);
        g_draw_rect(&list_data->selected_obj->region, 0xFFFF, 5);
    }*/
}
