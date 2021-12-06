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

namespace bk {
static constexpr const char *TAG = "BT";
static uint8_t adv_config_done = 0;

// No idea what is it
#define ESP_APP_ID 0x55
#define SVC_INST_ID 0

// TODO: this is some weird bit state machine, refactor
#define ADV_CONFIG_FLAG (1 << 0)
#define SCAN_RSP_CONFIG_FLAG (1 << 1)
#define PROFILE_NUM 1
#define PROFILE_APP_IDX 0
#define SAMPLE_DEVICE_NAME "BK_GATTS"
/* Attributes State Machine */
enum {
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    IDX_CHAR_B,
    IDX_CHAR_VAL_B,

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,

    HRS_IDX_NB,
};

/* The max length of characteristic value. When the GATT client performs a write or prepare write
 * operation, the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
 */
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE 1024
#define CHAR_DECLARATION_SIZE (sizeof(uint8_t))

/* Service */
static const uint16_t GATTS_SERVICE_UUID_TEST = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A = 0xFF01;
static const uint16_t GATTS_CHAR_UUID_TEST_B = 0xFF02;
static const uint16_t GATTS_CHAR_UUID_TEST_C = 0xFF03;

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_read_write_notify =
    ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t heart_measurement_ccc[2] = {0x00, 0x00};
static const uint8_t char_value[4] = {0x11, 0x22, 0x33, 0x44};

uint16_t heart_rate_handle_table[HRS_IDX_NB];

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

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

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param);

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned
 * by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM] = {
    {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] = {
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
                     (uint8_t *)&char_prop_read_write_notify}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_A] = {{ESP_GATT_AUTO_RSP},
                        {ESP_UUID_LEN_16,
                         (uint8_t *)&GATTS_CHAR_UUID_TEST_A,
                         ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                         GATTS_DEMO_CHAR_VAL_LEN_MAX,
                         sizeof(char_value),
                         (uint8_t *)char_value}},

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CFG_A] = {{ESP_GATT_AUTO_RSP},
                        {ESP_UUID_LEN_16,
                         (uint8_t *)&character_client_config_uuid,
                         ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                         sizeof(uint16_t),
                         sizeof(heart_measurement_ccc),
                         (uint8_t *)heart_measurement_ccc}},

    /* Characteristic Declaration */
    [IDX_CHAR_B] = {{ESP_GATT_AUTO_RSP},
                    {ESP_UUID_LEN_16,
                     (uint8_t *)&character_declaration_uuid,
                     ESP_GATT_PERM_READ,
                     CHAR_DECLARATION_SIZE,
                     CHAR_DECLARATION_SIZE,
                     (uint8_t *)&char_prop_read}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_B] = {{ESP_GATT_AUTO_RSP},
                        {ESP_UUID_LEN_16,
                         (uint8_t *)&GATTS_CHAR_UUID_TEST_B,
                         ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                         GATTS_DEMO_CHAR_VAL_LEN_MAX,
                         sizeof(char_value),
                         (uint8_t *)char_value}},

    /* Characteristic Declaration */
    [IDX_CHAR_C] = {{ESP_GATT_AUTO_RSP},
                    {ESP_UUID_LEN_16,
                     (uint8_t *)&character_declaration_uuid,
                     ESP_GATT_PERM_READ,
                     CHAR_DECLARATION_SIZE,
                     CHAR_DECLARATION_SIZE,
                     (uint8_t *)&char_prop_write}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_C] = {{ESP_GATT_AUTO_RSP},
                        {ESP_UUID_LEN_16,
                         (uint8_t *)&GATTS_CHAR_UUID_TEST_C,
                         ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                         GATTS_DEMO_CHAR_VAL_LEN_MAX,
                         sizeof(char_value),
                         (uint8_t *)char_value}},

};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param) {
    ESP_LOGI(TAG, "gatts profile event %x", event);
    switch (event) {
        case ESP_GATTS_REG_EVT: {
            ESP_ERROR_CHECK(esp_ble_gap_set_device_name(SAMPLE_DEVICE_NAME));

            // Set advertisement configuration
            ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&adv_data));

            adv_config_done |= ADV_CONFIG_FLAG;
            // Set scan response data
            ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&scan_rsp_data));

            adv_config_done |= SCAN_RSP_CONFIG_FLAG;

            // Register GATT table, SVC_INST_ID - is service instance, can be more than one
            ESP_ERROR_CHECK(
                esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID));

        } break;
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_READ_EVT");
            break;
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
            ESP_LOGI(TAG,
                     "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d",
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
            esp_log_buffer_hex(TAG, param->connect.remote_bda, 6);
            esp_ble_conn_update_params_t conn_params = {};
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            /* For the iOS system, please refer to Apple official documents about the BLE connection
             * parameters restrictions. */
            conn_params.latency = 0;
            conn_params.max_int = 0x20;  // max_int = 0x20*1.25ms = 40ms
            conn_params.min_int = 0x10;  // min_int = 0x10*1.25ms = 20ms
            conn_params.timeout = 400;   // timeout = 400*10ms = 4000ms
            // start sent the update connection parameters to the peer device.
            esp_ble_gap_update_conn_params(&conn_params);
            break;
        }
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
            esp_ble_gap_start_advertising(&adv_params);
            break;
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
        case ESP_GATTS_STOP_EVT:
        case ESP_GATTS_OPEN_EVT:
        case ESP_GATTS_CANCEL_OPEN_EVT:
        case ESP_GATTS_CLOSE_EVT:
        case ESP_GATTS_LISTEN_EVT:
        case ESP_GATTS_CONGEST_EVT:
        case ESP_GATTS_UNREG_EVT:
        case ESP_GATTS_DELETE_EVT:
        default:
            break;
    }
}

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
            heart_rate_profile_tab[PROFILE_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGE(TAG,
                     "reg app failed, app_id %04x, status %d",
                     param->reg.app_id,
                     param->reg.status);
            return;
        }
    }

    for (int idx = 0; idx < PROFILE_NUM; idx++) {
        /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb
        function */
        if (gatts_if == ESP_GATT_IF_NONE || gatts_if == heart_rate_profile_tab[idx].gatts_if) {
            if (heart_rate_profile_tab[idx].gatts_cb) {
                heart_rate_profile_tab[idx].gatts_cb(event, gatts_if, param);
            }
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

}  // namespace bk