#include "fit_activity.h"

#include "fit.h"
#include "fit_example.h"
#include "stdio.h"

namespace bk {

FITActivity::FITActivity() {
    static const time_t systemTimeOffset = 631065600; // Needed for conversion from UNIX time to FIT time

    // this->timeStamp = (FIT_DATE_TIME) (timeStamp - systemTimeOffset);
    //    this->fractionalTimeStamp = 0.0;

    FIT_UINT8 local_mesg_number = 0;
    FIT_FILE_ID_MESG file_id;
    Fit_InitMesg(fit_mesg_defs[FIT_MESG_FILE_ID], &file_id);

    FILE *fp = fopen("test.fit", "w+b");
}
}  // namespace bk