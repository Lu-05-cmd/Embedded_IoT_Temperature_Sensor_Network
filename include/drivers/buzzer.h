#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f103xb.h"

typedef enum {
    BUZZER_ALERT_OFF = 0,
    BUZZER_ALERT_SLOW,
    BUZZER_ALERT_FAST
} BuzzerAlertMode_t;

void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_UpdateAlert(BuzzerAlertMode_t mode, uint32_t now_ms);

#endif
