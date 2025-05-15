#include <avr/io.h>
#include "UART.h"

#ifndef F_CPU
#define F_CPU 16000000UL  // 16 MHz
#endif

// UART register pointers
UART_Regs_t *uart_offsets[] = {
    (UART_Regs_t *)&UCSR0A,
    (UART_Regs_t *)&UCSR1A,
    (UART_Regs_t *)&UCSR2A,
    (UART_Regs_t *)&UCSR3A
};

// UART setup
void UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop) {
    uint16_t ubrr = F_CPU / 16 / baudrate - 1;
    uart_offsets[com]->UBRR = ubrr;

    uint8_t ucsrc = 0;

    // Data bits
    if (size == 5)
        ucsrc |= (0 << UCSZ00) | (0 << UCSZ01);
    else if (size == 6)
        ucsrc |= (1 << UCSZ00) | (0 << UCSZ01);
    else if (size == 7)
        ucsrc |= (0 << UCSZ00) | (1 << UCSZ01);
    else
        ucsrc |= (1 << UCSZ00) | (1 << UCSZ01);

    // Parity
    if (parity == 1)
        ucsrc |= (1 << UPM00) | (1 << UPM01);
    else if (parity == 2)
        ucsrc |= (0 << UPM00) | (1 << UPM01);

    // Stop bits
    if (stop == 2)
        ucsrc |= (1 << USBS0);

    uart_offsets[com]->UCSRC = ucsrc;

    // Enable RX and TX
    uart_offsets[com]->UCSRB = (1 << RXEN0) | (1 << TXEN0);
}

// Send one char
void UART_putchar(uint8_t com, char data) {
    while (!(uart_offsets[com]->UCSRA & (1 << UDRE0)));
    uart_offsets[com]->UDR = data;
}

// Send string
void UART_puts(uint8_t com, const char *str) {
    while (*str) {
        UART_putchar(com, *str++);
    }
}

// Check if data is available
uint8_t UART_available(uint8_t com) {
    return (uart_offsets[com]->UCSRA & (1 << RXC0));
}

// Receive one char
char UART_getchar(uint8_t com) {
    while (!UART_available(com));
    return uart_offsets[com]->UDR;
}

// Read string until Enter
void UART_gets(uint8_t com, char *str) {
    uint8_t i = 0;
    char c;

    while (1) {
        c = UART_getchar(com);

        if (c == '\r' || c == '\n') {
            UART_putchar(com, '\r');
            UART_putchar(com, '\n');
            str[i] = '\0';
            break;
        }
        else if (c == 8 || c == 127) {
            if (i > 0) {
                UART_putchar(com, 8);
                UART_putchar(com, ' ');
                UART_putchar(com, 8);
                i--;
            }
        }
        else {
            if (i < 20) {
                UART_putchar(com, c);
                str[i++] = c;
            }
        }
    }

    if (i == 0) {
        str[0] = '\0';
    }
}

// Clear screen
void UART_clrscr(uint8_t com) {
    UART_puts(com, "\033[2J");
    UART_puts(com, "\033[H");
}

// Set text color
void UART_setColor(uint8_t com, uint8_t color) {
    char colorStr[10];
    UART_puts(com, "\033[");
    itoa(color, colorStr, 10);
    UART_puts(com, colorStr);
    UART_puts(com, "m");
}

// Move cursor
void UART_gotoxy(uint8_t com, uint8_t x, uint8_t y) {
    char posStr[10];
    UART_puts(com, "\033[");
    itoa(y, posStr, 10);
    UART_puts(com, posStr);
    UART_puts(com, ";");
    itoa(x, posStr, 10);
    UART_puts(com, posStr);
    UART_puts(com, "H");
}

// int to string
void itoa(uint16_t number, char *str, uint8_t base) {
    char temp[17];
    uint8_t i = 0;
    uint8_t j = 0;

    if (number == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (number != 0) {
        uint16_t rem = number % base;
        temp[i++] = (rem < 10) ? '0' + rem : 'A' + rem - 10;
        number = number / base;
    }

    for (j = 0; j < i; j++) {
        str[j] = temp[i - j - 1];
    }
    str[j] = '\0';
}

// string to int
uint16_t atoi(const char *str) {
    uint16_t result = 0;
    uint8_t i = 0;

    if (str[0] == '\0') {
        return 0;
    }

    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        if (result > MAX_INPUT_VALUE) {
            return MAX_INPUT_VALUE;
        }
        i++;
    }

    return result;
}
