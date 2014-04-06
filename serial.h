#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>

void serial_init(void);
uint8_t get_char(void);
int put_char(char txchar);
void serial_wait(void);

#endif
