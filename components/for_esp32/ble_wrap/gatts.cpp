#include "gatts.h"

#include <cstring>

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatt_common_api.h>
#include <esp_gatts_api.h>
#include <esp_log.h>

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
static const uint16_t GATTS_SERVICE_UUID_TEST = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A = 0xFF01;
static const uint16_t GATTS_CHAR_UUID_TEST_B = 0xFF02;

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t my_character_declaration_uuid =
    ESP_GATT_UUID_CHAR_CLIENT_CONFIG | ESP_GATT_UUID_CHAR_PRESENT_FORMAT;

static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_read_indicate =
    ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_INDICATE;

static const uint8_t char_prop_read_write =
    ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_indicate = ESP_GATT_CHAR_PROP_BIT_INDICATE;
static const uint8_t heart_measurement_ccc[2] = {0x00, 0x00};
static const uint8_t char_value[ESP_GATT_MAX_ATTR_LEN] = {};

// Keeps handles to bluetooth characteristic, is later used by:
// esp_ble_gatts_set_attr_value
uint16_t heart_rate_handle_table[HRS_IDX_NB];

/* The max length of characteristic value. When the GATT client performs a write or prepare write
 * operation, the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
 */
#define GATTS_DEMO_CHAR_VAL_LEN_MAX ESP_GATT_MAX_ATTR_LEN
#define PREPARE_BUF_MAX_SIZE 1024
#define CHAR_DECLARATION_SIZE (sizeof(uint8_t))

