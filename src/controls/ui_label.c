#include "controls/ui_label.h"

static void _ui_label_draw(ui_obj_t* label, const g_region_t* region);


ui_obj_t* ui_label_create(ui_obj_t* parent) {
    ui_obj_t* label = ui_obj_create(parent);
    if(!label) return NULL;

    ui_label_data_t* label_data = malloc(sizeof(ui_label_data_t));
    if(!label_data) { ui_obj_delete(label); return NULL; }
    label->ext = label_data;
    
    label->draw = _ui_label_draw;
    label_data->text = NULL;
    //label_data->fg_color = 0xFFFF;
    
    return label;
}

void ui_label_set_text(ui_obj_t* label, const char* text) {
    ui_label_data_t* label_data = label->ext;

    free(label_data->text);
    label_data->text = NULL;

    if(text)
        label_data->text = strdup(text);
    ui_obj_dispatch_event(label, UI_EVENT_VALUE_CHANGED);
}

static void _ui_label_draw(ui_obj_t* label, const g_region_t* region) {
    ui_label_data_t* label_data = label->ext;

    if(label_data->text) {
        g_color_t text_color = ui_obj_get_style_text_color(label);
        g_draw_string(label->region.x0, label->region.y0, label_data->text, text_color);
    }
}
