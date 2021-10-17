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
#include "fit_crc.h"
#include "fit_example.h"
#include "stdio.h"

namespace bk {
// TODO: well, fix it
static FIT_UINT16 data_crc;

// It's hard to believe that's not part of the API, but, well, it's not.
// Makes my pretty code to look like sh*t.
// TODO: move to some separate file, or something
void WriteData(const void *data, FIT_UINT8 data_size, FILE *fp) {
    FIT_UINT8 offset;

    fwrite(data, 1, data_size, fp);

    for (offset = 0; offset < data_size; offset++)
        data_crc = FitCRC_Get16(data_crc, *((FIT_UINT8 *)data + offset));
}

void WriteMessageDefinition(FIT_UINT8 local_mesg_number,
                            const void *mesg_def_pointer,
                            FIT_UINT8 mesg_def_size,
                            FILE *fp) {
    FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT;
    WriteData(&header, FIT_HDR_SIZE, fp);
    WriteData(mesg_def_pointer, mesg_def_size, fp);
}

void WriteMessage(FIT_UINT8 local_mesg_number,
                  const void *mesg_pointer,
                  FIT_UINT8 mesg_size,
                  FILE *fp) {
    WriteData(&local_mesg_number, FIT_HDR_SIZE, fp);
    WriteData(mesg_pointer, mesg_size, fp);
}

void WriteFileHeader(FILE *fp) {
    FIT_FILE_HDR file_header;

    file_header.header_size = FIT_FILE_HDR_SIZE;
    file_header.profile_version = FIT_PROFILE_VERSION;
    file_header.protocol_version = FIT_PROTOCOL_VERSION_20;
    memcpy((FIT_UINT8 *)&file_header.data_type, ".FIT", 4);
    fseek(fp, 0, SEEK_END);
    file_header.data_size = ftell(fp) - FIT_FILE_HDR_SIZE - sizeof(FIT_UINT16);
    file_header.crc = FitCRC_Calc16(&file_header, FIT_STRUCT_OFFSET(crc, FIT_FILE_HDR));

    fseek(fp, 0, SEEK_SET);
    fwrite((void *)&file_header, 1, FIT_FILE_HDR_SIZE, fp);
}

// This macro hides C API boilerplate. Repetitive stuff that
// is needed to create every single record in the FIT file.
// Firstly it initializes a message struct with default (invalid) data,
// Then "calls" user defined initialization,
// Finally, writes mesg definition and a message itself to the file.
// Lambdas uses operator () which is implicitly inlined,
// meaning there should be no overhead for calling it.
#define ADD_MESSAGE(NAME, LOCAL_MESG_NUMBER, LAMBDA)                                            \
    {                                                                                           \
        FIT_##NAME##_MESG the_mesg;                                                             \
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_##NAME], &the_mesg);                                \
        [&] LAMBDA();                                                                           \
                                                                                                \
        /* TODO: add definition only if needed */                                               \
        WriteMessageDefinition(                                                                 \
            LOCAL_MESG_NUMBER, fit_mesg_defs[FIT_MESG_##NAME], FIT_##NAME##_MESG_DEF_SIZE, fp); \
        WriteMessage(LOCAL_MESG_NUMBER, &the_mesg, FIT_##NAME##_MESG_SIZE, fp);                 \
    }

FITActivity::FITActivity() {
    static const time_t systemTimeOffset =
        631065600;  // Needed for conversion from UNIX time to FIT time

    // this->timeStamp = (FIT_DATE_TIME) (timeStamp - systemTimeOffset);
    //    this->fractionalTimeStamp = 0.0;

    FIT_UINT8 local_mesg_number = 0;
    FIT_FILE_ID_MESG file_id;
    Fit_InitMesg(fit_mesg_defs[FIT_MESG_FILE_ID], &file_id);

    FILE *fp = fopen("test.fit", "w+b");

    WriteFileHeader(fp);

    ADD_MESSAGE(FILE_ID, 0, { the_mesg.type = FIT_FILE_ACTIVITY; });

    // Update the header, must be last step!
    WriteFileHeader(fp);

    fclose(fp);
}
}  // namespace bk