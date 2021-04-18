#pragma once
#include "sensor_data.h"

namespace bk {
class PagePresenter {
 public:
    virtual ~PagePresenter() = default;

    virtual void onEnter() = 0;

    virtual void onLeave() = 0;
};
}  // namespace bk