/* Full Database Description - Used to add attributes into the database */
const esp_gatts_attr_db_t GATTS::gatt_db[HRS_IDX_NB] = {
    // Service Declaration
    [IDX_SVC] = {{ESP_GATT_AUTO_RSP},
                 {ESP_UUID_LEN_16,
                  (uint8_t *)&primary_service_uuid,
                  ESP_GATT_PERM_READ,
                  sizeof(uint16_t),
                  sizeof(GATTS_SERVICE_UUID_TEST),
                  (uint8_t *)&GATTS_SERVICE_UUID_TEST}},

    /* Characteristic Declaration */
    [IDX_CHAR_A] = {{ESP_GATT_AUTO_RSP},
                    {ESP_UUID_LEN_16,
                     (uint8_t *)&character_declaration_uuid,
                     ESP_GATT_PERM_READ,
                     CHAR_DECLARATION_SIZE,
                     CHAR_DECLARATION_SIZE,
                     (uint8_t *)&char_prop_indicate}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_A] = {{ESP_GATT_RSP_BY_APP},
                        {ESP_UUID_LEN_16,
                         (uint8_t *)&GATTS_CHAR_UUID_TEST_A,
                         ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                         GATTS_DEMO_CHAR_VAL_LEN_MAX,
                         sizeof(char_value),
                         (uint8_t *)char_value}},

    /* Client Characteristic Configuration Descriptor */
    // Configuration is needed to be able to sub or unsub for indications
    // Set to ESP_GATT_AUTO_RSP, so I don't have to worry about handling those requests
    [IDX_CHAR_CFG_A] = {{ESP_GATT_AUTO_RSP},
                        {ESP_UUID_LEN_16,
                         (uint8_t *)&character_client_config_uuid,
                         ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                         GATTS_DEMO_CHAR_VAL_LEN_MAX,
                         sizeof(char_value),
                         (uint8_t *)char_value}},
    //  sizeof(uint16_t),
    //  sizeof(heart_measurement_ccc),
    //  (uint8_t *)heart_measurement_ccc}},
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /* Characteristic Declaration */
    [IDX_CHAR_B] = {{ESP_GATT_AUTO_RSP},
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
    [IDX_CHAR_VAL_B] =
        {{ESP_GATT_RSP_BY_APP},
         {ESP_UUID_LEN_16,
          (uint8_t *)&GATTS_CHAR_UUID_TEST_B,

          // Those are actual characteristic permission, I have no idea why it's duplicated
          // (refer to (uint8_t *)&char_prop_read_write) in characteristic declaration
          ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
          GATTS_DEMO_CHAR_VAL_LEN_MAX,
          sizeof(char_value),
          (uint8_t *)char_value}},
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param);

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned
 * by ESP_GATTS_REG_EVT */
GATTS::gatts_profile_inst GATTS::heart_rate_profile_tab[PROFILE_NUM] = {
    {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

static uint8_t value_to_send[ESP_GATT_MAX_ATTR_LEN] = {};

void GATTS::test_indicate() {
    ESP_LOGI(TAG, "Setting the value for indication");

    static int COUNTER = 0;

    COUNTER++;
    memset(value_to_send, COUNTER, ESP_GATT_MAX_ATTR_LEN);

    esp_ble_gatts_set_attr_value(
        heart_rate_handle_table[IDX_CHAR_VAL_A], ESP_GATT_MAX_ATTR_LEN, value_to_send);
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param) {
    ESP_LOGI(TAG, "gatts profile event %u decimal", event);
    switch (event) {
        case ESP_GATTS_READ_EVT: {
            ESP_LOGI(TAG,
                     "ESP_GATTS_READ_EVT conn id %u, trans id %u, attr handle %u, offset %u, is "
                     "long %u need resp %u",
                     param->read.conn_id,
                     param->read.trans_id,
                     param->read.handle,
                     param->read.offset,
                     param->read.is_long,
                     param->read.need_rsp);

            // If characteristic in the table is set to auto response, this will be false
            if (!param->read.need_rsp)
                break;  // For some reason you can request a read but not want a response

            esp_gatt_rsp_t response = {};

            response.attr_value.len = 22;
            response.attr_value.offset = param->read.offset;
            response.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            response.attr_value.handle = param->read.handle;

            for (int i = 0; i < 22; i++) {
                response.attr_value.value[i] = param->read.offset;
            }

            ESP_ERROR_CHECK(esp_ble_gatts_send_response(
                gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &response));

            //   if (param->read.handle != this->handle_)
            //     break;  // Not this characteristic

            //   if (!param->read.need_rsp)
            //     break;  // For some reason you can request a read but not want a response

            //   uint16_t max_offset = 22;

            //   esp_gatt_rsp_t response;
            //   if (param->read.is_long) {
            //     if (this->value_.size() - this->value_read_offset_ < max_offset) {
            //       //  Last message in the chain
            //       response.attr_value.len = this->value_.size() - this->value_read_offset_;
            //       response.attr_value.offset = this->value_read_offset_;
            //       memcpy(response.attr_value.value, this->value_.data() +
            //       response.attr_value.offset, response.attr_value.len); this->value_read_offset_
            //       = 0;
            //     } else {
            //       response.attr_value.len = max_offset;
            //       response.attr_value.offset = this->value_read_offset_;
            //       memcpy(response.attr_value.value, this->value_.data() +
            //       response.attr_value.offset, response.attr_value.len);
            //       this->value_read_offset_ += max_offset;
            //     }
            //   } else {
            //     response.attr_value.offset = 0;
            //     if (this->value_.size() + 1 > max_offset) {
            //       response.attr_value.len = max_offset;
            //       this->value_read_offset_ = max_offset;
            //     } else {
            //       response.attr_value.len = this->value_.size();
            //     }
            //     memcpy(response.attr_value.value, this->value_.data(), response.attr_value.len);
            //   }

            //   response.attr_value.handle = this->handle_;
            //   response.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;

            //   esp_err_t err =
            //       esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
            //       param->read.trans_id, ESP_GATT_OK, &response);
            //   if (err != ESP_OK) {
            //     ESP_LOGE(TAG, "esp_ble_gatts_send_response failed: %d", err);
            //   }
            //   break;
            break;
        }

        case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_WRITE_EVT");
            break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            // the length of gattc prepare write data must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
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
            // conn_params.max_int = 0x20;  // max_int = 0x20*1.25ms = 40ms
            // conn_params.min_int = 0x10;  // min_int = 0x10*1.25ms = 20ms
            // conn_params.timeout = 400;   // timeout = 400*10ms = 4000ms
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
            } else if (param->add_attr_tab.num_handle != HRS_IDX_NB) {
                ESP_LOGE(TAG,
                         "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to HRS_IDX_NB(%d)",
                         param->add_attr_tab.num_handle,
                         HRS_IDX_NB);
            } else {
                ESP_LOGI(TAG,
                         "create attribute table successfully, the number handle = %d\n",
                         param->add_attr_tab.num_handle);
                memcpy(heart_rate_handle_table,
                       param->add_attr_tab.handles,
                       sizeof(heart_rate_handle_table));
                esp_ble_gatts_start_service(heart_rate_handle_table[IDX_SVC]);
            }
            break;
        }
        case ESP_GATTS_SET_ATTR_VAL_EVT: {
            ESP_LOGI(TAG, "Sending an indication");
            bool needs_confirmation = true;
            uint16_t handle = param->set_attr_val.attr_handle;

            esp_ble_gatts_send_indicate(
                gatts_if, param->reg.app_id, handle, 500, value_to_send, needs_confirmation);

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

}  // namespace bk
