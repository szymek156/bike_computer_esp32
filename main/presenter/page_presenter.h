#pragma once
#include "event_dispatcher.h"
// #include "root_window.h"
#include "sensor_data.h"

#include <memory>
#include <vector>

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
        more_ = {more};
    }

    void setMore(const std::vector<PresenterPtr> &more) {
        // TODO: there should be move, investigate if intuition is correct
        more_ = more;
    }

    void setLess(PresenterPtr less) {
        less_ = less;
    }

    PresenterPtr getWidget(const WidgetData &widget) {
        switch (widget.new_widget) {
            case WidgetData::next:
                return next_;
            case WidgetData::prev:
                return prev_;
            case WidgetData::more: {
                if (widget.widget_idx < more_.size()) {
                    return more_[widget.widget_idx];
                }
                break;
            }
            case WidgetData::less:
                return less_;
            default:
                return nullptr;
        }

        // Control reaches end of non void function
        // Yeah whatever, there is a DEFAULT switch case defined you MORON!
        return nullptr;
    }

 protected:
    PresenterPtr next_;
    PresenterPtr prev_;
    // There can be more than one "more" widgets, depending on the user selection
    std::vector<PresenterPtr> more_;
    PresenterPtr less_;

    IEventDispatcher *events_;
};
}  // namespace bk
