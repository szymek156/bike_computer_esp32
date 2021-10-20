#pragma once

#include "cstdio"
#include "fit.h"

namespace bk {
/** @brief Abstraction over writing FIT data to the file */
class FITFile {
 public:
    FITFile();
    ~FITFile();

    void writeMessageDefinition(FIT_UINT8 local_mesg_number,
                                const void *mesg_def_pointer,
                                size_t mesg_def_size);

    void writeMessage(FIT_UINT8 local_mesg_number, const void *mesg_pointer, size_t mesg_size);

 private:
    FIT_UINT16 data_crc_;
    FILE *fp_;

    void writeFileHeader();
    void writeData(const void *data, size_t data_size);
};

}  // namespace bk