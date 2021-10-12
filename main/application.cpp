#include "application.h"

#include "event_dispatcher.h"
#include "gnss.h"
#include "keypad.h"
#include "layout_factory.h"
#include "root_window.h"
#include "time_service.hpp"
#include "weather.h"
#include "fs_wrapper.h"
#include <display.h>
#include <string.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

// void file_testo_write() {
//     const char* TAG = "SPIFSS";
//     ESP_LOGI(TAG, "Initializing SPIFFS");

//     esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs",
//                                   .partition_label = NULL,
//                                   .max_files = 5,
//                                   .format_if_mount_failed = true};

//     // Use settings defined above to initialize and mount SPIFFS filesystem.
//     // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
//     esp_err_t ret = esp_vfs_spiffs_register(&conf);

//     if (ret != ESP_OK) {
//         if (ret == ESP_FAIL) {
//             ESP_LOGE(TAG, "Failed to mount or format filesystem");
//         } else if (ret == ESP_ERR_NOT_FOUND) {
//             ESP_LOGE(TAG, "Failed to find SPIFFS partition");
//         } else {
//             ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
//         }
//         return;
//     }

//     size_t total = 0, used = 0;
//     ret = esp_spiffs_info(conf.partition_label, &total, &used);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
//     } else {
//         ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
//     }

//     // Use POSIX and C standard library functions to work with files.
//     // First create a file.
//     ESP_LOGI(TAG, "Opening file");

//     // FILE* f = fopen("/spiffs/hello.txt", "w");
//     // if (f == NULL) {
//     //     ESP_LOGE(TAG, "Failed to open file for writing");
//     //     return;
//     // }
//     // fprintf(f, "Hello World!\n");
//     // fclose(f);
//     // ESP_LOGI(TAG, "File written");

//     ESP_LOGI(TAG, "Reading file");
//     FILE* f = fopen("/spiffs/hello.txt", "r");
//     if (f == NULL) {
//         ESP_LOGE(TAG, "Failed to open file for reading");
//         return;
//     }
//     char line[64];
//     fgets(line, sizeof(line), f);
//     fclose(f);
//     ESP_LOGI(TAG, "Read from file: '%s'", line);

//     // Check if destination file exists before renaming
//     // struct stat st;
//     // if (stat("/spiffs/foo.txt", &st) == 0) {
//     //     // Delete it if it exists
//     //     unlink("/spiffs/foo.txt");
//     // }

//     // // Rename original file
//     // ESP_LOGI(TAG, "Renaming file");
//     // if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0) {
//     //     ESP_LOGE(TAG, "Rename failed");
//     //     return;
//     // }

//     // // Open renamed file for reading
//     // ESP_LOGI(TAG, "Reading file");
//     // f = fopen("/spiffs/foo.txt", "r");
//     // if (f == NULL) {
//     //     ESP_LOGE(TAG, "Failed to open file for reading");
//     //     return;
//     // }
//     // char line[64];
//     // fgets(line, sizeof(line), f);
//     // fclose(f);
//     // // strip newline
//     // char* pos = strchr(line, '\n');
//     // if (pos) {
//     //     *pos = '\0';
//     // }
//     // ESP_LOGI(TAG, "Read from file: '%s'", line);

//     // All done, unmount partition and disable SPIFFS
//     esp_vfs_spiffs_unregister(conf.partition_label);
//     ESP_LOGI(TAG, "SPIFFS unmounted");

//     // FILE *pFile;
//     // const char *buffer = "it works!";
//     // pFile = fopen("myfile.bin", "w");

//     // ESP_LOGI("TESTO", "file pointer %p", pFile);
//     // fwrite(buffer, sizeof(char), strlen(buffer), pFile);
//     // fclose(pFile);
// }

void StartApplication() {
    static const char* TAG = "Application";

    bk::FSWrapper::mountStorage();

    bk::Display display;

    display.start();

    bk::Weather weather;
    bk::GNSS gnss;
    bk::Keypad keypad;
    bk::TimeService time_service;

    bk::EventDispatcher dispatcher(&weather, &gnss, &keypad, &time_service);
    time_service.setEventDispatcher(&dispatcher);

    weather.start();
    gnss.start();
    keypad.start();
    time_service.start();

    bk::LayoutFactory factory(&display, &dispatcher);

    bk::RootWindow root(&dispatcher, &factory);

    dispatcher.listenForEvents();
}