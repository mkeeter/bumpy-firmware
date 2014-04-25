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

void sd_read_blocks(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo,
                    offset_t BlockAddress, uint16_t TotalBlocks)
{
    /* Wait until endpoint is ready before continuing */
    if (Endpoint_WaitUntilReady())
        return;

    const offset_t EndAddress = BlockAddress + TotalBlocks;
    for (offset_t addr = BlockAddress; addr < EndAddress; addr++)
    {
        sd_raw_cache_block(addr << VIRTUAL_MEMORY_BLOCK_SHIFT);

        uint16_t written = 0;
        uint8_t error;
        while((error = Endpoint_Write_Stream_LE(
                    sd_raw_block, VIRTUAL_MEMORY_BLOCK_SIZE, &written))
              ==  ENDPOINT_RWSTREAM_IncompleteTransfer);

        if (error != ENDPOINT_RWSTREAM_NoError)
        {
            printf(":(");
        }
    }

    /* If the endpoint is full, send its contents to the host */
    if (!(Endpoint_IsReadWriteAllowed()))
      Endpoint_ClearIN();
}

////////////////////////////////////////////////////////////////////////////////

void sd_write_blocks(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo,
                     offset_t BlockAddress, uint16_t TotalBlocks)
{
    /* Wait until endpoint is ready before continuing */
    if (Endpoint_WaitUntilReady())
      return;

    const offset_t EndAddress = BlockAddress + TotalBlocks;
    for (offset_t addr = BlockAddress; addr < EndAddress; addr++)
    {
        sd_raw_sync();
        sd_raw_block_address = addr << VIRTUAL_MEMORY_BLOCK_SHIFT;
        sd_raw_block_written = 0;

        uint8_t error;
        uint16_t written = 0;

        while((error = Endpoint_Read_Stream_LE(
                    sd_raw_block, VIRTUAL_MEMORY_BLOCK_SIZE, &written))
              ==  ENDPOINT_RWSTREAM_IncompleteTransfer);

        if (error != ENDPOINT_RWSTREAM_NoError)
        {
            printf(":(");
        }
    }

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
