#include "uart.h"
#include "driver/uart.h"
#include "string.h"

#define UART_PORT_NUM      UART_NUM_2
#define UART_TX_PIN        17
#define UART_RX_PIN        16
#define BUF_SIZE           1024

static uint8_t buffer[BUF_SIZE];

void uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
}

int uart_read_line(char *out, int max_len)
{
    int len = uart_read_bytes(UART_PORT_NUM, buffer, BUF_SIZE, 20 / portTICK_PERIOD_MS);

    if (len > 0)
    {
        int idx = 0;

        for (int i = 0; i < len && idx < max_len - 1; i++)
        {
            if (buffer[i] == '\n')
            {
                out[idx] = '\0';
                return idx;
            }

            if (buffer[i] != '\r')
            {
                out[idx++] = buffer[i];
            }
        }
        out[idx] = '\0';
        return idx;
    }

    return 0;
}