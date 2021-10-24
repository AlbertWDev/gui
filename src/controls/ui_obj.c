#include "controls/ui_obj.h"

#include "view.h"
#include "gui.h"

#include <esp_log.h>
static const char* TAG = "UI_OBJ";


static void _ui_obj_draw(ui_obj_t* obj, const g_region_t* region) {
    //g_draw_rect(&obj->region, 0x0000, G_FILLED);

    g_color_t bg_color = ui_obj_get_style_bg_color(obj);
    g_draw_rect(&obj->region, bg_color, G_FILLED);
}

static bool _ui_obj_handle_input(ui_obj_t* obj, ui_input_type_t type, ui_key_t key) {
    if(obj->input_override)
        return obj->input_override->handle_input(obj->input_override, type, key);

    if(type == UI_INPUT_KEY) {
        if(key == UI_KEY_ENTER && obj->clickable) {
            ui_obj_dispatch_event(obj, UI_EVENT_CLICKED);
            ui_invalidate_region(obj->region);
            return true;
        }
    }

    return false;
}

ui_obj_t* ui_obj_create(ui_obj_t* parent) {
    ui_obj_t* obj = NULL;
    if(!parent) {
        obj = malloc(sizeof(ui_obj_t));
        if(!obj) return NULL;
        memset(obj, 0, sizeof(ui_obj_t));

        obj->style = NULL;
        obj->region.x0 = 0;
        obj->region.y0 = 0;
        obj->region.x1 = DISP_WIDTH - 1;
        obj->region.y1 = DISP_HEIGHT - 1;
    } else {
        obj = _ui_llist_add(&parent->children);
        if(!obj) return NULL;
        memset(obj, 0, sizeof(ui_obj_t));

        obj->parent = parent;

        obj->style = parent->style;
        obj->region.x0 = parent->region.x0;
        obj->region.y0 = parent->region.y0;
        obj->region.x1 = parent->region.x1;
        obj->region.y1 = parent->region.y1;
    }
    _ui_llist_init(&obj->children, sizeof(ui_obj_t));
    obj->draw = _ui_obj_draw;
    obj->handle_input = _ui_obj_handle_input;
    obj->handle_event = NULL;
    obj->focusable = false;
    obj->clickable = true;
    obj->input_override = NULL;

    ESP_LOGI(TAG, "[NEW] %p {parent=%p}", obj, obj->parent);

    return obj;
}

void ui_obj_delete(ui_obj_t* obj) {
    // Delete children recursively
    ui_obj_t* child_obj = _ui_llist_get_first(&obj->children);
    while(child_obj) {
        ui_obj_delete(child_obj);
        child_obj = _ui_llist_get_first(&obj->children);
    }

    // Remove object from parent's children
    if(obj->parent)
        _ui_llist_remove(&obj->parent->children, obj);

    ui_handle_deleted_obj(obj);
    
    if(obj->ext) free(obj->ext);
    free(obj);
}

void ui_obj_set_pos(ui_obj_t* obj, g_coord_t x, g_coord_t y) {
    if(obj->parent) {
        x += obj->parent->region.x0;
        y += obj->parent->region.y0;
    }

    g_coord_t x_diff = x - obj->region.x0;
    g_coord_t y_diff = y - obj->region.y0;

    obj->region.x0 = x;
    obj->region.y0 = y;
    obj->region.x1 += x_diff;
    obj->region.y1 += y_diff;


    // TODO: Update children position
}

void ui_obj_set_x(ui_obj_t* obj, g_coord_t x) {
    ui_obj_set_pos(obj, x, ui_obj_get_x(obj));
}

void ui_obj_set_y(ui_obj_t* obj, g_coord_t y) {
    ui_obj_set_pos(obj, ui_obj_get_y(obj), y);
}


void ui_obj_set_size(ui_obj_t* obj, size_t width, size_t height) {
    obj->region.x1 = obj->region.x0 + width - 1;
    obj->region.y1 = obj->region.y0 + height - 1;

    // TODO: Update children?
}

