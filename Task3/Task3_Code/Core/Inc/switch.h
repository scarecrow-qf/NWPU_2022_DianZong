#ifndef __SWITCH_H
#define __SWITCH_H

#include "stm32f4xx_hal.h"

/*  ���ƾ���ֱ����Դ��������   */
#define SWITCH_CONTROL_PORT  GPIOC
#define SWITCH_PIN     GPIO_PIN_0

void SWITCH_ON(void);
void SWITCH_OFF(void);



#endif

