#include "descriptors.h"
#include "scsi.h"

/** LUFA Mass Storage Class driver interface configuration and state information. This structure is
 *  passed to all Mass Storage Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_MS_Device_t Disk_MS_Interface =
{
    .Config =
    {
        .InterfaceNumber           = INTERFACE_ID_MassStorage,
        .DataINEndpoint            =
        {
            .Address           = MASS_STORAGE_IN_EPADDR,
            .Size              = MASS_STORAGE_IO_EPSIZE,
            .Banks             = 1,
        },
        .DataOUTEndpoint           =
        {
            .Address           = MASS_STORAGE_OUT_EPADDR,
            .Size              = MASS_STORAGE_IO_EPSIZE,
            .Banks             = 1,
        },
        .TotalLUNs                 = TOTAL_LUNS,
    },
};

void mass_storage_init(void)
{
    USB_Init();
}

void usb_task(void)
{
    MS_Device_USBTask(&Disk_MS_Interface);
    USB_USBTask();
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
    MS_Device_ConfigureEndpoints(&Disk_MS_Interface);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
    MS_Device_ProcessControlRequest(&Disk_MS_Interface);
}

/** Mass Storage class driver callback function the reception of SCSI commands from the host, which must be processed.
 *
 *  \param[in] MSInterfaceInfo  Pointer to the Mass Storage class interface configuration structure being referenced
 */
bool CALLBACK_MS_Device_SCSICommandReceived(USB_ClassInfo_MS_Device_t* const MSInterfaceInfo)
{
    return SCSI_DecodeSCSICommand(MSInterfaceInfo);
}

