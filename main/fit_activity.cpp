#include "fit_activity.h"

// I decided to use C API instead of C++ API for following reasons:
// C API is extremely small, moreover, it is possible to fine tune
// exactly which messages, and even which fields will be part of the
// compilation - currently, that's not the case for C++.
// Unfortunately it's also very low-level, every single step of the
// protocol must be defined by developer, and by C nature itself,
// it's very repetitive.
// C++ is more abstracted, and looks very nice to use, but it
// brings a lot of code that would be not used in this application.
// There is no way to define which messages are included and which not.
// It has whole machinery for publisher/subscriber pattern for message
// decoding.
// All of that could be reduced in size, but by hand.
// Currently code generation from .csv file works only for C API...
// As long as I will not be insanely pissed off by C API,
// I will stick to it. In case if my angrymeter explodes, switching
// to the C++ API sounds like a plan B, then as an incremental step
// is to cut unused part of the code manually.
// Example encode binary from C API is 39kB
// Example encode binary from C++ API if 2.9 MB - that may not fit
// on the flash!
// Also I need to save some space in FLASH and RAM for:
// - Bluetooth stack
// - ANT driver
// - Activities, other FIT files
// - Other libraries (hopefully there should be none?), maybe timezone as
// a stretch goal
#include "fit.h"
#include "fit_example.h"
#include "stdio.h"

#include <cstring>

namespace bk {

// This macro hides C API boilerplate. Repetitive stuff that
// is needed to create every single record in the FIT file.
// Firstly it initializes a message struct with default (invalid) data,
// Then "calls" user defined initialization,
// Finally, writes mesg definition and a message itself to the file.
// Lambdas uses operator () which is implicitly inlined,
// meaning there should be no overhead for calling it.

// TODO: try to find FIT_MESG_##NAME in some dict {FIT_NAME -> LOCAL_MESG_NR}
// if returns none, set local mesg to 0 and write definition
// else don't add definition just a message
// TODO: set most common messages to local_message = 1..15, 0 keep for one time garbage
#define ADD_MESSAGE(NAME, LOCAL_MESG_NUMBER, LAMBDA)                                  \
    {                                                                                 \
        FIT_##NAME##_MESG the_mesg;                                                   \
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_##NAME], &the_mesg);                      \
        [&] LAMBDA();                                                                 \
        fit_file_.writeMessage(LOCAL_MESG_NUMBER, &the_mesg, FIT_##NAME##_MESG_SIZE); \
    }

#define ADD_MESSAGE_DEF(NAME, LOCAL_MESG_NUMBER)                                            \
    {                                                                                       \
        fit_file_.writeMessageDefinition(                                                   \
            LOCAL_MESG_NUMBER, fit_mesg_defs[FIT_MESG_##NAME], FIT_##NAME##_MESG_DEF_SIZE); \
    }

FITActivity::FITActivity() {
    static const time_t systemTimeOffset =
        631065600;  // Needed for conversion from UNIX time to FIT time

    // this->timeStamp = (FIT_DATE_TIME) (timeStamp - systemTimeOffset);
    //    this->fractionalTimeStamp = 0.0;

    FIT_UINT8 local_mesg_number = 0;
    FIT_FILE_ID_MESG file_id;
    Fit_InitMesg(fit_mesg_defs[FIT_MESG_FILE_ID], &file_id);

    ADD_MESSAGE(FILE_ID, 0, { the_mesg.type = FIT_FILE_ACTIVITY; });
}

void FITActivity::onGNSSData(const GNSSData &data) {
    // TODO: accumulate distance
    // create record message and write it
}

}  // namespace bk