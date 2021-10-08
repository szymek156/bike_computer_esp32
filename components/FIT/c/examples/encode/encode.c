////////////////////////////////////////////////////////////////////////////////
// The following FIT Protocol software provided may be used with FIT protocol
// devices only and remains the copyrighted property of Dynastream Innovations Inc.
// The software is being provided on an "as-is" basis and as an accommodation,
// and therefore all warranties, representations, or guarantees of any kind
// (whether express, implied or statutory) including, without limitation,
// warranties of merchantability, non-infringement, or fitness for a particular
// purpose, are specifically disclaimed.
//
// Copyright 2008-2015 Dynastream Innovations Inc.
////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"

#include "fit_product.h"
#include "fit_crc.h"

///////////////////////////////////////////////////////////////////////
// Private Function Prototypes
///////////////////////////////////////////////////////////////////////

void WriteFileHeader(FILE *fp);
///////////////////////////////////////////////////////////////////////
// Creates a FIT file. Puts a place-holder for the file header on top of the file.
///////////////////////////////////////////////////////////////////////

void WriteMessageDefinition(FIT_UINT8 local_mesg_number, const void *mesg_def_pointer, FIT_UINT8 mesg_def_size, FILE *fp);
///////////////////////////////////////////////////////////////////////
// Appends a FIT message definition (including the definition header) to the end of a file.
///////////////////////////////////////////////////////////////////////

void WriteMessageDefinitionWithDevFields
   (
   FIT_UINT8 local_mesg_number,
   const void *mesg_def_pointer,
   FIT_UINT8 mesg_def_size,
   FIT_UINT8 number_dev_fields,
   FIT_DEV_FIELD_DEF *dev_field_definitions,
   FILE *fp
   );
///////////////////////////////////////////////////////////////////////
// Appends a FIT message definition (including the definition header)
// and additionalo dev field definition data to the end of a file.
///////////////////////////////////////////////////////////////////////

void WriteMessage(FIT_UINT8 local_mesg_number, const void *mesg_pointer, FIT_UINT8 mesg_size, FILE *fp);
///////////////////////////////////////////////////////////////////////
// Appends a FIT message (including the message header) to the end of a file.
///////////////////////////////////////////////////////////////////////

void WriteDeveloperField(const void* data, FIT_UINT8 data_size, FILE *fp);
///////////////////////////////////////////////////////////////////////
// Appends Developer Fields to a Message
///////////////////////////////////////////////////////////////////////

void WriteData(const void *data, FIT_UINT8 data_size, FILE *fp);
///////////////////////////////////////////////////////////////////////
// Writes data to the file and updates the data CRC.
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Private Variables
///////////////////////////////////////////////////////////////////////

static FIT_UINT16 data_crc;

int main(void)
{
   FILE *fp;

   data_crc = 0;
   fp = fopen("test.fit", "w+b");

   WriteFileHeader(fp);

   // Write file id message.
   {
      FIT_UINT8 local_mesg_number = 0;
      FIT_FILE_ID_MESG file_id;
      Fit_InitMesg(fit_mesg_defs[FIT_MESG_FILE_ID], &file_id);
      file_id.type = FIT_FILE_SETTINGS;
      file_id.manufacturer = FIT_MANUFACTURER_DEVELOPMENT;
      WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_FILE_ID], FIT_FILE_ID_MESG_DEF_SIZE, fp);
      WriteMessage(local_mesg_number, &file_id, FIT_FILE_ID_MESG_SIZE, fp);
   }

   // Write a Developer Id Message
   {
      const FIT_UINT8 appId[] =
         {
         0x0, 0x1, 0x2, 0x3,
         0x4, 0x5, 0x6, 0x7,
         0x8, 0x9, 0xA, 0xB,
         0xC, 0xD, 0xE, 0xF
         };
      FIT_UINT8 local_mesg_number = 0;
      FIT_DEVELOPER_DATA_ID_MESG data_id_mesg;
      Fit_InitMesg(fit_mesg_defs[FIT_MESG_DEVELOPER_DATA_ID], &data_id_mesg);
      data_id_mesg.developer_data_index = 0;
      memcpy(data_id_mesg.application_id, appId, FIT_DEVELOPER_DATA_ID_MESG_APPLICATION_ID_COUNT);
      data_id_mesg.manufacturer_id = FIT_MANUFACTURER_GARMIN;
      WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_DEVELOPER_DATA_ID], FIT_DEVELOPER_DATA_ID_MESG_DEF_SIZE, fp);
      WriteMessage(local_mesg_number, &data_id_mesg, FIT_DEVELOPER_DATA_ID_MESG_SIZE, fp);
   }

   // Write a Field Description
   {
      FIT_UINT8 local_mesg_number = 0;
      FIT_FIELD_DESCRIPTION_MESG field_description_mesg;
      Fit_InitMesg(fit_mesg_defs[FIT_MESG_FIELD_DESCRIPTION], &field_description_mesg);
      field_description_mesg.developer_data_index = 0;
      field_description_mesg.field_definition_number = 1;
      strcpy(field_description_mesg.field_name, "dog_age");
      strcpy(field_description_mesg.units, "years");

      field_description_mesg.fit_base_type_id = FIT_BASE_TYPE_UINT16;
      WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_FIELD_DESCRIPTION], FIT_FIELD_DESCRIPTION_MESG_DEF_SIZE, fp);
      WriteMessage(local_mesg_number, &field_description_mesg, FIT_FIELD_DESCRIPTION_MESG_SIZE, fp);
   }

   // Write user profile message.
   {
      FIT_UINT8 local_mesg_number = 1; // In some cases, careful selection of local message numbers may reduce number of definition messages in a FIT file
      FIT_UINT16 dog_age = 33 * 4 + 21;
      FIT_USER_PROFILE_MESG user_profile;
      FIT_DEV_FIELD_DEF dog_age_def;

      dog_age_def.def_num = 1;
      dog_age_def.size = sizeof(FIT_UINT16);
      dog_age_def.dev_index = 0;

      Fit_InitMesg(fit_mesg_defs[FIT_MESG_USER_PROFILE], &user_profile);
      user_profile.gender = FIT_GENDER_FEMALE;
      user_profile.age = 35;
      WriteMessageDefinitionWithDevFields
         (
         local_mesg_number,
         fit_mesg_defs[FIT_MESG_USER_PROFILE],
         FIT_USER_PROFILE_MESG_DEF_SIZE,
         1,
         &dog_age_def,
         fp
         );

      WriteMessage(local_mesg_number, &user_profile, FIT_USER_PROFILE_MESG_SIZE, fp);
      WriteDeveloperField(&dog_age, sizeof(FIT_UINT16), fp);
   }

   // Write CRC.
   fwrite(&data_crc, 1, sizeof(FIT_UINT16), fp);

   // Update file header with data size.
   WriteFileHeader(fp);

   fclose(fp);

   return 0;
}

