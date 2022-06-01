#include "beep.h"



void BEEP_ON(void)
{
	HAL_GPIO_WritePin(BEEP_CONTROL_PORT,BEEP_PIN,GPIO_PIN_SET);
}
void BEEP_OFF(void)
{
	HAL_GPIO_WritePin(BEEP_CONTROL_PORT,BEEP_PIN,GPIO_PIN_RESET);

}


