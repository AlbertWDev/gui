#include "gui.h"

#include "controls/ui_obj.h"

// TODO: Fix focused obj not reset when obj is deleted
// TODO: What happens when the focused obj doesn't exist anymore? Or when it belongs to a previous view?



ui_layer_t _sys_layer = { .type = UI_LAYER_OVERLAY };
ui_layer_t _top_layer = { .type = UI_LAYER_OVERLAY };
ui_layer_t _user_layer = { .type = UI_LAYER_VIEW, .view = NULL };
ui_layer_t* _focused_layer = &_user_layer;

void ui_input_default_handler(ui_obj_t* focused_obj, ui_input_type_t type, ui_key_t key);

ui_obj_t* ui_layer_get_root(ui_layer_t* layer) {
    if(layer->type == UI_LAYER_OVERLAY && layer->overlay)
        return layer->overlay->root;
    else if(layer->type == UI_LAYER_VIEW && layer->view)
        return layer->view->root;
    else
        return NULL;
}

ui_obj_t* ui_layer_get_last(ui_layer_t* layer) {
    ui_obj_t* root = ui_layer_get_root(layer);
    if(!root) return NULL;
    return ui_obj_get_last(root);
}

ui_obj_t* ui_layer_get_focus(ui_layer_t* layer) {
    if(layer->type == UI_LAYER_OVERLAY && layer->overlay)
        return layer->overlay->focused_obj;
    else if(layer->type == UI_LAYER_VIEW && layer->view)
        return layer->view->focused_obj;
    else
        return NULL;
}

esp_err_t ui_set_focus(ui_layer_t* layer) {
    _focused_layer = layer;
    return ESP_OK;
}

void ui_refocus() {
    if(!ui_layer_get_focus(_focused_layer)) {
        ui_input_default_handler(NULL, UI_INPUT_KEY, UI_KEY_NEXT);
    }
}

esp_err_t ui_layer_set_focus(ui_layer_t* layer, ui_obj_t* obj) {
    if(layer->type == UI_LAYER_OVERLAY && layer->overlay)
        layer->overlay->focused_obj = obj;
    else if(layer->type == UI_LAYER_VIEW && layer->view)
        layer->view->focused_obj = obj;
    else
        return ESP_FAIL;
    
    return ESP_OK;
}

ui_obj_t* ui_layer_find_next_focusable(ui_layer_t* layer) {
    ui_obj_t* focused_obj = ui_layer_get_focus(layer);
    if(!focused_obj) {
        focused_obj = ui_layer_get_root(layer);
        if(!focused_obj) return NULL;
        if(ui_obj_is_focusable(focused_obj)) return focused_obj;
    }

    return ui_obj_find_next_focusable(focused_obj);
}

ui_layer_t* ui_layer_next(ui_layer_t* layer) {
    if(layer == &_sys_layer)
        return &_top_layer;
    else if(layer == &_top_layer)
        return &_user_layer;
    else
        return NULL;
}

ui_layer_t* ui_layer_prev(ui_layer_t* layer) {
    if(layer == &_user_layer)
        return &_top_layer;
    else if(layer == &_top_layer)
        return &_sys_layer;
    else
        return NULL;
}

const char* LAYER_NAME(ui_layer_t* layer) {
    if(!layer)
        return "NULL";
    if(layer == &_sys_layer)
        return "SYS";
    else if(layer == &_top_layer)
        return "TOP";
    else if(layer == &_user_layer)
        return "USER";
    else
        return "UNKNOWN";
}

QueueHandle_t _ui_render_queue;
TaskHandle_t _renderTask;
void renderTask(void*);

//g_region_t _work_area;

