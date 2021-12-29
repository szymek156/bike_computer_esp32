#include "ble_wrapper.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <cstring>

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatt_common_api.h>
#include <esp_gatts_api.h>
#include <esp_log.h>
// #include <nvs_flash.h>
#include "file_transfer_gatts.h"

// TODO: Remove all that static crap, UGHHHH!
namespace bk {
static constexpr const char *TAG = "BT";
static uint8_t adv_config_done = 0;

#define SAMPLE_DEVICE_NAME "BK_GATTS"
// No idea what is it
#define ESP_APP_ID 0x55

static FileTransferGATTS gatts_;

// TODO: let it be some meaningful value
static uint8_t service_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    // first uuid, 16bit, [12],[13] is the value
    0xfb,
    0x34,
    0x9b,
    0x5f,
    0x80,
    0x00,
    0x00,
    0x80,
    0x00,
    0x10,
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00,
};

// TODO: this is some weird bit state machine, refactor
#define ADV_CONFIG_FLAG (1 << 0)
#define SCAN_RSP_CONFIG_FLAG (1 << 1)

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/* The length of adv data must be less than 31 bytes */
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,  // slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010,  // slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0,        // TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL,  // test_manufacturer,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(service_uuid),
    .p_service_uuid = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,        // TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL,  //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(service_uuid),
    .p_service_uuid = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    ESP_LOGI(TAG, "gap event %x", event);
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~ADV_CONFIG_FLAG);
            if (adv_config_done == 0) {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
            if (adv_config_done == 0) {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            /* advertising start complete event to indicate advertising start successfully or failed
             */
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "advertising start failed");
            } else {
                ESP_LOGI(TAG, "advertising start successfully");
            }
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "Advertising stop failed");
            } else {
                ESP_LOGI(TAG, "Stop adv successfully\n");
            }
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(TAG,
                     "update connection params status = %d, min_int = %d, max_int = %d,conn_int = "
                     "%d,latency = %d, timeout = %d",
                     param->update_conn_params.status,
                     param->update_conn_params.min_int,
                     param->update_conn_params.max_int,
                     param->update_conn_params.conn_int,
                     param->update_conn_params.latency,
                     param->update_conn_params.timeout);
            break;
        default:
            break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event,
                                esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param) {
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gatts.html#enumerations

    // Param is an union, different struct, depending on the event type
    // /**
    //  * @brief ESP_GATTS_REG_EVT
    //  */
    // struct gatts_reg_evt_param {
    //     esp_gatt_status_t status; /*!< Operation status */
    //     uint16_t app_id;          /*!< Application id which input in register API */
    // } reg;

    /* If event is register event, store the gatts_if for each profile */
    ESP_LOGI(TAG, "gatts event %x gatts_if %u", event, gatts_if);

    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            // There can be more than one application, that can be distinguished by app_id in param
            gatts_.gatts_if = gatts_if;
        } else {
            ESP_LOGE(TAG,
                     "reg app failed, app_id %04x, status %d",
                     param->reg.app_id,
                     param->reg.status);
            return;
        }
    }

    /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb
    function */
    if (gatts_if == ESP_GATT_IF_NONE || gatts_if == gatts_.gatts_if) {
        switch (event) {
            // Handle register event by GAP
            case ESP_GATTS_REG_EVT: {
                ESP_ERROR_CHECK(esp_ble_gap_set_device_name(SAMPLE_DEVICE_NAME));

                // Set advertisement configuration
                ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&adv_data));

                adv_config_done |= ADV_CONFIG_FLAG;
                // Set scan response data
                ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&scan_rsp_data));

                adv_config_done |= SCAN_RSP_CONFIG_FLAG;

                // Register GATT table, SVC_INST_ID - is service instance, can be more than
                // one
                ESP_ERROR_CHECK(esp_ble_gatts_create_attr_tab(
                    gatts_.gatt_db, gatts_if, ATT_IDX_END, SVC_INST_ID));

            } break;
            case ESP_GATTS_DISCONNECT_EVT:
                ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
                esp_ble_gap_start_advertising(&adv_params);
                break;
            default:
                // Other events go to GATTS service
                gatts_.gatts_profile_event_handler(event, gatts_if, param);
        }
    }
}

void BLEWrapper::enable() {
    ESP_LOGI(TAG, "Enabling BT...");
    // esp_err_t ret;

    // /* Initialize NVS. */
    // ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(ret);

    // If the app calls esp_bt_controller_enable(ESP_BT_MODE_BLE) to use BLE only then it is safe to
    // call esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT) at initialization time to free
    // unused BT Classic memory.
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    // Allocates BT task and stuff, should be called exactly once
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));

    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    // Initialize BLE SW stack
    ESP_ERROR_CHECK(esp_bluedroid_init());

    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_event_handler));

    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));

    // That triggers gatts event ESP_GATTS_REG_EVT,
    // You may want to register more than one application?
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(ESP_APP_ID));

    ESP_ERROR_CHECK(esp_ble_gatt_set_local_mtu(500));

    ESP_LOGI(TAG, "BT enabled.");
}

void BLEWrapper::disable() {
}

void BLEWrapper::test_indicate() {
    gatts_.test_indicate();
}
}  // namespace bk