#pragma once

#include <display.h>
#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/welcome_view.h"

namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class WelcomePresenter : public IPagePresenter, public WeatherListener, public TimeListener {
 public:
    WelcomePresenter(IDisplay *display, IEventDispatcher *events);
    ~WelcomePresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onWeatherData(const WeatherData &data) override;
    virtual void onTimeData(const TimeData &data) override;

 private:
    WelcomeView view_;
    IEventDispatcher *events_;
};

}  // namespace bk
