#include "VGUS.h"

/*注意！！！！！

所有组件的基本信息是按照16位设置发送的

*/

/***********************************
函数描述：寄存器写指令函数
内容：    VGUS屏有256Byte的寄存器，主要用于相关硬件控制操作，按照字节（Byte）寻址。
					起始地址对于寄存器写/读指令(0x80、0x81)是单字节，地址范围为0x00~0xff。
					数据长度(N)以字节为单位 ，表示连续读寄存器单元个数。
					单元功能已经定义好，用于硬件配置和控制操作，典型功能如背光亮度调节、版本信息读取、发串口指令控制图片切换、音频播放控制、视频播放控制等。

数据手册：
表2-4 串口发送0x80指令帧结构（写寄存器）
定义 指令帧头 指令长度 指令 起始地址 数据内容
长度（字节） 2 1 1(0x80) 1 N
举例：寄存器0x03和0x04单元连续写入0x00 、0x01
发送：0xA5 0x5A 0x04 0x80 0x03 0x00 0x01
参数：写入数据  指令长度(包含指令和起始地址)  写寄存器起始长度
***********************************/
void VGUS_WriteToRegister(uint16_t data[], uint8_t command_length, uint16_t address)
{		
		uint8_t data_buff[8],i=0;
		data_buff[0] = 0xA5;
		data_buff[1] = 0x5A;								//指令帧头
		data_buff[2] = command_length;			//后续指令长度
		data_buff[3] = 0x80;								//写指令 0x80
		data_buff[4] = address; 						//起始写入地址
		for(i=0; i<(command_length-2);i++)
		{
			data_buff[i+5] = data[i];
		}
		HAL_UART_Transmit(&huart4, (uint8_t *)data_buff, sizeof(data_buff),0x00ff);
	
}

/***********************************
函数描述：寄存器读指令函数

	
描述：同寄存器写指令 代表了读取寄存器的指令

数据手册参考：
表2-5 串口发送0x81指令帧结构（读寄存器发送）
定义 指令帧头 指令长度 指令 起始地址 数据长度
长度（字节） 2 1 1(0x81) 1 1(N)

表2-6 串口接收0x81指令帧结构（读寄存器接收，串口屏发送的）
定义 指令帧头 指令长度 指令 起始地址 数据长度 数据内容
长度（字节） 2 1 1(0x81) 1 1(N) N
举例：连续读取寄存器寄存器0x03和0x04单元
发送：0xA5 0x5A 0x03 0x81 0x03 0x02
返回：0xA5 0x5A 0x05 0x81 0x03 0x02 0x00 0x01
***********************************/
void VGUS_Read_Register(uint16_t data[], uint8_t command_length, uint16_t address, uint8_t data_length)
{		
		uint8_t data_buff[8];
		data_buff[0] = 0xA5;
		data_buff[1] = 0x5A;						//指令帧头
		data_buff[2] = command_length;	//后续指令长度
		data_buff[3] = 0x81;						//读指令 0x81
		data_buff[4] = address; 				//起始写入地址
		data_buff[5] = data_length;
		HAL_UART_Transmit(&huart4, (uint8_t *)data_buff, sizeof(data_buff),0x00ff);
}

