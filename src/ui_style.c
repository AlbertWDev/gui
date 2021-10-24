#include "ui_style.h"

#include "fonts/base.h"


ui_style_t* ui_style_create() {
    ui_style_t* style = malloc(sizeof(ui_style_t));
    style->items = NULL;
    style->prop_count = 0;
    return style;
}

ui_style_prop_t lv_style_register_property() {
    return -1;
}

void ui_style_set_property(ui_style_t* style, ui_style_prop_t property, ui_style_value_t value) {
    if(style->prop_count > 0) {
        for(uint8_t i = 0; i < style->prop_count; i++) {
            if(style->items[i].property == property) {
                // Property found in style, just edit its value
                style->items[i].value = value;
                return;
            }
        }

        // Add property to style
        style->items = realloc(style->items, (style->prop_count + 1) * sizeof(ui_style_item_t));
        // TODO: check non null
        style->items[style->prop_count].property = property;
        style->items[style->prop_count].value = value; // TODO: Check if this is valid with unions
        style->prop_count++;
    }
    else {
        // First property of the style
        style->items = malloc(sizeof(ui_style_item_t));
        // TODO: check non null
        style->items[0].property = property;
        style->items[0].value = value; // TODO: Check if this is valid with unions
        style->prop_count++;
    }
}

esp_err_t ui_style_get_property(ui_style_t* style, ui_style_prop_t property, ui_style_value_t* value) {
    if(style->prop_count == 0) return ESP_ERR_NOT_FOUND;

    for(uint8_t i = 0; i < style->prop_count; i++) {
        if(style->items[i].property == property) {
            // Property found in style, just edit its value
            *value = style->items[i].value;
            return ESP_OK;
        }
    }

    return ESP_ERR_NOT_FOUND;
}

ui_style_value_t ui_style_get_default(ui_style_prop_t property)
{
    ui_style_value_t value;
    switch(property){
        case UI_STYLE_X:
        case UI_STYLE_Y:
            value.num = 0;
            break;
        case UI_STYLE_WIDTH:
        case UI_STYLE_HEIGHT:
            value.num = 10;
            break;

        case UI_STYLE_BACKGROUND_COLOR:
            value.color = HEX_TO_COLOR(0x0000);
            break;

        case UI_STYLE_TEXT_COLOR:
            value.color = HEX_TO_COLOR(0xFFFF);
            break;
        case UI_STYLE_TEXT_FONT:
            value.ptr = &base_font;
            break;

        default:
            value.num = 0;
            value.ptr = NULL;
            break;
    }
    return value;
}