void ui_obj_set_width(ui_obj_t* obj, size_t width) {
    ui_obj_set_size(obj, width, ui_obj_get_height(obj));
}

void ui_obj_set_height(ui_obj_t* obj, size_t height) {
    ui_obj_set_size(obj, ui_obj_get_width(obj), height);
}


g_coord_t ui_obj_get_x(ui_obj_t* obj) {
    if(obj->parent)
        return obj->region.x0 - obj->parent->region.x0;
    else
        return obj->region.x0;
}

g_coord_t ui_obj_get_y(ui_obj_t* obj) {
    if(obj->parent)
        return obj->region.y0 - obj->parent->region.y0;
    else
        return obj->region.y0;
}


size_t ui_obj_get_width(ui_obj_t* obj) {
    return g_region_width(&obj->region);
}

size_t ui_obj_get_height(ui_obj_t* obj) {
    return g_region_height(&obj->region);
}

void ui_obj_set_focusable(ui_obj_t* obj, bool focusable) {
    obj->focusable = focusable;
}

bool ui_obj_is_focusable(ui_obj_t* obj) {
    return obj->focusable;
    /*if(obj->focusable) return true;

    ui_obj_t* child;
    _UI_LLIST_READ(&obj->children, child) {
        if(ui_obj_is_focusable(child)) return true;
    }
    return false;*/
}

void ui_obj_set_clickable(ui_obj_t* obj, bool clickable) {
    obj->clickable = clickable;
}

bool ui_obj_is_clickable(ui_obj_t* obj) {
    return obj->clickable;
}

ui_obj_t* ui_obj_find_next_focusable(ui_obj_t* focused_obj) {
    if(!focused_obj) return NULL;
    // TODO: Check for null (i.e. UI not initialized yet)
    /*if(!focused_obj) {
        focused_obj = ui_layer_sys();
        if(ui_obj_is_focusable(focused_obj)) return focused_obj;
    }*/

    ui_obj_t* parent = focused_obj->parent;
    ui_obj_t* current_obj = focused_obj;
    ui_obj_t* next_obj;

    ESP_LOGI("FIND_NEXT", "Finding a successor to %p", current_obj);

    do {
        // Check for children first
        next_obj = _ui_llist_get_first(&current_obj->children);
        if(next_obj) { // Current object has a child, so that's our new current object
            ESP_LOGI("FIND_NEXT", "Trying first child (%p) of %p", next_obj, current_obj);
            parent = current_obj;
            current_obj = next_obj;
            continue;
        } else {
            ESP_LOGI("FIND_NEXT", "%p has no children", current_obj);
        }

        if(parent) {
            // Check for siblings
            next_obj = _ui_llist_get_next(&parent->children, current_obj);
            if(next_obj) {  // Current object has a sibling, so that's our new current object
                ESP_LOGI("FIND_NEXT", "%p has a sibling (%p), sharing %p as parent", current_obj, next_obj, parent);
                current_obj = next_obj;
                continue;
            } else {
                ESP_LOGI("FIND_NEXT", "%p has no siblings", current_obj);
            }

            // Check for parent siblings recursively
            do {
                current_obj = parent;
                parent = parent->parent;
                ESP_LOGI("FIND_NEXT", "Trying to find siblings of its parent %p, sharing %p as grandparent", current_obj, parent);
            } while(parent && !(next_obj = _ui_llist_get_next(&parent->children, current_obj)));
            if(parent) { // Parent sibling was found only if we didn't reach the top of the tree
                current_obj = next_obj;
                ESP_LOGI("FIND_NEXT", "Found ancestor %p, whose parent is %p", current_obj, parent);
                continue;
            }
        }

        /*ESP_LOGI("FIND_NEXT", "Searching in another layer");
        // Move to another layer
        

        if(current_obj == ui_layer_sys())
            current_obj = ui_layer_top();
        else if(current_obj == ui_layer_top())
            current_obj = ui_layer_user();
        else
            return NULL;*/
        current_obj = NULL;
        break;
    } while(!ui_obj_is_focusable(current_obj));

    ESP_LOGI("FIND_NEXT", "Next object will be %p", current_obj);

    return current_obj;


    /*ui_obj_t* parent = focused_obj->parent;
    if(!parent) return NULL;

    ui_obj_t* obj = _ui_llist_get_next(&parent->children, focused_obj);
    while(obj) {
        if(ui_obj_is_focusable(obj)) return obj;
        obj = _ui_llist_get_next(&parent->children, obj);
    }
    return NULL;*/

    /*ui_obj_t* obj = focused_obj;
    ui_obj_t* parent = focused_obj->parent;

    while(parent) {
        obj = _ui_llist_get_next(&parent->children, obj);
        while(obj) {
            if(ui_obj_is_focusable(obj)) return obj;
            obj = _ui_llist_get_next(&parent->children, obj);
        }
        obj = parent;
        parent = parent->parent;
    }
    return NULL;*/
}

