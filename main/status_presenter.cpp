#include "status_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

StatusPresenter::StatusPresenter(IDisplay *display) : view_(StatusView(display)) {
}

StatusPresenter::~StatusPresenter() {
}

void StatusPresenter::onEnter() {
}

void StatusPresenter::onLeave() {
}

}  // namespace bk
