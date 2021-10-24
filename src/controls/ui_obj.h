#pragma once

#include <graphics.h>

#include "utils/linked_list.h"

#include "ui_style.h"

typedef enum {
    UI_INPUT_KEY,
    UI_INPUT_TOUCH,
} ui_input_type_t;

typedef enum {
    UI_KEY_NONE,
    UI_KEY_UP,
    UI_KEY_DOWN,
    UI_KEY_RIGHT,
    UI_KEY_LEFT,
    UI_KEY_ENTER,
    UI_KEY_NEXT,
    UI_KEY_PREV
} ui_key_t;

typedef struct {
    int16_t x;
    int16_t y;
} ui_touch_t;

typedef enum {
    UI_EVENT_CLICKED,
    UI_EVENT_VALUE_CHANGED
} ui_event_t;



struct _ui_obj;

typedef void (*ui_draw_cb_t)(struct _ui_obj* obj, const g_region_t* region);
typedef bool (*ui_input_handler_t)(struct _ui_obj* obj, ui_input_type_t type, ui_key_t key); // True=event consumed; false otherwise
typedef void (*ui_event_handler_t)(struct _ui_obj* obj, ui_event_t event);

typedef struct _ui_obj {
    struct _ui_obj* parent;
    _ui_llist_t children;

    g_region_t region;
    ui_draw_cb_t draw;
    ui_style_t* style;
    ui_input_handler_t handle_input;
    ui_event_handler_t handle_event;

    void* ext;
    bool focusable : 1;     // Active objects handle interaction though input events and can be focused, passive objects can't
    bool clickable : 1;     // The object region is reactive to clicks/taps/enter buttons

    struct _ui_obj* input_override;
} ui_obj_t;

ui_obj_t* ui_obj_create(ui_obj_t* parent);
void ui_obj_delete(ui_obj_t* obj);

//void ui_obj_set_parent(ui_obj_t* obj, ui_obj_t* parent);

void ui_obj_set_pos(ui_obj_t* obj, g_coord_t x, g_coord_t y);
void ui_obj_set_x(ui_obj_t* obj, g_coord_t x);
void ui_obj_set_y(ui_obj_t* obj, g_coord_t y);

void ui_obj_set_size(ui_obj_t* obj, size_t width, size_t height);
void ui_obj_set_width(ui_obj_t* obj, size_t width);
void ui_obj_set_height(ui_obj_t* obj, size_t height);

g_coord_t ui_obj_get_x(ui_obj_t* obj);
g_coord_t ui_obj_get_y(ui_obj_t* obj);

size_t ui_obj_get_width(ui_obj_t* obj);
size_t ui_obj_get_height(ui_obj_t* obj);

void ui_obj_set_focusable(ui_obj_t* obj, bool focusable);
bool ui_obj_is_focusable(ui_obj_t* obj);

void ui_obj_set_clickable(ui_obj_t* obj, bool clickable);
bool ui_obj_is_clickable(ui_obj_t* obj);

ui_obj_t* ui_obj_get_last(ui_obj_t* obj);
ui_obj_t* ui_obj_find_next_focusable(ui_obj_t* focused_obj);
ui_obj_t* ui_obj_find_prev_focusable(ui_obj_t* focused_obj);

void ui_obj_set_event_handler(ui_obj_t* obj, ui_event_handler_t handler);
void ui_obj_dispatch_event(ui_obj_t* obj, ui_event_t event);

ui_style_value_t ui_obj_get_style_property(const ui_obj_t* obj, ui_style_prop_t prop);

static inline g_color_t ui_obj_get_style_bg_color(const ui_obj_t* obj)
{
    ui_style_value_t value = ui_obj_get_style_property(obj, UI_STYLE_BACKGROUND_COLOR);
    return value.color;
}

static inline g_color_t ui_obj_get_style_text_color(const ui_obj_t* obj)
{
    ui_style_value_t value = ui_obj_get_style_property(obj, UI_STYLE_TEXT_COLOR);
    return value.color;
}
