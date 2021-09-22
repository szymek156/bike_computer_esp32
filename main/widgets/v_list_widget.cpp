#include "v_list_widget.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;
using bk::UNCOLORED;

VListWidget::VListWidget(IDisplay *display, const std::vector<std::string> &elements,
                          const Rect &area)
    : display_(display),
      area_(area),
      elements_(elements),
      el_height_((area_.y1 - area_.y0) / elements_.size()),
      current_selection_(0) {
}

void VListWidget::drawStatic() {
    display_->enqueueStaticDraw(
      [&](Paint &paint) {
          for (size_t i = 0; i < elements_.size(); i++) {
            auto offset = i * el_height_;
            paint.DrawStringAt(area_.x0, area_.y0 + offset,
              elements_[i].c_str(), &Font16, COLORED);
          }

          auto offset = current_selection_ * el_height_;
          paint.DrawFilledRectangle(area_.x0, area_.y0 + offset, area_.x1,
              area_.y0 + offset + el_height_, COLORED);

          paint.DrawStringAt(area_.x0, area_.y0 + offset,
              elements_[current_selection_].c_str(), &Font16, UNCOLORED);
      },
      area_);
}

void VListWidget::clearSelection() {
  int offset = current_selection_ * el_height_;

  display_->enqueueDraw(
      [&](Paint &paint) {
          paint.DrawFilledRectangle(area_.x0, area_.y0 + offset, area_.x1,
              area_.y0 + offset + el_height_, UNCOLORED);

          paint.DrawStringAt(area_.x0, area_.y0 + offset,
              elements_[current_selection_].c_str(), &Font16, COLORED);
  }, {area_.x0, area_.y0 + offset, area_.x1,
      area_.y0 + offset + el_height_});
}

void VListWidget::markSelection() {
  int offset = current_selection_ * el_height_;
  // Select next one
  display_->enqueueDraw(
      [&](Paint &paint) {
          paint.DrawFilledRectangle(area_.x0, area_.y0 + offset, area_.x1,
              area_.y0 + offset + el_height_, COLORED);

          paint.DrawStringAt(area_.x0, area_.y0 + offset,
              elements_[current_selection_].c_str(), &Font16, UNCOLORED);
  }, {area_.x0, area_.y0 + offset, area_.x1,
      area_.y0 + offset + el_height_});
}

void VListWidget::goDown() {
  // Unselect current one
  clearSelection();

  // Move to next element
  current_selection_ = (current_selection_ + 1) % elements_.size();

  // Select that element
  markSelection();
}

void VListWidget::goUp() {
  // Unselect current one
  clearSelection();

  current_selection_ = std::min(current_selection_ - 1u, elements_.size() - 1u);

  // Select that element
  markSelection();
}

std::string VListWidget::getCurrentSelection() {
  return elements_[current_selection_];
}

size_t VListWidget::getCurrentSelectionIdx() {
  return current_selection_;
}
}  // namespace bk
