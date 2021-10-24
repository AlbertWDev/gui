#include <stdio.h>
#include <esp_spiffs.h>

#include <gui.h>
#include "ui/splash_view/splash_view.h"

#include <esp_log.h>
static const char* TAG = "MAIN";

void app_main()
{
    esp_err_t ret;

    // Initialize GUI
    ret = ui_init();
    if(ret != ESP_OK) return;

    // Initialize filesystem
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/data",
        .partition_label = "data",
        .max_files = 5,
        .format_if_mount_failed = true
    };
    ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Error: Failed to mount or format /system");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Error: System storage partition not found");
        } else {
            ESP_LOGE(TAG, "Error: SPIFFS initialization failed (%s)", esp_err_to_name(ret));
        }
        return;
    }

    // Load splash screen
    ui_view_add(new_splash_view());
}