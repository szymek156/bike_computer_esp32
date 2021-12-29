#include "file_transfer_gatts.h"

#include <cstring>

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatt_common_api.h>
#include <esp_gatts_api.h>
#include <esp_log.h>
#include <fs_wrapper.h>

// For future me, maybe I will be insane enough to visit this part of the code again.
// TODO: BLE API is horrible, verbose, and in many places undocumented.
// This is minimal working piece of code that allows me to do what I want,
// i.e. sending the file
// 1. Read carefully through
//    https://punchthrough.com/ble-throughput-part-4/
//    Find pieces I missed in following implementation
// 2. Use human friendly wrapper for BLE https://github.com/nkolban/ESP32_BLE_Arduino
// 3. Great resource of wisdom is https://github.com/chegewara/esp32-OTA-over-BLE
namespace bk {

static constexpr const char *TAG = "BT_GATTS";

/* Service */
static const uint16_t GATTS_SERVICE_UUID_FILE_TRANS = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_FILE_TRANS = 0xFF01;
static const uint16_t GATTS_CHAR_UUID_TEST_FILE_LIST = 0xFF02;

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read_write =
    ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_indicate = ESP_GATT_CHAR_PROP_BIT_INDICATE;

static const uint8_t char_value[ESP_GATT_MAX_ATTR_LEN] = {};

// Max supported MTU is 500, -3 that's some message overhead, resulting in reality
// in that value that can be stored at once in a characteristic
const int ATTR_LEN = 500 - 3;

// uint16_t FileTransferGATTS::handle_table[ATT_IDX_END] = {};

#define CHAR_DECLARATION_SIZE (sizeof(uint8_t))

static uint8_t value_to_send[ESP_GATT_MAX_ATTR_LEN] = {};

FileTransferGATTS::FileTransferGATTS()
    : gatt_db{
          // Service Declaration
          [IDX_SVC] = {{ESP_GATT_AUTO_RSP},
                       {ESP_UUID_LEN_16,
                        (uint8_t *)&primary_service_uuid,
                        ESP_GATT_PERM_READ,
                        sizeof(uint16_t),
                        sizeof(GATTS_SERVICE_UUID_FILE_TRANS),
                        (uint8_t *)&GATTS_SERVICE_UUID_FILE_TRANS}},

          /* Characteristic Declaration */
          [IDX_CHAR_FILE_TRANS] = {{ESP_GATT_AUTO_RSP},
                                   {ESP_UUID_LEN_16,
                                    (uint8_t *)&character_declaration_uuid,
                                    ESP_GATT_PERM_READ,
                                    CHAR_DECLARATION_SIZE,
                                    CHAR_DECLARATION_SIZE,
                                    (uint8_t *)&char_prop_indicate}},

          /* Characteristic Value */
          [IDX_CHAR_VAL_FILE_TRANS] = {{ESP_GATT_RSP_BY_APP},
                                       {ESP_UUID_LEN_16,
                                        (uint8_t *)&GATTS_CHAR_UUID_TEST_FILE_TRANS,
                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                        ESP_GATT_MAX_ATTR_LEN,
                                        sizeof(char_value),
                                        (uint8_t *)char_value}},

          /* Client Characteristic Configuration Descriptor */
          // Configuration is needed to be able to sub or unsub for indications
          // Set to ESP_GATT_AUTO_RSP, so I don't have to worry about handling those requests
          [IDX_CHAR_CFG_FILE_TRANS] = {{ESP_GATT_AUTO_RSP},
                                       {ESP_UUID_LEN_16,
                                        (uint8_t *)&character_client_config_uuid,
                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                        ESP_GATT_MAX_ATTR_LEN,
                                        sizeof(char_value),
                                        (uint8_t *)char_value}},

          /////////////////////////////////////////////////////////////////////////////////////////
          /* Characteristic Declaration */
          [IDX_CHAR_FILE_LIST] = {{ESP_GATT_AUTO_RSP},
                                  {ESP_UUID_LEN_16,
                                   (uint8_t *)&character_declaration_uuid,
                                   ESP_GATT_PERM_READ,
                                   CHAR_DECLARATION_SIZE,
                                   CHAR_DECLARATION_SIZE,
                                   // This declares what you can do with the characteristic,
                                   // In nRF Connect app that defines what arrows are visible
                                   // in this case read and write will be seen
                                   (uint8_t *)&char_prop_read_write}},

          /* Characteristic Value */
          [IDX_CHAR_VAL_FILE_LIST] =
              {{ESP_GATT_RSP_BY_APP},
               {ESP_UUID_LEN_16,
                (uint8_t *)&GATTS_CHAR_UUID_TEST_FILE_LIST,

                // Those are actual characteristic permission, I have no idea why it's duplicated
                // (refer to (uint8_t *)&char_prop_read_write) in characteristic declaration
                ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                ESP_GATT_MAX_ATTR_LEN,
                sizeof(char_value),
                (uint8_t *)char_value}},
      },
      handle_table{} {
}

void FileTransferGATTS::test_indicate() {
    ESP_LOGI(TAG, "Setting the value for indication");

    static int COUNTER = 0;

    COUNTER++;
    memset(value_to_send, COUNTER, ESP_GATT_MAX_ATTR_LEN);

    // TODO: Check that we have a connection
    ESP_LOGI(TAG,
             "Sending indication directly app id 0x%X, gatts_if 0x%X, handle 0x%X",
             SVC_INST_ID,
             this->gatts_if,
             handle_table[IDX_CHAR_VAL_FILE_TRANS]);

    // I (29855) BT_GATTS: Sending an indication, app id 0x0, gatts_if 0x3, handle 0x2A
    bool needs_confirmation = true;

    esp_ble_gatts_send_indicate(this->gatts_if,
                                SVC_INST_ID /* app_id */,
                                handle_table[IDX_CHAR_VAL_FILE_TRANS],
                                ATTR_LEN,
                                value_to_send,
                                needs_confirmation);
}

void FileTransferGATTS::gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                                    esp_gatt_if_t gatts_if,
                                                    esp_ble_gatts_cb_param_t *param) {
    ESP_LOGI(TAG, "gatts profile event %u decimal", event);
    switch (event) {
        case ESP_GATTS_READ_EVT: {
            ESP_LOGD(TAG,
                     "ESP_GATTS_READ_EVT conn id %u, trans id %u, attr handle %u, offset %u, is "
                     "long %u need resp %u",
                     param->read.conn_id,
                     param->read.trans_id,
                     param->read.handle,
                     param->read.offset,
                     param->read.is_long,
                     param->read.need_rsp);

            if (handle_table[IDX_CHAR_VAL_FILE_LIST] == param->read.handle) {
                // User requested list the files
                ESP_LOGI(TAG, "Got request to list the files");

                if (!param->read.need_rsp) {
                    // If characteristic in the table is set to auto response, this will be false
                    ESP_LOGW(TAG,
                             "Got ESP_GATTS_READ_EVT, but response is not expected, check GATT "
                             "table configuration");
                    break;
                }

                esp_gatt_rsp_t response = {};

                FileTransferGATTS f;

                auto resp_len = f.storeFilesToSync((char *)response.attr_value.value);
                response.attr_value.len = resp_len;
                response.attr_value.offset = param->read.offset;
                response.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
                response.attr_value.handle = param->read.handle;

                ESP_ERROR_CHECK(esp_ble_gatts_send_response(
                    gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &response));
            } else {
                ESP_LOGE(
                    TAG, "Got ESP_GATTS_READ_EVT on unsupported handle: %u", param->read.handle);
            }

            break;
        }

        case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_WRITE_EVT");
            // if (param->write.handle == m_handle) {
            // 	if (param->write.is_prep) {
            // 		m_value.addPart(param->write.value, param->write.len);
            // 		m_writeEvt = true;
            // 	} else {
            // 		setValue(param->write.value, param->write.len);
            // 		if (m_pCallbacks != nullptr && param->write.is_prep != true) {
            // 			m_pCallbacks->onWrite(this); // Invoke the onWrite callback handler.
            // 		}
            // 	}

            // 	ESP_LOGD(LOG_TAG, " - Response to write event: New value: handle: %.2x, uuid: %s",
            // 			getHandle(), getUUID().toString().c_str());

            // 	char* pHexData = BLEUtils::buildHexData(nullptr, param->write.value,
            // param->write.len); 	ESP_LOGD(LOG_TAG, " - Data: length: %d, data: %s",
            // param->write.len, pHexData); 	free(pHexData);

            // 	if (param->write.need_rsp) {
            // 		esp_gatt_rsp_t rsp;

            // 		rsp.attr_value.len      = param->write.len;
            // 		rsp.attr_value.handle   = m_handle;
            // 		rsp.attr_value.offset   = param->write.offset;
            // 		rsp.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            // 		memcpy(rsp.attr_value.value, param->write.value, param->write.len);

            // 		esp_err_t errRc = ::esp_ble_gatts_send_response(
            // 				gatts_if,
            // 				param->write.conn_id,
            // 				param->write.trans_id, ESP_GATT_OK, &rsp);
            // 		if (errRc != ESP_OK) {
            // 			ESP_LOGE(LOG_TAG, "esp_ble_gatts_send_response: rc=%d %s", errRc,
            // GeneralUtils::errorToString(errRc));
            // 		}
            // 	} // Response needed

            // } // Match on handles.
            break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            // the length of gattc prepare write data must be less than ESP_GATT_MAX_ATTR_LEN.
            ESP_LOGI(TAG, "ESP_GATTS_EXEC_WRITE_EVT");
            // example_exec_write_event_env(&prepare_write_env, param);
            break;
        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
            break;
        case ESP_GATTS_CONF_EVT:
            // Confirmation of indication, if status != 0 (invalid)
            // len, and value contains the packet that failed
            ESP_LOGI(TAG,
                     "ESP_GATTS_CONF_EVT, - confirmation, status = %d, attr_handle %d",
                     param->conf.status,
                     param->conf.handle);
            break;
        case ESP_GATTS_START_EVT:
            ESP_LOGI(TAG,
                     "SERVICE_START_EVT, status %d, service_handle %d",
                     param->start.status,
                     param->start.service_handle);
            break;
        case ESP_GATTS_CONNECT_EVT: {
            ESP_LOGI(TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
            // Configure connection parameters

            // Log a buffer of hex bytes at info level
            ESP_LOGI(TAG, "Remote device address:");
            esp_log_buffer_hex(TAG, param->connect.remote_bda, 6);

            esp_ble_conn_update_params_t conn_params = {};

            // BDA - bluetooth remote device address
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            conn_params.latency = 0;
            conn_params.max_int = 0x6;  // max_int = 0x20*1.25ms = 40ms
            conn_params.min_int = 0x6;  // min_int = 0x10*1.25ms = 20ms
            conn_params.timeout = 400;  // timeout = 400*10ms = 4000ms
            // start sent the update connection parameters to the peer device.
            esp_ble_gap_update_conn_params(&conn_params);
            break;
        }
        case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
            ESP_LOGI(TAG, "ESP_GATTS_CREAT_ATTR_TAB_EVT");

            if (param->add_attr_tab.status != ESP_GATT_OK) {
                ESP_LOGE(TAG,
                         "create attribute table failed, error code=0x%x",
                         param->add_attr_tab.status);
            } else if (param->add_attr_tab.num_handle != ATT_IDX_END) {
                ESP_LOGE(TAG,
                         "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to ATT_IDX_END(%d)",
                         param->add_attr_tab.num_handle,
                         ATT_IDX_END);
            } else {
                ESP_LOGI(TAG,
                         "create attribute table successfully, the number handle = %d\n",
                         param->add_attr_tab.num_handle);
                memcpy(FileTransferGATTS::handle_table,
                       param->add_attr_tab.handles,
                       sizeof(FileTransferGATTS::handle_table));
                esp_ble_gatts_start_service(FileTransferGATTS::handle_table[IDX_SVC]);
            }
            break;
        }
        case ESP_GATTS_SET_ATTR_VAL_EVT: {
            uint16_t handle = param->set_attr_val.attr_handle;
            ESP_LOGW(TAG,
                     "ESP_GATTS_SET_ATTR_VAL_EVT Set attribute event, app id 0x%X, gatts_if 0x%X, "
                     "handle 0x%X",
                     param->reg.app_id,
                     gatts_if,
                     handle);

            break;
        }
        case ESP_GATTS_STOP_EVT:
        case ESP_GATTS_OPEN_EVT:
        case ESP_GATTS_CANCEL_OPEN_EVT:
        case ESP_GATTS_CLOSE_EVT:
        case ESP_GATTS_LISTEN_EVT:
        case ESP_GATTS_CONGEST_EVT:
        case ESP_GATTS_UNREG_EVT:
        case ESP_GATTS_DELETE_EVT:
        // TODO:
        default:
            break;
    }
}

size_t FileTransferGATTS::storeFilesToSync(char *buffer) {
    // TODO: to have implementation simple file listing needs to fit on ~500 bytes

    auto files = FSWrapper::listFiles("storage");

    size_t total_len = 0;

    for (const auto &info : files) {
        // Don't store buffer on the stack, to avoid overflow
        static const int msg_size = 256;
        static char message[msg_size];

        snprintf(message, msg_size, "%s, %lu\n", info.filename.c_str(), info.size);

        size_t message_len = strlen(message);
        ESP_LOGD(TAG, "File info len %u", message_len);

        if (total_len + message_len < ATTR_LEN) {
            snprintf((char *)(buffer + total_len), message_len + 1, "%s", message);

            total_len += message_len;

            ESP_LOGD(TAG, "Wrote '%s', total len %u", message, total_len);

        } else {
            // TODO: implement something similar to pagination
            ESP_LOGW(TAG, "Not all files fit in one read request");
            break;
        }
    }

    return total_len;
}

}  // namespace bk
