#pragma once
#include <esp_gatts_api.h>

// @brief GATTS implementing file transfer.
// There are 2 characteristics, one with read/write permission
// Upon read it lists files that are avaliable to get
// Upon write it selects a file that client wants to fetch
// Write triggers the task that starts indication on other characteristic
// Indications are of size 500 bytes, client **must** send MTU == 500 request
// In order to have correct data.
// Indication of first chunk is send, wait for ACK, then send second chunk and so on...

namespace bk {
/* Attributes State Machine */
enum {
    IDX_SVC,

    IDX_CHAR_FILE_TRANS,
    IDX_CHAR_VAL_FILE_TRANS,
    IDX_CHAR_CFG_FILE_TRANS,

    IDX_CHAR_FILE_LIST,
    IDX_CHAR_VAL_FILE_LIST,

    ATT_IDX_END,
};

#define PROFILE_NUM 1
#define PROFILE_NUM 1
#define PROFILE_APP_IDX 0

#define SVC_INST_ID 0

// TODO: remove those statics, my eyes bleeds!
class FileTransferGATTS {
 public:
    // Defines characteristics exposed by the server
    static const esp_gatts_attr_db_t gatt_db[ATT_IDX_END];

    struct gatts_profile_inst {
        esp_gatts_cb_t gatts_cb;
        uint16_t gatts_if;
    };

    // Keeps callback for event handler
    static gatts_profile_inst profile_tab[PROFILE_NUM];

    // Keeps handles to bluetooth characteristics, can be later used to set value on them
    // by, for example esp_ble_gatts_set_attr_value
    static uint16_t handle_table[ATT_IDX_END];

    static void test_indicate();

    // @brief Gets files that can be synced over BT, puts the listing on the buffer
    // @return number of bytes written to the buffer
    size_t storeFilesToSync(char *buffer);
 private:
};

}  // namespace bk
