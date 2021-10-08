////////////////////////////////////////////////////////////////////////////////
// The following FIT Protocol software provided may be used with FIT protocol
// devices only and remains the copyrighted property of Garmin Canada Inc.
// The software is being provided on an "as-is" basis and as an accommodation,
// and therefore all warranties, representations, or guarantees of any kind
// (whether express, implied or statutory) including, without limitation,
// warranties of merchantability, non-infringement, or fitness for a particular
// purpose, are specifically disclaimed.
//
// Copyright 2021 Garmin Canada Inc.
////////////////////////////////////////////////////////////////////////////////
// ****WARNING****  This file is auto-generated!  Do NOT edit this file.
// Profile Version = 21.60Release
// Tag = production/akw/21.60.00-0-g38385705
// Product = EXAMPLE
// Alignment = 4 bytes, padding disabled.
////////////////////////////////////////////////////////////////////////////////


#include <string.h>

#include "fit_ram.h"
#include "fit_crc.h"
#include "fit_convert.h"

#if defined(FIT_RAM_INCLUDE)

///////////////////////////////////////////////////////////////////////////////
// Private Variables
///////////////////////////////////////////////////////////////////////////////

static FIT_UINT32 read_crc_offset;
static FIT_UINT16 read_crc;


///////////////////////////////////////////////////////////////////////////////
// Public Functions
///////////////////////////////////////////////////////////////////////////////

FIT_RAM_FILE FitRAM_LookupFile(FIT_FILE file)
{
   FIT_RAM_FILE ram_file;

   for (ram_file = (FIT_RAM_FILE)0; ram_file < FIT_RAM_FILES; ram_file++)
   {
      if (file == fit_ram_files[ram_file]->file_def->type)
         return ram_file;
   }

   return FIT_RAM_FILES;
}

FIT_UINT32 FitRAM_GetFileSize(FIT_RAM_FILE file)
{
   if (file >= FIT_RAM_FILES)
      return 0;

   return FIT_FILE_HDR_SIZE + fit_ram_files[file]->file_def->data_size + sizeof(FIT_UINT16);
}

void FitRAM_FileReadBytes(FIT_RAM_FILE file, FIT_UINT16 file_index, FIT_UINT32 file_offset, void *data, FIT_UINT32 data_size)
{
   FIT_BYTE *data_ptr = (FIT_BYTE *) data;
   const FIT_RAM_FILE_DATA *file_data = fit_ram_files[file]->data;
   FIT_UINT16 data_index = 0;
   FIT_UINT32 offset = file_offset;
   FIT_UINT32 size;
   FIT_UINT8 read_crc_size = 0;

   if ((file_offset + data_size) > (FIT_FILE_HDR_SIZE + fit_ram_files[file]->file_def->data_size))
   {
      read_crc_size = (FIT_UINT8)((file_offset + data_size) - (FIT_FILE_HDR_SIZE + fit_ram_files[file]->file_def->data_size)); // Don't include file crc in copy.
      data_size -= read_crc_size;

      if (read_crc_size > 2)
         read_crc_size = 2;
   }

   if (data_size > 0)
   {
      do
      {
         while ((data_index < fit_ram_files[file]->data_count) && ((file_data->file_offset + file_data->size) <= offset))
         {
            file_data++;
            data_index++;
         }

         if ((data_index >= fit_ram_files[file]->data_count) || (file_data->file_offset >= (file_offset + data_size)))
         {
            size = file_offset + data_size - offset;
            memcpy(&data_ptr[offset - file_offset], &fit_ram_files[file]->file[offset], size);
            offset += size;
            break; // Done.
         }

         if (offset < file_data->file_offset)
         {
            size = file_data->file_offset - offset;

            if ((offset + size) > (file_offset + data_size))
               size = file_offset + data_size - offset;

            memcpy(&data_ptr[offset - file_offset], &fit_ram_files[file]->file[offset], size);
            offset += size;
         }

         size = offset + file_data->size;

         if (size > (file_offset + data_size))
            size = file_offset + data_size;

         size -= offset;
         memcpy(&data_ptr[offset - file_offset], &((FIT_BYTE *)file_data->data)[offset - file_data->file_offset], size);
         offset += size;
      } while (offset < (file_offset + data_size));

      if (file_offset == 0)
      {
         read_crc = 0;
         read_crc_offset = 0;
      }

      if (file_offset > read_crc_offset)
         read_crc_offset = file_offset; // Non-contiguous read. CRC will be invalid.

      if ((file_offset + data_size) > read_crc_offset)
         read_crc = FitCRC_Update16(read_crc, &data_ptr[read_crc_offset - file_offset], (FIT_UINT8)(file_offset + data_size - read_crc_offset));

      read_crc_offset = file_offset + data_size;
   }

   if (read_crc_size > 0)
   {
      data_ptr[offset - file_offset] = (FIT_BYTE)read_crc;
      offset++;

      if (read_crc_size == 1)
         return;

      data_ptr[offset - file_offset] = (FIT_BYTE)(read_crc >> 8);
   }
}

void FitRAM_FileWriteBytes(FIT_RAM_FILE file, FIT_UINT16 file_index, FIT_UINT32 file_offset, const void *data, FIT_UINT32 data_size)
{
   const FIT_RAM_FILE_DATA *file_data;
   FIT_UINT16 data_index;
   FIT_BYTE *src;
   FIT_BYTE *dest;
   FIT_UINT32 size;

   for (data_index = 0; data_index < fit_ram_files[file]->data_count; data_index++)
   {
      file_data = &fit_ram_files[file]->data[data_index];

      if (file_data->file_offset >= (file_offset + data_size))
         return; // Data start is after end of write so we are done.

      if (file_offset >= (file_data->file_offset + file_data->size))
         continue; // Data end is after write so continue to next data element.

      src = (FIT_BYTE *) data;
      dest = (FIT_BYTE *) file_data->data;
      size = data_size;

      if (file_offset < file_data->file_offset)
      {
         size -= file_data->file_offset - file_offset;
         src += file_data->file_offset - file_offset;
      }
      else
      {
         dest += file_offset - file_data->file_offset;
      }

      if (size > file_data->size)
         size = file_data->size;

      memcpy(dest, src, size);
   }
}

#if defined(FIT_CONVERT_MULTI_THREAD)
void FitRAM_FileWriteMesg(FIT_CONVERT_STATE *state, FIT_RAM_FILE file, FIT_UINT16 file_index, FIT_UINT16 mesg_num, const void *mesg_data, FIT_BOOL restore_fields)
#else
void FitRAM_FileWriteMesg(FIT_RAM_FILE file, FIT_UINT16 file_index, FIT_UINT16 mesg_num, const void *mesg_data, FIT_BOOL restore_fields)
#endif
{
   FIT_UINT32 offset;
   FIT_UINT8 size;
   FIT_BYTE old_mesg[FIT_MESG_SIZE];

   offset = Fit_GetFileMesgOffset(fit_ram_files[file]->file_def, mesg_num, 0);
   size = Fit_GetMesgSize(mesg_num);

   if (restore_fields)
   {
      FitRAM_FileReadBytes(file, file_index, offset, old_mesg, size);
      #if defined(FIT_CONVERT_MULTI_THREAD)
          FitConvert_RestoreFields(state, old_mesg);
      #else
          FitConvert_RestoreFields(old_mesg);
      #endif
   }

   FitRAM_FileWriteBytes(file, file_index, offset, mesg_data, size);
}

#endif // defined(FIT_RAM_INCLUDE)

