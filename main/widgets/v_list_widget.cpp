#include "v_list_widget.h"

#include <paint.h>
namespace bk {

VListWidget::VListWidget(
    IDisplay *display, const sFONT &font, const Rect &area, size_t el_in_view, Alignment Alignment)
    : display_(display),
      font_(font),
      area_(area),
      center_vert_(0),
      current_selection_(0),
      el_in_view_(el_in_view),
      viewport_start_(0),
      viewport_size_(0),
      el_height_(0),
      el_width_(0),
      alignment_(Alignment) {
    updateElements({"Element 1", "Element 2"});
}

void VListWidget::drawStatic() {
    invalidateViewport();
}

void VListWidget::invalidateViewport() {
    display_->enqueueDraw(
        [&](Paint &paint) {
            for (size_t i = viewport_start_; i < viewport_start_ + viewport_size_; i++) {
                // TODO: Beware, drawNotSelected call enqueueDraw
                // It happens to work in drawStatic context
                // But that's an implementation detail I am relying
                // on now. If something will change in enqueueDraw,
                // this code will likely to be broken.
                drawNotSelected(i);
            }

            // Mark selection
            drawSelected(current_selection_);
        },
        area_);
}
void VListWidget::drawNotSelected(size_t selection) {
    int offset = (selection - viewport_start_) * el_height_;

    display_->enqueueDraw(
        [&](Paint &paint) {
            paint.DrawFilledRectangle(
                area_.x0, area_.y0 + offset, area_.x1, area_.y0 + offset + el_height_, UNCOLORED);

            paint.DrawStringAt(area_.x0 + computeAlignment(selection),
                               area_.y0 + offset + center_vert_,
                               elements_[selection].c_str(),
                               &font_,
                               COLORED);
        },
        {area_.x0, area_.y0 + offset, area_.x1, area_.y0 + offset + el_height_});
}

void VListWidget::drawSelected(size_t selection) {
    int offset = (selection - viewport_start_) * el_height_;
    // Select next one
    display_->enqueueDraw(
        [&](Paint &paint) {
            paint.DrawFilledRectangle(
                area_.x0, area_.y0 + offset, area_.x1, area_.y0 + offset + el_height_, COLORED);

            paint.DrawStringAt(area_.x0 + computeAlignment(selection),
                               area_.y0 + offset + center_vert_,
                               elements_[selection].c_str(),
                               &font_,
                               UNCOLORED);
        },
        {area_.x0, area_.y0 + offset, area_.x1, area_.y0 + offset + el_height_});
}

int VListWidget::computeAlignment(size_t selection) {
    switch (alignment_) {
        case Alignment::Centered:
            // Center each element horizontally according to it's string width,
            // or clamp to 0, if text does not fit to the cell.
            return std::max((el_width_ - (int)elements_[selection].size() * font_.Width) / 2, 0);

        case Alignment::Left:
        default:
            return 0;
    }
}
void VListWidget::goDown() {
    // Unselect current one
    drawNotSelected(current_selection_);

    // Move to the next element
    current_selection_ = (current_selection_ + 1) % elements_.size();

    if (current_selection_ == 0) {
        viewport_start_ = 0;
        invalidateViewport();
    } else if (current_selection_ >= viewport_start_ + viewport_size_) {
        viewport_start_++;
        invalidateViewport();
    }

    // Select that element
    drawSelected(current_selection_);
}

void VListWidget::goUp() {
    // Unselect current one
    drawNotSelected(current_selection_);

    // Move to the previous element
    current_selection_ = std::min(current_selection_ - 1u, elements_.size() - 1u);

    if (current_selection_ < viewport_start_) {
        viewport_start_--;
        invalidateViewport();
    } else if (current_selection_ == elements_.size() - 1u) {
        viewport_start_ = elements_.size() - viewport_size_;
        invalidateViewport();
    }

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

    viewport_size_ = std::min(el_in_view_, elements_.size());
    current_selection_ = 0;
    viewport_start_ = 0;
    el_height_ = (area_.y1 - area_.y0) / viewport_size_;
    el_width_ = area_.x1 - area_.x0;
    center_vert_ = (el_height_ - font_.Height) / 2;
}

}  // namespace bk
