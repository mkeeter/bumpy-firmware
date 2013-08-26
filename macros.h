#ifndef MACROS_H
#define MACROS_H

#define OUTPUT(directions,pin) (directions |= (1<<pin)) 
#define INPUT(directions, pin) (directions &= ~(1<<(pin)))

#define SET(port,pin) (port |= (1<<pin))
#define CLEAR(port,pin) (port &= (~(1<<pin)))

#endif
