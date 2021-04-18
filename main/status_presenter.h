#pragma once

#include "display.h"
#include "page_presenter.h"
#include "status_view.h"

namespace bk {
class StatusPresenter : public PagePresenter {
 public:
    StatusPresenter(IDisplay* display);
    ~StatusPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

 private:
    StatusView view_;
};

}  // namespace bk
