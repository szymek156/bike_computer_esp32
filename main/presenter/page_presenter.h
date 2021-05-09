#pragma once
#include "event_dispatcher.h"
// #include "root_window.h"
#include "sensor_data.h"

#include <memory>

namespace bk {
class PagePresenter;
using PresenterPtr = std::shared_ptr<PagePresenter>;

class PagePresenter {
 public:
    PagePresenter(IEventDispatcher *events) : events_(events) {
    }

    virtual ~PagePresenter() = default;

    virtual void onEnter() = 0;

    virtual void onLeave() = 0;

    void setNext(PresenterPtr next) {
        next_ = next;
    }

    void setPrevious(PresenterPtr prev) {
        prev_ = prev;
    }

    void setMore(PresenterPtr more) {
        more_ = more;
    }

    void setLess(PresenterPtr less) {
        less_ = less;
    }

    PresenterPtr getWidget(WidgetData::Widget widget) {
        switch (widget) {
            case WidgetData::next:
                return next_;
            case WidgetData::prev:
                return prev_;
            case WidgetData::more:
                return more_;
            case WidgetData::less:
                return less_;
        }

        return nullptr;
    }

 protected:
    PresenterPtr next_;
    PresenterPtr prev_;
    PresenterPtr more_;
    PresenterPtr less_;

    IEventDispatcher *events_;
};
}  // namespace bk
