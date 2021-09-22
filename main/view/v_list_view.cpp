#include "v_list_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

VListView::VListView(IDisplay *display, const std::vector<std::string> &elements)
    : display_(display),
      elements_(elements) {
}

void VListView::drawStatic() {
}

}  // namespace bk
