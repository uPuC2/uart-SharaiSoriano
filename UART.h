#include <avr/io.h>
#define GREEN           32
#define YELLOW          33
#define BLUE            34


#define MAX_INPUT_VALUE 65535   // Tope maximo para el numero ingresado

// UART register structure
typedef struct {
    volatile uint8_t  UCSRA;
    volatile uint8_t  UCSRB;
    volatile uint8_t  UCSRC;
    uint8_t           res;
    volatile uint16_t UBRR;
    volatile uint8_t  UDR;
} UART_Regs_t;

// Prototypes
// Initialization
void UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop);

// Send
void UART_puts(uint8_t com, const char *str);
void UART_putchar(uint8_t com, char data);

// Received
uint8_t UART_available(uint8_t com);
char UART_getchar(uint8_t com);
void UART_gets(uint8_t com, char *str);

// Escape sequences
void UART_clrscr(uint8_t com);
void UART_setColor(uint8_t com, uint8_t color);
void UART_gotoxy(uint8_t com, uint8_t x, uint8_t y);

// Utils
void itoa(uint16_t number, char *str, uint8_t base);
uint16_t atoi(const char *str);
