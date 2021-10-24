#pragma once

/// TODO: Object parts and each part with a style?
/// TODO: Object states or style properties associated to states?


#include <graphics.h>

#include <esp_err.h>

typedef enum {
    UI_STYLE_X,
    UI_STYLE_Y,
    UI_STYLE_WIDTH,
    UI_STYLE_HEIGHT,

    UI_STYLE_BACKGROUND_COLOR,

    UI_STYLE_TEXT_COLOR,
    UI_STYLE_TEXT_FONT,
} ui_style_prop_t;

typedef union {
    int32_t num;
    const void * ptr; /* For fonts, images or other objects */
    g_color_t color;
} ui_style_value_t;

typedef struct {
	ui_style_prop_t property;
	ui_style_value_t value;
} ui_style_item_t;

typedef struct {
	ui_style_item_t* items;
	uint8_t prop_count;
} ui_style_t;

ui_style_t* ui_style_create();
ui_style_prop_t lv_style_register_property();

void ui_style_set_property(ui_style_t* style, ui_style_prop_t property, ui_style_value_t value);
esp_err_t ui_style_get_property(ui_style_t* style, ui_style_prop_t property, ui_style_value_t* value);
ui_style_value_t ui_style_get_default(ui_style_prop_t property);

