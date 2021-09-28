#include "v_list_widget.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;
using bk::UNCOLORED;

VListWidget::VListWidget(IDisplay *display, const sFONT &font, const Rect &area)
    : display_(display),
      elements_({"Element 1", "Element 2"}),
      font_(font),
      area_(area),
      el_height_((area_.y1 - area_.y0) / elements_.size()),
      el_width_(area_.x1 - area_.x0),
      center_vert_((el_height_ - font_.Height) / 2),
      current_selection_(0) {
}

void VListWidget::drawStatic() {
    display_->enqueueStaticDraw(
        [&](Paint &paint) {
            for (size_t i = 0; i < elements_.size(); i++) {
                // TODO: Beware, drawNotSelected call enqueueDraw
                // It happens to work in drawStatic context
                // But that's an implementation detail I am relying
                // on now. If something will change in enqueueStaticDraw,
                // this code will likely to be broken.
                drawNotSelected(i);
            }

            // Mark selection
            drawSelected(current_selection_);
        },
        area_);
}

void VListWidget::drawNotSelected(size_t selection) {
    int offset = selection * el_height_;

    display_->enqueueDraw(
        [&](Paint &paint) {
            paint.DrawFilledRectangle(
                area_.x0, area_.y0 + offset, area_.x1, area_.y0 + offset + el_height_, UNCOLORED);

            // Center each element horizontally according to it's string width,
            // or clamp to 0, if text does not fit to the cell.
            auto center_horizontal =
                std::max((el_width_ - (int)elements_[selection].size() * font_.Width) / 2, 0);

            paint.DrawStringAt(area_.x0 + center_horizontal,
                               area_.y0 + offset + center_vert_,
                               elements_[selection].c_str(),
                               &font_,
                               COLORED);
        },
        {area_.x0, area_.y0 + offset, area_.x1, area_.y0 + offset + el_height_});
}

void VListWidget::drawSelected(size_t selection) {
    int offset = selection * el_height_;
    // Select next one
    display_->enqueueDraw(
        [&](Paint &paint) {
            paint.DrawFilledRectangle(
                area_.x0, area_.y0 + offset, area_.x1, area_.y0 + offset + el_height_, COLORED);

            // Center each element horizontally according to it's string width,
            // or clamp to 0, if text does not fit to the cell.
            auto center_horizontal =
                std::max((el_width_ - (int)elements_[selection].size() * font_.Width) / 2, 0);

            paint.DrawStringAt(area_.x0 + center_horizontal,
                               area_.y0 + offset + center_vert_,
                               elements_[selection].c_str(),
                               &font_,
                               UNCOLORED);
        },
        {area_.x0, area_.y0 + offset, area_.x1, area_.y0 + offset + el_height_});
}

void VListWidget::goDown() {
    // Unselect current one
    drawNotSelected(current_selection_);

    // Move to the next element
    current_selection_ = (current_selection_ + 1) % elements_.size();

    // Select that element
    drawSelected(current_selection_);
}

void VListWidget::goUp() {
    // Unselect current one
    drawNotSelected(current_selection_);

    // Move to the previous element
    current_selection_ = std::min(current_selection_ - 1u, elements_.size() - 1u);

    // Select that element
    drawSelected(current_selection_);
}

std::string VListWidget::getCurrentSelection() {
    return elements_[current_selection_];
}

size_t VListWidget::getCurrentSelectionIdx() {
    return current_selection_;
}

void VListWidget::updateElements(const std::vector<std::string> elements) {
    elements_ = elements;
    current_selection_ = 0;
    el_height_ = (area_.y1 - area_.y0) / elements_.size();
    el_width_ = area_.x1 - area_.x0;
    center_vert_ = (el_height_ - font_.Height) / 2;
}

}  // namespace bk