esp_err_t ui_init() {
    esp_err_t ret;

    if((ret = g_init()) != ESP_OK) return ret;

    _sys_layer.overlay = ui_overlay_create();
    // ui_obj_set_clickable(_sys_layer, false);
    _top_layer.overlay = ui_overlay_create();
    // ui_obj_set_clickable(_top_layer, false);

    _ui_render_queue = xQueueCreate(10, sizeof(struct g_region_t*));
    if(!_ui_render_queue) return ESP_ERR_NO_MEM;
    
    if(xTaskCreate(renderTask, "ui_render", 4*1024, NULL, 1, &_renderTask) != pdPASS) return ESP_FAIL;

    return ESP_OK;
}

esp_err_t ui_view_add(ui_view_t* v) {
    if(!v) return ESP_FAIL;
    v->prev = _user_layer.view;
    
    // Pause previous view
    if(_user_layer.view && _user_layer.view->onPause) _user_layer.view->onPause(_user_layer.view->ctx);

    // Start new view
    esp_err_t ret;
    if(v->onStart && (ret = v->onStart(&v->root, v->ctx)) != ESP_OK) {
        if(_user_layer.view && _user_layer.view->onResume) _user_layer.view->onResume(_user_layer.view->ctx);
        if(v->onDestroy) v->onDestroy(v->ctx);
        return ret;
    }

    // TODO: Views can't specify focused object
    //if(!v->focused_obj) v->focused_obj = v->root;

    

    // TODO: Wait rendering?
    _user_layer.view = v;

    // Focus first focusable object in new view
    ui_refocus();
    // TODO: This is a workaround (simulating a NEXT key event with no focused object)
    //ui_input_default_handler(NULL, UI_INPUT_KEY, UI_KEY_NEXT);
    ui_invalidate();

    return ESP_OK;
}

esp_err_t ui_view_remove() {
    if(!_user_layer.view) return ESP_FAIL;

    // TODO: Wait rendering?
    ui_view_t* v = _user_layer.view;
    _user_layer.view = _user_layer.view->prev;

    // Re-activate last view
    if(_user_layer.view->onResume) _user_layer.view->onResume(_user_layer.view->ctx);
    ui_invalidate();
    
    // Destroy old view
    if(v->onDestroy) v->onDestroy(v->ctx);
    if(v->root)
        ui_obj_delete(v->root);
    free(v->ctx);
    free(v);
    
    return ESP_OK;
}

void* ui_view_get_current_context() {
    if(!_user_layer.view) return NULL;
    return _user_layer.view->ctx;
}

ui_layer_t* ui_layer_sys() {
    //return _sys_layer.overlay->root;
    return &_sys_layer;
}

ui_layer_t* ui_layer_top() {
    //return _top_layer.overlay->root;
    return &_top_layer;
}

ui_layer_t* ui_layer_user() {
    //if(_user_layer.view) return _user_layer.view->root;
    //return NULL;
    return &_user_layer;
}

void ui_invalidate() {
    //xTaskNotify(_renderTask, 0, eNoAction);
    ui_invalidate_region((g_region_t){.x0=0, .y0=0, .x1=DISP_WIDTH-1, .y1=DISP_HEIGHT-1}/*G_REGION_DISPLAY*/);
}

void ui_invalidate_region(g_region_t region) {
    g_region_t* reg = malloc(sizeof(g_region_t));
    memcpy(reg, &region, sizeof(g_region_t));
    xQueueSend(_ui_render_queue, (void*)&reg, (TickType_t)0);
}


