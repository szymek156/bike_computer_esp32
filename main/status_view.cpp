#include "status_view.h"

namespace bk {
using bk::COLORED;

StatusView::StatusView(IDisplay *display) : display_(display), paint_(display_->getPaint()) {
}

void StatusView::drawGNSSData(const GNSSData &data) {
    const int msg_size = 128;
    char message[msg_size];

    snprintf(message, msg_size, "SPD %5.2f km/h", data.speed_kmh);
    paint_.DrawStringAt(0, 32, message, &Font20, COLORED);

    snprintf(message, msg_size, "ALT %7.2f mnpm", data.altitude);
    paint_.DrawStringAt(0, 54, message, &Font16, COLORED);

    snprintf(message, msg_size, "SAT's in view  %2d", data.sats_in_view);
    paint_.DrawStringAt(170, 95, message, &Font12, COLORED);

    snprintf(message, msg_size, "SAT's tracked  %2d", data.sats_tracked);
    paint_.DrawStringAt(170, 105, message, &Font12, COLORED);

    snprintf(message, msg_size, "GPS fix status %2d", data.fix_status);
    paint_.DrawStringAt(170, 115, message, &Font12, COLORED);
}

void StatusView::drawDateTime(const GNSSData &data) {
    const int msg_size = 128;
    char message[msg_size];

    strftime(message, msg_size, "%D %T", &(data.date_time));
    paint_.DrawStringAt(0, 4, message, &Font24, COLORED);
}
}  // namespace bk
