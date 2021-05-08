#pragma once
#include "event_dispatcher.h"
// #include "root_window.h"
#include "sensor_data.h"

#include <memory>

namespace bk {
class RootWindow;
class PagePresenter;
using PresenterPtr = std::shared_ptr<PagePresenter>;

class PagePresenter {
 public:
    PagePresenter(IEventDispatcher *events, RootWindow *root) : events_(events), root_(root) {
    }

    virtual ~PagePresenter() = default;

    virtual void onEnter() = 0;

    virtual void onLeave() = 0;

    void setNext(PresenterPtr next) {
        next_ = std::move(next);
    }

    void setPrevious(PresenterPtr prev) {
        prev_ = std::move(prev);
    }

    void setMore(PresenterPtr more) {
        more_ = std::move(more);
    }

    void setLess(PresenterPtr less) {
        less_ = std::move(less);
    }

 protected:
    PresenterPtr next_;
    PresenterPtr prev_;
    PresenterPtr more_;
    PresenterPtr less_;

    IEventDispatcher *events_;
    RootWindow *root_;
};
}  // namespace bk
