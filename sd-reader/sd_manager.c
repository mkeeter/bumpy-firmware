/*
  Includes code from the LUFA Library, with the following
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

#include "sd_manager.h"
#include "sd_raw.h"
#include "scsi.h"

uint8_t sd_read_block_callback(uint8_t* buffer, offset_t offset, void* p)
{
    uint16_t count = 0;
    while(Endpoint_Write_Stream_LE(
                buffer, VIRTUAL_MEMORY_BLOCK_SIZE, &count)
          ==  ENDPOINT_RWSTREAM_IncompleteTransfer);
    return 1;
}

void sd_read_blocks(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo,
                    offset_t BlockAddress, uint16_t TotalBlocks)
{
    /* Wait until endpoint is ready before continuing */
    if (Endpoint_WaitUntilReady())
        return;

    sd_raw_read_blocks(BlockAddress << VIRTUAL_MEMORY_BLOCK_SHIFT,
                       TotalBlocks, &sd_read_block_callback, NULL);

    /* If the endpoint is full, send its contents to the host */
    if (!(Endpoint_IsReadWriteAllowed()))
      Endpoint_ClearIN();
}

////////////////////////////////////////////////////////////////////////////////

uintptr_t sd_write_block_callback(uint8_t* buffer, offset_t offset, void* p)
{
    uint16_t count = 0;
    while(Endpoint_Read_Stream_LE(
                buffer, VIRTUAL_MEMORY_BLOCK_SIZE, &count)
          == ENDPOINT_RWSTREAM_IncompleteTransfer);
    return VIRTUAL_MEMORY_BLOCK_SIZE;
}

void sd_write_blocks(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo,
                     offset_t BlockAddress, uint16_t TotalBlocks)
{
    /* Wait until endpoint is ready before continuing */
    if (Endpoint_WaitUntilReady())
      return;

    sd_raw_write_blocks(BlockAddress << VIRTUAL_MEMORY_BLOCK_SHIFT,
                        TotalBlocks, &sd_write_block_callback, NULL);

    /* If the endpoint is empty, clear it ready for the next packet from the host */
    if (!(Endpoint_IsReadWriteAllowed()))
      Endpoint_ClearOUT();
}

////////////////////////////////////////////////////////////////////////////////

uint32_t sd_get_blocks(void)
{
    struct sd_raw_info info;
    sd_raw_get_info(&info);

    return info.capacity >> VIRTUAL_MEMORY_BLOCK_SHIFT;
}
