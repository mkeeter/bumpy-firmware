#include <avr/io.h>
#include <avr/interrupt.h>

#include "serial.h"
#include "macros.h"

#define BUFFER_SIZE 128 // must be a power of 2, otherwise the
#define BUFFER_MASK (BUFFER_SIZE - 1) // masking won't work.

#define BAUD 9600
#define MYUBRR (F_CPU/(8L*BAUD)-1)


static volatile char in_buffer[BUFFER_SIZE];
static volatile char out_buffer[BUFFER_SIZE];

static volatile unsigned int in_buffer_read   = 0,
                             in_buffer_write  = 0,
                             out_buffer_read  = 0,
                             out_buffer_write = 0;

static int put_char_f(char txchar, FILE* stream);

// Set up a stream to use printf
static FILE serial_stdout = FDEV_SETUP_STREAM(
        put_char_f, NULL, _FDEV_SETUP_WRITE
    );


static int in_buffer_full(void)
{
    return in_buffer_write == ((in_buffer_read - 1) & BUFFER_MASK);
}


static int out_buffer_full(void)
{
    return out_buffer_write == ((out_buffer_read - 1) & BUFFER_MASK);
}


static int in_buffer_empty(void)
{
    return in_buffer_write == in_buffer_read;
}


static int out_buffer_empty(void)
{
    return out_buffer_write == out_buffer_read;
}


void serial_init(void)
{
    // Clear transmit complete flag, double speed, do not use MPC
    UCSR1A = (1 << U2X1);

    // Default frame settings are already correct (8 data bits, 1 stop bit)

    // Set baud
    UBRR1H = (MYUBRR >> 8) & 0b00001111;
    UBRR1L = MYUBRR & 255;

    // Turn it on and enable RX interrupt
    // (tx interrupt is enabled when you put a char in the buffer)
    UCSR1B = (1 << TXEN1) | (1 << RXEN1) | (1 << RXCIE1);

    // Initialize stdout to the custom file-style handler
    // that we defined above.
    stdout = &serial_stdout;

    // Enable interrupts!
    sei();
}


//  Returns a character from the buffer, or 0 if the buffer is empty
uint8_t get_char(void) {
    CLEAR(UCSR1B, RXCIE1); // Disable rx interrupt

    const char c = in_buffer_empty() ? 0 : in_buffer[in_buffer_read++];
    in_buffer_read &= BUFFER_MASK;

    SET(UCSR1B, RXCIE1); // Enable rx interrupt
    return c;
}

static int put_char_f(char txchar, FILE* stream) {
    return put_char(txchar);
}

int put_char(char txchar) {
    CLEAR(UCSR1B, UDRIE1); // Disable tx interrupt

    // If the OUT buffer is filled, skip the next output
    // character by advancing the read head by one.
    if (out_buffer_full()) {// Skip a character
        out_buffer_read = (out_buffer_read + 1) & BUFFER_MASK;
    }
    out_buffer[out_buffer_write++] = txchar;
    out_buffer_write &= BUFFER_MASK;

    SET(UCSR1B, UDRIE1); // Enable tx interrupt
    return 0;
}


// Interrupt vector called when a character is done
// being sent.  Automatically sends the next character
ISR(USART1_UDRE_vect)
{
    // Put the next character from the OUT buffer into the transmit
    // register, to be sent out as soon as possible.
    UDR1 = out_buffer[out_buffer_read];

    // Advance the read head on the OUT buffer
    out_buffer_read = (out_buffer_read + 1) & BUFFER_MASK;

    // If we have no more to send, then disable the output interrupt.
    if (out_buffer_empty()) {
        CLEAR(UCSR1B, UDRIE1);
    }
}


// Interrupt vector called when a new character is received.
ISR(USART1_RX_vect)
{
    // If the IN buffer is filled, skip a character by
    // advancing the read pointer forward by one.
    if (in_buffer_full()) {
        in_buffer_read = (in_buffer_read + 1) & BUFFER_MASK;
    }

    // Save the received character in the IN buffer.
    in_buffer[in_buffer_write] = UDR1;

    // Advance the write head of the IN buffer
    in_buffer_write = (in_buffer_write + 1) & BUFFER_MASK;
}
