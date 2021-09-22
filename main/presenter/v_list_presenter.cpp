#include "select_activity_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

VListPresenter::VListPresenter(IDisplay *display,
                               const std::vector<std::string> &elements)
    :
      view_(VListView(display, elements)),
      elements_(elements) {
}

VListPresenter::~VListPresenter() {
    ESP_LOGE("VListPresenter", "DTOR");
}
}  // namespace bk
