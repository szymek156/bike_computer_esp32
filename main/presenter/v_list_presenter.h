#pragma once

#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "root_window.h"
#include "view/v_list_view.h"

#include <string>
#include <vector>

#include <display.h>

namespace bk {
class VListPresenter {
 public:
    VListPresenter(IDisplay *display,
                   const std::vector<std::string> &elements);
    ~VListPresenter();

 private:
    VListView view_;
    std::vector<std::string> elements_;
};

}  // namespace bk
