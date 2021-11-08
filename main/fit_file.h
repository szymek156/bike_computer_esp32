#pragma once

#include <cstdio>

#include <fit.h>

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

    /** @brief Sets if file should be saved correctly, or discarded at the very end of object usage
     */
    void setDiscard(bool to_discard);

 private:
    static constexpr const char *TAG = "FITFile";
    FIT_UINT16 data_crc_;
    FILE *fp_;
    bool discard_;

    void writeFileHeader();
    void writeData(const void *data, size_t data_size);
};

}  // namespace bk