#ifndef LOGGER_H
#define LOGGER_H
#include <stdint.h>
#define LOGGER_BUFFER_SIZE 128
void Logger_Init(void);
void Logger_Log(const char *msg);
void Logger_Logf(const char *fmt, ...);
#endif // LOGGER_H