void ui_input_default_handler(ui_obj_t* focused_obj, ui_input_type_t type, ui_key_t key) {
    //default event handler:
    //    - NEXT:
    //          Move to next element, respecting layer order.
    //          If no focused element:
    //              1: Select focused layer root.
    //              2: If no root, move to next layer.
    //              3: If no focused element, select focused layer root.
    //              4: If no root, move to next layer, etc. (until last layer)
    //    - PREV:
    //          Move to prev element, respecting layer order
    ESP_LOGI("GUI [Input Handler]", "Default input handler triggered");

    ui_layer_t* current_layer = _focused_layer;
    ui_layer_t* next_layer;

    if(type == UI_INPUT_KEY) {
        if(key == UI_KEY_NEXT) {
            if(focused_obj) {
                ESP_LOGI("GUI [Input Handler]", "Focused object is [%s::%p], finding next focusable", LAYER_NAME(current_layer), focused_obj);
                focused_obj = ui_obj_find_next_focusable(focused_obj);
                if(!focused_obj) {
                    next_layer = ui_layer_next(current_layer);
                    ESP_LOGI("GUI [Input Handler]", "No more focusable objects in layer %s, moving to layer %s", LAYER_NAME(current_layer), LAYER_NAME(next_layer));
                    if(!next_layer) return;
                    current_layer = next_layer;
                }
            }
            while(!focused_obj) {
                ESP_LOGI("GUI [Input Handler]", "Checking if layer %s has a focused object", LAYER_NAME(current_layer));
                focused_obj = ui_layer_get_focus(current_layer);
                if(focused_obj) break;

                ESP_LOGI("GUI [Input Handler]", "No focused object at layer %s, checking root object", LAYER_NAME(current_layer));
                focused_obj = ui_layer_get_root(current_layer);
                if(focused_obj) {
                    ESP_LOGI("GUI [Input Handler]", "Checking if root [%s::%p] is focusable", LAYER_NAME(current_layer), focused_obj);
                    if(ui_obj_is_focusable(focused_obj)) break;
                    ESP_LOGI("GUI [Input Handler]", "Searching next focusable starting from root [%s::%p]", LAYER_NAME(current_layer), focused_obj);
                    focused_obj = ui_obj_find_next_focusable(focused_obj);
                    if(focused_obj) break;
                }

                next_layer = ui_layer_next(current_layer);
                ESP_LOGI("GUI [Input Handler]", "No focusable object found in layer %s, moving to layer %s", LAYER_NAME(current_layer), LAYER_NAME(next_layer));
                if(!next_layer) return;
                current_layer = next_layer;
            }

            ESP_LOGI("GUI [Input Handler]", "Consumed KEY::NEXT event, focusing [%s::%p]", LAYER_NAME(current_layer), focused_obj);
            ui_set_focus(current_layer);
            ui_layer_set_focus(current_layer, focused_obj);
            //ui_invalidate_region(cont->region);
            return;
        }
        else if(key == UI_KEY_PREV) {
            if(focused_obj) {
                ESP_LOGI("GUI [Input Handler]", "Focused object is [%s::%p], finding prev focusable", LAYER_NAME(current_layer), focused_obj);
                focused_obj = ui_obj_find_prev_focusable(focused_obj);
                if(!focused_obj) {
                    next_layer = ui_layer_prev(current_layer);
                    ESP_LOGI("GUI [Input Handler]", "No more focusable objects in layer %s, moving to layer %s", LAYER_NAME(current_layer), LAYER_NAME(next_layer));
                    if(!next_layer) return;
                    current_layer = next_layer;
                }
            }
            while(!focused_obj) {
                ESP_LOGI("GUI [Input Handler]", "Checking if layer %s has a focused object", LAYER_NAME(current_layer));
                focused_obj = ui_layer_get_focus(current_layer);
                if(focused_obj) break;

                ESP_LOGI("GUI [Input Handler]", "No focused object at layer %s, checking last object", LAYER_NAME(current_layer));
                focused_obj = ui_layer_get_last(current_layer);
                if(focused_obj) {
                    ESP_LOGI("GUI [Input Handler]", "Checking if last obj [%s::%p] is focusable", LAYER_NAME(current_layer), focused_obj);
                    if(ui_obj_is_focusable(focused_obj)) break;
                    ESP_LOGI("GUI [Input Handler]", "Searching prev focusable starting from last obj [%s::%p]", LAYER_NAME(current_layer), focused_obj);
                    focused_obj = ui_obj_find_prev_focusable(focused_obj);
                    if(focused_obj) break;
                }

                next_layer = ui_layer_prev(current_layer);
                ESP_LOGI("GUI [Input Handler]", "No focusable object found in layer %s, moving to layer %s", LAYER_NAME(current_layer), LAYER_NAME(next_layer));
                if(!next_layer) return;
                current_layer = next_layer;
            }

            ESP_LOGI("GUI [Input Handler]", "Consumed KEY::PREV event, focusing [%s::%p]", LAYER_NAME(current_layer), focused_obj);
            ui_set_focus(current_layer);
            ui_layer_set_focus(current_layer, focused_obj);
            //ui_invalidate_region(cont->region);
            return;
        }
    }
}

