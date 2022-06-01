#ifndef __VGUS_H
#define __VGUS_H

#include "usart.h"
#include "stm32f4xx_hal.h"
#include "string.h"


void VGUS_WriteToRegister(uint16_t data[], uint8_t command_length, uint16_t address);
void VGUS_Read_Register(uint16_t data[], uint8_t command_length, uint16_t address, uint8_t data_length);
void VGUS_WriteTostorage(uint16_t data[], uint8_t command_length, uint16_t address);
void VGUS_Read_Storage(uint16_t data[], uint8_t command_length, uint16_t address, uint8_t data_length);
void Send_Temperature(float temperature, uint16_t address);
void Send_Time(uint8_t time, uint16_t address);
struct Receive_data analysis_VGUS_data(uint8_t Receive_data[50]);
#endif
