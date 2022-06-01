#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f4xx_hal.h"

/*  ·äÃùÆ÷Òý½Å   */
#define BEEP_CONTROL_PORT  GPIOC
#define BEEP_PIN     GPIO_PIN_0

void BEEP_ON(void);
void BEEP_OFF(void);



#endif

