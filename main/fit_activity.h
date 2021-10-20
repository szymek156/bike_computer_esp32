#pragma once
#include "fit_file.h"
#include "listerers_interface.h"

namespace bk {

class FITActivity : public GNSSListener {
 public:
    FITActivity();

    void onGNSSData(const GNSSData &data) override;

 private:
    FITFile fit_file_;
};
}  // namespace bk