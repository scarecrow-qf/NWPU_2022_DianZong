#include "switch.h"



void SWITCH_ON(void)
{
	HAL_GPIO_WritePin(SWITCH_CONTROL_PORT,SWITCH_PIN,GPIO_PIN_RESET);
}
void SWITCH_OFF(void)
{
	HAL_GPIO_WritePin(SWITCH_CONTROL_PORT,SWITCH_PIN,GPIO_PIN_SET);

}


