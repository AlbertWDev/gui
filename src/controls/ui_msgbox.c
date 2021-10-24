#include "controls/ui_msgbox.h"

#include "controls/ui_label.h"

/// TODO: Implement alertbox object that takes generic obj as content??

static void _ok_onclick_event(ui_obj_t* ok_btn, ui_event_t event) {
    if(event == UI_EVENT_CLICKED) {
        ui_obj_t* msg_box = ok_btn->parent;
        ui_msgbox_close(msg_box);
    }
}

static bool _consume_input_event(ui_obj_t* obj, ui_input_type_t type, ui_key_t key) {
    if(type == UI_INPUT_KEY) {
        if(key == UI_KEY_ENTER && obj->clickable) {
            ui_obj_dispatch_event(obj, UI_EVENT_CLICKED);
            ui_invalidate_region(obj->region);
        }
    }

    // Consume event always, even if we didn't handle it
    return true;
}

typedef int ui_msgbox_button_t;

ui_obj_t* ui_msgbox_create(ui_layer_t* layer, const char* title, const char* content, bool modal) {
    if(!layer) layer = ui_layer_top();

    ui_obj_t* layer_root = ui_layer_get_root(layer);
    if(!layer_root) return NULL;

    ui_obj_t* msg_box = ui_obj_create(layer_root);
    /// TODO: Wrap to content size
    ui_obj_set_size(msg_box, DISP_WIDTH/2, DISP_HEIGHT/2);
    ui_obj_set_pos(msg_box, DISP_WIDTH/4, DISP_HEIGHT/4);
    if(modal) {
        msg_box->handle_input = _consume_input_event;
    }

    ui_obj_t* title_lbl = ui_label_create(msg_box);
    ui_obj_set_height(title_lbl, 12);
    ui_label_set_text(title_lbl, title);

    ui_obj_t* content_lbl = ui_label_create(msg_box);
    ui_obj_set_y(content_lbl, 15);
    ui_label_set_text(content_lbl, content);

    ui_obj_t* ok_btn = ui_obj_create(msg_box);
    ui_obj_set_height(ok_btn, 12);
    ui_obj_set_y(ok_btn, DISP_HEIGHT/2 - 12);
    ui_obj_set_event_handler(ok_btn, _ok_onclick_event);
    ui_obj_set_focusable(ok_btn, true);

    ui_obj_t* ok_lbl = ui_label_create(ok_btn);
    ui_label_set_text(ok_lbl, "OK");

    ui_set_focus(layer);
    /// TODO: Implement proper way to select focused element
    ui_refocus();
    ui_invalidate_region(msg_box->region);

    return msg_box;
}

void ui_msgbox_close(ui_obj_t* msg_box) {
    ui_obj_delete(msg_box);
    ui_invalidate();
}