/***********************************
函数描述：变量存储器写指令函数


数据手册参考：
表2-8 串口发送0x82指令帧结构（写变量存储器单元）
定义 指令帧头 指令长度 指令 起始地址 数据内容
长度（字节） 2 1 1(0x82) 2 2N
举例：变量存储器0x0003单元写入0x00 、0x01
发送：0xA5 0x5A 0x05 0x82 0x00 0x03 0x00 0x01

***********************************/
uint8_t data_buff[8],i=0;
void VGUS_WriteTostorage(uint16_t data[], uint8_t command_length, uint16_t address)
{		
		
		data_buff[0] = 0xA5;
		data_buff[1] = 0x5A;									//指令帧头
		data_buff[2] = command_length;				//后续指令长度
		data_buff[3] = 0x82;									//写指令 0x82
		data_buff[4] = (address>>8)&0xff; 		//起始写入地址
		data_buff[5] = address&0xff;					//地址
		for(i=0; i<(command_length - 3)/2;i++)
		{
			data_buff[i+6] = (data[i]>>8)&0xff;
			data_buff[i+7] = data[i]&0xff;
		}
		HAL_UART_Transmit(&huart4, (uint8_t *)data_buff, sizeof(data_buff),0x00ff);
}
/***********************************
函数描述：变量存储器读指令函数


数据手册参考：
表2-9 串口发送0x83指令帧结构（读变量存储器发送）
定义 指令帧头 指令长度 指令 起始地址 数据长度
长度（字节） 2 1 1(0x83) 2 1(N)
表2-10 串口接收0x83指令帧结构（读变量存储器接收，串口屏发送的）
定义 指令帧头 指令长度 指令 起始地址 数据长度 数据内容
长度（字节） 2 1 1(0x83) 2 1(N) 2N
举例：连续读取变量存储器0x0003和0x0004单元
发送：0xA5 0x5A 0x04 0x83 0x00 0x03 0x02
返回：0xA5 0x5A 0x08 0x83 0x00 0x03 0x02 0x00 0x01 0xff 0xff

***********************************/

void VGUS_Read_Storage(uint16_t data[], uint8_t command_length, uint16_t address, uint8_t data_length)
{
		uint8_t data_buff[8];
		data_buff[0] = 0xA5;
		data_buff[1] = 0x5A;	//指令帧头
		data_buff[2] = command_length;				//后续指令长度
		data_buff[3] = 0x83;									//读指令 0x83
		data_buff[4] = (address>>8)&0xff; 		//起始写入地址
		data_buff[5] = address&0xff;					//地址
		data_buff[6] = data_length;
		HAL_UART_Transmit(&huart4, (uint8_t *)data_buff, sizeof(data_buff),0x00ff);
	
}
/***********************************
函数描述：变量存储器读指令函数
描述：    发送电压数据给串口屏，串口屏中需要设置温度的组件保留一位小数，三位整数	
***********************************/
void Send_Voltage(float voltage, uint16_t address)
{
	uint16_t data_to_send[1];
	/*码制转换*/
	data_to_send[0] = (int)(voltage*10);
	VGUS_WriteTostorage(data_to_send,5,address);
}
/***********************************
函数描述：变量存储器读指令函数
描述：    发送电流数据给串口屏，串口屏单位是mA 因此不需要小数 显示数字为整数
***********************************/
void Send_Current(uint16_t current, uint16_t address)
{
	uint16_t data_to_send[1];
	/*码制转换*/
	data_to_send[0] = current;
	VGUS_WriteTostorage(data_to_send,5,address);
}

/***********************************
函数描述：变量存储器读指令函数
描述：    发送时间数据给串口屏 
************************************/
void Send_Time(uint8_t time, uint16_t address)
{
	uint16_t data_to_send[1];
	/*码制转换*/
	data_to_send[0] = time;
	VGUS_WriteTostorage(data_to_send,5,address);
}

/***********************************
函数描述：返回数据接收状态机
描述：    将串口屏返回的数据进行解析	
************************************/
struct Receive_data analysis_VGUS_data(uint8_t Receive_data[50])
{
	struct Receive_data data;
	uint8_t i;
	data.header[0] = Receive_data[0];
	data.header[1] = Receive_data[1];
	data.command_length = Receive_data[2];
	data.command = Receive_data[3];
	data.start_address[0] = Receive_data[4];
	data.start_address[1] = Receive_data[5];
	data.data_length = Receive_data[6];
	for(i=1;i<= 2*data.data_length; i++)
		data.data[i-1] = Receive_data[6+i];
	return data;
}