void ui_input_handler(ui_input_type_t type, ui_key_t key) {
    // touch mode:
    // Find touched element recursively, respecting layer order:
    //      - SYS layer
    //      - TOP layer
    //      - VIEW layer
    // If not clickable or not consumed, check parents recursively (only clickable ones)


    ui_obj_t* focused_obj = ui_layer_get_focus(_focused_layer);
    ESP_LOGI("GUI [Input Handler]", "Focused object is [%s::%p]", LAYER_NAME(_focused_layer), focused_obj);

    // Iterate through focused obj and its parents until someone consumes the event
    ui_obj_t* target_obj = focused_obj;
    while(target_obj && !target_obj->handle_input(target_obj, type, key)) {
        target_obj = target_obj->parent;
        ESP_LOGI("GUI [Input Handler]", "Event not consumed, dispatching to its parent: [%s::%p]", LAYER_NAME(_focused_layer), target_obj);
    }

    if(!target_obj) {
        ESP_LOGI("GUI [Input Handler]", "Event not consumed by objects at layer %s", LAYER_NAME(_focused_layer));
        ui_input_default_handler(focused_obj, type, key);
    }

    ui_invalidate();
}

void _layer_handle_deleted_obj(ui_layer_t* layer, ui_obj_t* obj) {
    if(obj == ui_layer_get_focus(layer)) {
        ui_layer_set_focus(layer, NULL);
        ui_refocus();
        //ui_input_default_handler(NULL, UI_INPUT_KEY, UI_KEY_NEXT);
        // TODO: Refactor handling NULL focused objects
    }
}

void ui_handle_deleted_obj(ui_obj_t* obj) {
    // TODO: Transform this into a generic event handler

    _layer_handle_deleted_obj(&_sys_layer, obj);
    _layer_handle_deleted_obj(&_top_layer, obj);
    _layer_handle_deleted_obj(&_user_layer, obj);
    // TODO: What to do if deleted obj belonged to another view in USER layer?
}

static void _ui_draw_recursive(ui_obj_t* obj, const g_region_t* region) {
    if(obj->draw) obj->draw(obj, region);

    ui_obj_t* child_obj;
    _UI_LLIST_READ(&obj->children, child_obj) {
        _ui_draw_recursive(child_obj, region);
    }
}

static void _ui_draw(const g_region_t* region) {
    ui_obj_t* root = ui_layer_get_root(&_user_layer);
    if(root) _ui_draw_recursive(root, region);

    _ui_draw_recursive(ui_layer_get_root(&_top_layer), region);
    _ui_draw_recursive(ui_layer_get_root(&_sys_layer), region);

    ui_obj_t* focused_obj = ui_layer_get_focus(_focused_layer);
    if(focused_obj)
        g_draw_rect(&focused_obj->region, HEX_TO_COLOR(0xF800), 1);
}

void renderTask(void *pvParameters)
{
    g_region_t* invalidated_region;
    while(1) {
        if(xQueueReceive(_ui_render_queue, &invalidated_region, pdMS_TO_TICKS(1000)) == pdFALSE) continue;
        //if(xTaskNotifyWait(0, 0, NULL, portMAX_DELAY) == pdFALSE) continue;
        
        /*if(!_view) {
            free(invalidated_region);
            break;
            // TODO: Recover from null view
        }*/
        
        g_refresh_region(invalidated_region, _ui_draw);
        free(invalidated_region);
    }
    vTaskDelete(NULL);
}
