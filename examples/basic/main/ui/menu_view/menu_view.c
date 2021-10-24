#include "ui/menu_view/menu_view.h"

#include <controls/ui_list.h>
#include <controls/ui_label.h>
#include <controls/ui_bar.h>
#include <controls/ui_switch.h>
#include <controls/ui_msgbox.h>

#include <esp_log.h>
static const char* TAG = "MENU_VIEW";

ui_view_t* new_menu_view() {
    ui_view_t* v = new_view(
        menu_view_onStart,
        menu_view_onPause,
        menu_view_onResume,
        menu_view_onDestroy);
    if(!v) return NULL;
    
    /*v->ctx = malloc(sizeof(menu_view_t));
    if(!v->ctx) { / * TODO * / free(v); return NULL; }
    memset(v->ctx, 0, sizeof(menu_view_t));*/
    
    return v;
}

static void _sw_handler(ui_obj_t* sw, ui_event_t type) {
    if(type == UI_EVENT_VALUE_CHANGED) {
        ui_switch_data_t* sw_data = sw->ext;
        ESP_LOGI(TAG, "Switch changed: %s", sw_data->checked ? "enabled" : "disabled");
    }
}

static void _msgbox_btn_handler(ui_obj_t* btn, ui_event_t type) {
    if(type == UI_EVENT_CLICKED) {
        ESP_LOGI(TAG, "MsgBox button clicked");
        ui_msgbox_create(ui_layer_top(), "Title", "Content", true);
    }
}



esp_err_t menu_view_onStart(ui_obj_t** view, void* ctx) {
    //menu_view_t* v = (menu_view_t*)ctx;

    {
        ui_obj_t* list = ui_list_create(NULL);
        if(!list) return ESP_ERR_NO_MEM;
        *view = list;

        {
            ui_obj_t* header = ui_list_add_item(list);
            if(!header) return ESP_ERR_NO_MEM;
            ui_obj_set_height(header, 14);
            ui_obj_set_focusable(header, false);

            ui_obj_t* header_label = ui_label_create(header);
            if(!header_label) return ESP_ERR_NO_MEM;
            ui_label_set_text(header_label, "Title");
        }

        {
            ui_obj_t* item1 = ui_list_add_item(list);
            if(!item1) return ESP_ERR_NO_MEM;
            ui_obj_set_height(item1, 20);

            ui_obj_t* item1_label = ui_label_create(item1);
            if(!item1_label) return ESP_ERR_NO_MEM;
            ui_label_set_text(item1_label, "Item 1");

            ui_obj_t* item1_bar = ui_bar_create(item1);
            if(!item1_bar) return ESP_ERR_NO_MEM;
            ui_obj_set_pos(item1_bar, 5, 14);
            ui_obj_set_size(item1_bar, DISP_WIDTH - 10, 4);
            ui_obj_set_focusable(item1_bar, false);
            ui_bar_set_value(item1_bar, 20);
            ui_bar_set_editable(item1_bar, true);

            item1->input_override = item1_bar;
        }

        {
            ui_obj_t* item2 = ui_list_add_item(list);
            if(!item2) return ESP_ERR_NO_MEM;
            ui_obj_set_height(item2, 30);
            ui_obj_set_focusable(item2, false);

            ui_obj_t* item2_title = ui_label_create(item2);
            if(!item2_title) return ESP_ERR_NO_MEM;
            ui_label_set_text(item2_title, "Item 2");

            ui_obj_t* item2_content = ui_label_create(item2);
            if(!item2_content) return ESP_ERR_NO_MEM;
            ui_obj_set_pos(item2_content, 0, 15);
            //ui_obj_set_focusable(item2_content, true);
            ui_label_set_text(item2_content, "Content");
        }

        {
            ui_obj_t* separator = ui_list_add_item(list);
            if(!separator) return ESP_ERR_NO_MEM;
            ui_obj_set_height(separator, 2);
            ui_obj_set_focusable(separator, false);

            ui_obj_t* separator_bar = ui_bar_create(separator);
            if(!separator_bar) return ESP_ERR_NO_MEM;
            ui_obj_set_focusable(separator_bar, false);
            ui_bar_set_value(separator_bar, 100);
        }

        {
            ui_obj_t* item3 = ui_list_add_item(list);
            if(!item3) return ESP_ERR_NO_MEM;
            ui_obj_set_height(item3, 12);

            ui_obj_t* item3_label = ui_label_create(item3);
            if(!item3_label) return ESP_ERR_NO_MEM;
            ui_label_set_text(item3_label, "Item 3");

            ui_obj_t* item3_switch = ui_switch_create(item3);
            if(!item3_switch) return ESP_ERR_NO_MEM;
            ui_switch_set_checked(item3_switch, true);
            ui_obj_set_size(item3_switch, 16, 8);
            ui_obj_set_pos(item3_switch, ui_obj_get_width(item3) - 17, ui_obj_get_height(item3)/2 - 5);
            ui_obj_set_event_handler(item3_switch, _sw_handler);
            ui_obj_set_focusable(item3_switch, false);

            item3->input_override = item3_switch;
        }

        {
            ui_obj_t* item4_btn = ui_list_add_item(list);
            if(!item4_btn) return ESP_ERR_NO_MEM;
            ui_obj_set_height(item4_btn, 12);
            ui_obj_set_focusable(item4_btn, true);
            ui_obj_set_event_handler(item4_btn, _msgbox_btn_handler);

            ui_obj_t* item4_label = ui_label_create(item4_btn);
            if(!item4_label) return ESP_ERR_NO_MEM;
            ui_label_set_text(item4_label, "Test MsgBox");
        }

        {
            ui_style_t* light_style = ui_style_create();
            ui_style_value_t bg_color = { .color = HEX_TO_COLOR(0xCE59) };   // Light gray background
            ui_style_set_property(light_style, UI_STYLE_BACKGROUND_COLOR, bg_color);
            ui_style_value_t text_color = { .color = HEX_TO_COLOR(0x0000) };   // Black text
            ui_style_set_property(light_style, UI_STYLE_TEXT_COLOR, text_color);

            ui_style_t* red_style = ui_style_create();
            bg_color.color = HEX_TO_COLOR(0xF800);   // Red background
            ui_style_set_property(red_style, UI_STYLE_BACKGROUND_COLOR, bg_color);

            ui_obj_t* item5_btn = ui_list_add_item(list);
            if(!item5_btn) return ESP_ERR_NO_MEM;
            ui_obj_set_height(item5_btn, 24);
            item5_btn->style = light_style;

            ui_obj_t* item5_label = ui_label_create(item5_btn);
            if(!item5_label) return ESP_ERR_NO_MEM;
            ui_label_set_text(item5_label, "Gray background, red below");

            ui_obj_t* item5_div = ui_obj_create(item5_btn);
            if(!item5_div) return ESP_ERR_NO_MEM;
            ui_obj_set_size(item5_div, 30, 10);
            ui_obj_set_pos(item5_div, 10, 12);
            item5_div->style = red_style;
        }
    }

    return ESP_OK;
}

void menu_view_onPause(void* ctx) {}

void menu_view_onResume(void* ctx) {}

void menu_view_onDestroy(void* ctx) {
    // TODO
}
