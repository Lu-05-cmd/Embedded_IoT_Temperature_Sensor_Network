#include "middleware/logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "drivers/gpio.h" 

static char logger_buffer[LOGGER_BUFFER_SIZE];

void Logger_Init(void) {
}

void Logger_Log(const char *msg) {
	snprintf(logger_buffer, LOGGER_BUFFER_SIZE, "%s\n", msg);
}

void Logger_Logf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsnprintf(logger_buffer, LOGGER_BUFFER_SIZE, fmt, args);
	va_end(args);
}
