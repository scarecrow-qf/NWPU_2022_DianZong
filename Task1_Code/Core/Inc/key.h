#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f4xx.h" 

/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
#define KEY0 		HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) //PB1
#define KEY1 		HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1)	//PC1


#define KEY0_PRES 	1
#define KEY1_PRES	2


uint8_t KEY_Scan(void);  		//����ɨ�躯��	

#endif
