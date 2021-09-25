#pragma once

#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/test_view.h"
#include "root_window.h"
#include <display.h>

namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class TestPresenter : public PagePresenter,
                         public GNSSListener,
                         public WeatherListener,
                         public TimeListener,
                         public KeypadListener {
 public:
    TestPresenter(IDisplay *display, IEventDispatcher *events);
    ~TestPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onGNSSData(const GNSSData &data) override;
    virtual void onWeatherData(const WeatherData &data) override;
    virtual void onTimeData(const TimeData &data) override;
    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    TestView view_;
};

}  // namespace bk