void WriteFileHeader(FILE *fp)
{
   FIT_FILE_HDR file_header;

   file_header.header_size = FIT_FILE_HDR_SIZE;
   file_header.profile_version = FIT_PROFILE_VERSION;
   file_header.protocol_version = FIT_PROTOCOL_VERSION_20;
   memcpy((FIT_UINT8 *)&file_header.data_type, ".FIT", 4);
   fseek (fp , 0 , SEEK_END);
   file_header.data_size = ftell(fp) - FIT_FILE_HDR_SIZE - sizeof(FIT_UINT16);
   file_header.crc = FitCRC_Calc16(&file_header, FIT_STRUCT_OFFSET(crc, FIT_FILE_HDR));

   fseek (fp , 0 , SEEK_SET);
   fwrite((void *)&file_header, 1, FIT_FILE_HDR_SIZE, fp);
}

void WriteMessageDefinition(FIT_UINT8 local_mesg_number, const void *mesg_def_pointer, FIT_UINT8 mesg_def_size, FILE *fp)
{
   FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT;
   WriteData(&header, FIT_HDR_SIZE, fp);
   WriteData(mesg_def_pointer, mesg_def_size, fp);
}

void WriteMessageDefinitionWithDevFields
   (
   FIT_UINT8 local_mesg_number,
   const void *mesg_def_pointer,
   FIT_UINT8 mesg_def_size,
   FIT_UINT8 number_dev_fields,
   FIT_DEV_FIELD_DEF *dev_field_definitions,
   FILE *fp
   )
{
   FIT_UINT16 i;
   FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT | FIT_HDR_DEV_DATA_BIT;
   WriteData(&header, FIT_HDR_SIZE, fp);
   WriteData(mesg_def_pointer, mesg_def_size, fp);

   WriteData(&number_dev_fields, sizeof(FIT_UINT8), fp);
   for (i = 0; i < number_dev_fields; i++)
   {
      WriteData(&dev_field_definitions[i], sizeof(FIT_DEV_FIELD_DEF), fp);
   }
}

void WriteMessage(FIT_UINT8 local_mesg_number, const void *mesg_pointer, FIT_UINT8 mesg_size, FILE *fp)
{
   WriteData(&local_mesg_number, FIT_HDR_SIZE, fp);
   WriteData(mesg_pointer, mesg_size, fp);
}

void WriteDeveloperField(const void *data, FIT_UINT8 data_size, FILE *fp)
{
   WriteData(data, data_size, fp);
}

void WriteData(const void *data, FIT_UINT8 data_size, FILE *fp)
{
   FIT_UINT8 offset;

   fwrite(data, 1, data_size, fp);

   for (offset = 0; offset < data_size; offset++)
      data_crc = FitCRC_Get16(data_crc, *((FIT_UINT8 *)data + offset));
}
