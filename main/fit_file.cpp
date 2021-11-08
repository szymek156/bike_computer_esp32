#include "fit_file.h"

#include "fit_crc.h"
#include "fs_wrapper.h"

#include <cstring>

#include <esp_log.h>
namespace bk {

// TODO: come up with fancier name
static const char *FILE_PATH = "/storage/test_walk.fit";

FITFile::FITFile() : discard_(false) {
    fp_ = fopen(FILE_PATH, "w+b");
    ESP_LOGI(TAG, "Creating a file: %s %p", FILE_PATH, fp_);

    writeFileHeader();
}

FITFile::~FITFile() {
    // Update the header, must be last step!
    writeFileHeader();

    fclose(fp_);

    if (discard_) {
        int res = remove(FILE_PATH);

        ESP_LOGI(TAG, "File %s removed with status %d", FILE_PATH, res);
    } else {
        ESP_LOGI(TAG, "File %s saves", FILE_PATH);
    }
}

void FITFile::setDiscard(bool to_discard) {
    discard_ = to_discard;
}

void FITFile::writeData(const void *data, size_t data_size) {
    fwrite(data, 1, data_size, fp_);

    for (size_t offset = 0; offset < data_size; offset++) {
        data_crc_ = FitCRC_Get16(data_crc_, *((FIT_UINT8 *)data + offset));
    }
}

void FITFile::writeMessageDefinition(FIT_UINT8 local_mesg_number,
                                     const void *mesg_def_pointer,
                                     size_t mesg_def_size) {
    FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT;
    writeData(&header, FIT_HDR_SIZE);
    writeData(mesg_def_pointer, mesg_def_size);
}

void FITFile::writeMessage(FIT_UINT8 local_mesg_number,
                           const void *mesg_pointer,
                           size_t mesg_size) {
    writeData(&local_mesg_number, FIT_HDR_SIZE);
    writeData(mesg_pointer, mesg_size);
}

void FITFile::writeFileHeader() {
    FIT_FILE_HDR file_header;

    file_header.header_size = FIT_FILE_HDR_SIZE;
    file_header.profile_version = FIT_PROFILE_VERSION;
    file_header.protocol_version = FIT_PROTOCOL_VERSION_20;
    memcpy((FIT_UINT8 *)&file_header.data_type, ".FIT", 4);
    fseek(fp_, 0, SEEK_END);
    file_header.data_size = ftell(fp_) - FIT_FILE_HDR_SIZE - sizeof(FIT_UINT16);
    file_header.crc = FitCRC_Calc16(&file_header, FIT_STRUCT_OFFSET(crc, FIT_FILE_HDR));

    fseek(fp_, 0, SEEK_SET);
    fwrite((void *)&file_header, 1, FIT_FILE_HDR_SIZE, fp_);
}

}  // namespace bk