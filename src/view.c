#include "view.h"

static void _overlay_draw(ui_obj_t* obj, const g_region_t* region) { }

ui_overlay_t* ui_overlay_create() {
    ui_overlay_t* overlay = malloc(sizeof(ui_overlay_t));
    if(!overlay) return NULL;
    overlay->root = ui_obj_create(NULL);
    overlay->root->draw = _overlay_draw;    /// TODO: use styles to avoid drawing
    overlay->focused_obj = NULL;    /// TODO: focus 'root' if focuseable??
    return overlay;
}

ui_view_t* new_view(ui_view_onStart_t onStart, ui_view_onPause_t onPause, ui_view_onResume_t onResume,
    ui_view_onDestroy_t onDestroy)
{
    ui_view_t* v = malloc(sizeof(ui_view_t));
    if(!v) return NULL;
    
    v->ctx = NULL;
    v->focused_obj = NULL;
    v->onStart = onStart;
    v->onPause = onPause;
    v->onResume = onResume;
    v->onDestroy = onDestroy;

    return v;
}
