#pragma once
#include "sensor_data.h"

#include <memory>

namespace bk {
class PagePresenter;
using PresenterPtr = std::unique_ptr<PagePresenter>;

class PagePresenter {
 public:
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

    PresenterPtr next_;
    PresenterPtr prev_;
    PresenterPtr more_;
    PresenterPtr less_;
};
}  // namespace bk
