#ifndef UART_DRIVER_H
#define UART_DRIVER_H

void uart_init(void);
int uart_read_line(char *out, int max_len);

#endif