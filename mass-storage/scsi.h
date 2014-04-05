/*
  Based on code from the LUFA Library, with the following
  copyright information:

  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for SCSI.c.
 */

#ifndef _SCSI_H_
#define _SCSI_H_

/* Includes: */
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <LUFA/Drivers/USB/USB.h>

/*****************************************************************************/

/* Configuration */
#define TOTAL_LUNS      1

/** Total number of kilobytes of the storage medium, comprised of one 8 GB microSD card. */
#define VIRTUAL_MEMORY_KBYTES                ((uint32_t)0x800000)

/** Block size of the device. This is kept at 512 to remain compatible with the OS despite the underlying
 *  storage media (Dataflash) using a different native block size. Do not change this value.
 */
#define VIRTUAL_MEMORY_BLOCK_SIZE           512

/** Total number of blocks of the virtual memory for reporting to the host as the device's total capacity. Do not
 *  change this value; change VIRTUAL_MEMORY_BYTES instead to alter the media size.
 */
#define VIRTUAL_MEMORY_BLOCKS               ((uint32_t)((VIRTUAL_MEMORY_KBYTES / VIRTUAL_MEMORY_BLOCK_SIZE) << 10))

/** Blocks in each LUN, calculated from the total capacity divided by the total number of Logical Units in the device. */
#define LUN_MEDIA_BLOCKS                    ((uint32_t)(VIRTUAL_MEMORY_BLOCKS / TOTAL_LUNS))

#define DISK_READ_ONLY  false

/*****************************************************************************/

/* Macros: */
/** Macro to set the current SCSI sense data to the given key, additional sense code and additional sense qualifier. This
 *  is for convenience, as it allows for all three sense values (returned upon request to the host to give information about
 *  the last command failure) in a quick and easy manner.
 *
 *  \param[in] Key    New SCSI sense key to set the sense code to
 *  \param[in] Acode  New SCSI additional sense key to set the additional sense code to
 *  \param[in] Aqual  New SCSI additional sense key qualifier to set the additional sense qualifier code to
 */
#define SCSI_SET_SENSE(Key, Acode, Aqual)  MACROS{ SenseData.SenseKey                 = (Key);   \
                                                   SenseData.AdditionalSenseCode      = (Acode); \
                                                   SenseData.AdditionalSenseQualifier = (Aqual); }MACROE

/** Macro for the \ref SCSI_Command_ReadWrite_10() function, to indicate that data is to be read from the storage medium. */
#define DATA_READ           true

/** Macro for the \ref SCSI_Command_ReadWrite_10() function, to indicate that data is to be written to the storage medium. */
#define DATA_WRITE          false

/** Value for the DeviceType entry in the SCSI_Inquiry_Response_t enum, indicating a Block Media device. */
#define DEVICE_TYPE_BLOCK   0x00

/** Value for the DeviceType entry in the SCSI_Inquiry_Response_t enum, indicating a CD-ROM device. */
#define DEVICE_TYPE_CDROM   0x05

/* Function Prototypes: */
bool SCSI_DecodeSCSICommand(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo);

#endif
