#pragma once

#include <graphics.h>

#include "controls/ui_obj.h"

/*
 * Prepares view to be displayed to the user. UI initialization
 * code should be here. If it returns other than ESP_OK, <type>_view_onDestroy
 * is immediately called and view is not displayed.
 * 
 * A CREATED view is now in ACTIVE state.
 */
typedef esp_err_t (*ui_view_onStart_t)(ui_obj_t** view, void* ctx);

/*
 * Prepares view to be paused before another view is displayed on top.
 * 
 * An ACTIVE view is now in PAUSED state.
 */
typedef void (*ui_view_onPause_t)(void* ctx);

/*
 * Prepares view to be resumed when the view on top has been destroyed.
 * 
 * A PAUSED view is now in ACTIVE state.
 */
typedef void (*ui_view_onResume_t)(void* ctx);

/*
 * Frees view resources before deletion
 * 
 * An ACTIVE view is now in DESTROYED state.
 */
typedef void (*ui_view_onDestroy_t)(void* ctx);

//typedef void (*ui_view_onInput_t)(void* ctx, input_source_t source, input_action_t action, void* data);
//typedef void (*ui_view_onEvent_t)(void* ctx, ui_event_type_t type, ui_key_t key);

//typedef void (*ui_view_draw_t)(void* ctx, const g_region_t* region);

typedef struct {
    ui_obj_t* root;
    ui_obj_t* focused_obj;
} ui_overlay_t;

typedef struct ui_view_t ui_view_t;
struct ui_view_t {
    ui_obj_t* root;
    ui_obj_t* focused_obj;

    ui_view_t* prev;
    void* ctx;
    ui_view_onStart_t onStart;      // View must get ready to be shown to the user (CREATED => ACTIVE)
    ui_view_onPause_t onPause;      // Another view is shown on top (ACTIVE => PAUSED)
    ui_view_onResume_t onResume;    // A top view is closed and this one comes back to front (PAUSED => ACTIVE)
    ui_view_onDestroy_t onDestroy;  // View must free its resources. Called before deletion (ACTIVE => DESTROYED)
};

ui_overlay_t* ui_overlay_create();

#define NEW_VIEW(view_name) { .start = view_name ## _onStart, .stop = view_name ## _stop }

ui_view_t* new_view(ui_view_onStart_t onStart, ui_view_onPause_t onPause, ui_view_onResume_t onResume,
    ui_view_onDestroy_t onDestroy);

esp_err_t ui_view_set_focus(ui_obj_t* obj);
ui_obj_t* ui_view_get_focus();
