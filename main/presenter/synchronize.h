#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "widgets/v_list_widget.h"
#include <display.h>

namespace bk {
class SynchronizeView {
 public:
    SynchronizeView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

    void drawBluetoothStatus(const BLEStatusData &data);

   VListWidget& getBleStatusList();
 protected:
    IDisplay *display_;
    // Keep status in VListWidget - get centering for free!
    VListWidget ble_status_;
};

class SynchronizePresenter : public PagePresenter, public KeypadListener, public BluetoothEventListener {
 public:
    SynchronizePresenter(IDisplay *display, IEventDispatcher *events);
    ~SynchronizePresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;
    virtual void onBluetoothChange(const BLEStatusData &data) override;


 private:
    SynchronizeView view_;
};

}  // namespace bk
