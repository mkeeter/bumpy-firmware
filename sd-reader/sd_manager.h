#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#include <LUFA/Drivers/USB/USB.h>

void sd_read_blocks(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo,
                    uint32_t BlockAddress, uint16_t TotalBlocks);
void sd_write_blocks(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo,
                     uint32_t BlockAddress, uint16_t TotalBlocks);

#endif