ui_obj_t* ui_obj_get_last(ui_obj_t* obj) {
    if(!obj) return NULL;

    ui_obj_t* child;
    while((child = _ui_llist_get_last(&obj->children)))
        obj = child;

    return obj;
}

ui_obj_t* ui_obj_find_prev_focusable(ui_obj_t* focused_obj) {
    if(!focused_obj) return NULL;

    ui_obj_t* parent = focused_obj->parent;
    ui_obj_t* current_obj = focused_obj;
    ui_obj_t* prev_obj;

    do {
        if(!parent) return NULL;

        prev_obj = _ui_llist_get_prev(&parent->children, current_obj);
        // If no siblings, previous object is the parent
        if(!prev_obj) {
            ESP_LOGI("FIND_NEXT", "%p has no siblings, moving to its parent %p", current_obj, parent);
            
            current_obj = parent;
            parent = parent->parent;

        } else {
            // Sibling found, search its deepest-right child
            ESP_LOGI("FIND_NEXT", "%p has a sibling (%p), sharing %p as parent", current_obj, prev_obj, parent);
            
            // Loop moving to get_last(children) until its NULL, then that's the next object
            current_obj = ui_obj_get_last(prev_obj);
            parent = current_obj->parent;
        }
    } while(!ui_obj_is_focusable(current_obj));

    
    return current_obj;


    /*ui_obj_t* parent = focused_obj->parent;
    if(!parent) return NULL;

    ui_obj_t* obj = _ui_llist_get_prev(&parent->children, focused_obj);
    while(obj) {
        if(ui_obj_is_focusable(obj)) return obj;
        obj = _ui_llist_get_prev(&parent->children, obj);
    }
    return NULL;*/

    /*ui_obj_t* obj = focused_obj;
    ui_obj_t* parent = focused_obj->parent;

    while(parent) {
        obj = _ui_llist_get_next(&parent->children, obj);
        while(obj) {
            if(ui_obj_is_focusable(obj)) return obj;
            obj = _ui_llist_get_next(&parent->children, obj);
        }
        obj = parent;
        parent = parent->parent;
    }
    return NULL;*/
}

void ui_obj_set_event_handler(ui_obj_t* obj, ui_event_handler_t handler) {
    obj->handle_event = handler;
}

void ui_obj_dispatch_event(ui_obj_t* obj, ui_event_t event) {
    if(obj && obj->handle_event)
        obj->handle_event(obj, event);
}

ui_style_value_t ui_obj_get_style_property(const ui_obj_t* obj, ui_style_prop_t property)
{
    ui_style_value_t value;
    if(obj->style && ui_style_get_property(obj->style, property, &value) == ESP_OK) {
        return value;
    }
    
    value = ui_style_get_default(property);
    return value;
}
