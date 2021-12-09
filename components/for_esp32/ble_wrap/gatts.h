#pragma once
#include <esp_gatts_api.h>

namespace bk {
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

#define PROFILE_NUM 1
#define PROFILE_NUM 1
#define PROFILE_APP_IDX 0

class GATTS {
 public:
    static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB];

    struct gatts_profile_inst {
        esp_gatts_cb_t gatts_cb;
        uint16_t gatts_if;
    };

    static gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM];
};

}  // namespace bk
