#include "fs_wrapper.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
#include <esp_spiffs.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <dirent.h>

namespace bk {

void FSWrapper::mountStorage() {
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {.base_path = "/storage",
                                  .partition_label = "storage",
                                  .max_files = 5,
                                  .format_if_mount_failed = true};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    // ESP_LOGE(TAG, "Formatting ....");

    // ret = esp_spiffs_format(conf.partition_label);

    // ESP_LOGE(TAG, "Format result (%s)", esp_err_to_name(ret));

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

std::vector<FileInfo> FSWrapper::listFiles(const std::string &partition) {
    std::vector<FileInfo> files;

    // In year 2021 C++ compilers still does not support std::filesystem yay!
    // Switch to the old C API!
    std::string path = "/" + partition;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string full_path = path + "/" + ent->d_name;

            struct stat st = {};
            stat(full_path.c_str(), &st);

            files.push_back(FileInfo{.filename=ent->d_name, .size=st.st_size});

            ESP_LOGD(TAG, "file: %s size: %ld", ent->d_name, st.st_size);
        }
        closedir(dir);
    } else {
        /* could not open directory */
        ESP_LOGE(TAG, "Cannot open directory %s", path.c_str());
    }

    return files;
}


}  // namespace bk