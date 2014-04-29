#ifndef MASS_STORAGE_H
#define MASS_STORAGE_H

#include <stdbool.h>

void mass_storage_init(void);
void mass_storage_eject(void);
bool usb_task(void);

#endif
