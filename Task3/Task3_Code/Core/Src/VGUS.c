#include "VGUS.h"

/*ע�⣡��������

��������Ļ�����Ϣ�ǰ���16λ���÷��͵�

*/

/***********************************
�����������Ĵ���дָ���
���ݣ�    VGUS����256Byte�ļĴ�������Ҫ�������Ӳ�����Ʋ����������ֽڣ�Byte��Ѱַ��
					��ʼ��ַ���ڼĴ���д/��ָ��(0x80��0x81)�ǵ��ֽڣ���ַ��ΧΪ0x00~0xff��
					���ݳ���(N)���ֽ�Ϊ��λ ����ʾ�������Ĵ�����Ԫ������
					��Ԫ�����Ѿ�����ã�����Ӳ�����úͿ��Ʋ��������͹����米�����ȵ��ڡ��汾��Ϣ��ȡ��������ָ�����ͼƬ�л�����Ƶ���ſ��ơ���Ƶ���ſ��Ƶȡ�

�����ֲ᣺
��2-4 ���ڷ���0x80ָ��֡�ṹ��д�Ĵ�����
���� ָ��֡ͷ ָ��� ָ�� ��ʼ��ַ ��������
���ȣ��ֽڣ� 2 1 1(0x80) 1 N
�������Ĵ���0x03��0x04��Ԫ����д��0x00 ��0x01
���ͣ�0xA5 0x5A 0x04 0x80 0x03 0x00 0x01
������д������  ָ���(����ָ�����ʼ��ַ)  д�Ĵ�����ʼ����
***********************************/
void VGUS_WriteToRegister(uint16_t data[], uint8_t command_length, uint16_t address)
{		
		uint8_t data_buff[8],i=0;
		data_buff[0] = 0xA5;
		data_buff[1] = 0x5A;								//ָ��֡ͷ
		data_buff[2] = command_length;			//����ָ���
		data_buff[3] = 0x80;								//дָ�� 0x80
		data_buff[4] = address; 						//��ʼд���ַ
		for(i=0; i<(command_length-2);i++)
		{
			data_buff[i+5] = data[i];
		}
		HAL_UART_Transmit(&huart4, (uint8_t *)data_buff, sizeof(data_buff),0x00ff);
	
}

/***********************************
�����������Ĵ�����ָ���

	
������ͬ�Ĵ���дָ�� �����˶�ȡ�Ĵ�����ָ��

�����ֲ�ο���
��2-5 ���ڷ���0x81ָ��֡�ṹ�����Ĵ������ͣ�
���� ָ��֡ͷ ָ��� ָ�� ��ʼ��ַ ���ݳ���
���ȣ��ֽڣ� 2 1 1(0x81) 1 1(N)

��2-6 ���ڽ���0x81ָ��֡�ṹ�����Ĵ������գ����������͵ģ�
���� ָ��֡ͷ ָ��� ָ�� ��ʼ��ַ ���ݳ��� ��������
���ȣ��ֽڣ� 2 1 1(0x81) 1 1(N) N
������������ȡ�Ĵ����Ĵ���0x03��0x04��Ԫ
���ͣ�0xA5 0x5A 0x03 0x81 0x03 0x02
���أ�0xA5 0x5A 0x05 0x81 0x03 0x02 0x00 0x01
***********************************/
void VGUS_Read_Register(uint16_t data[], uint8_t command_length, uint16_t address, uint8_t data_length)
{		
		uint8_t data_buff[8];
		data_buff[0] = 0xA5;
		data_buff[1] = 0x5A;						//ָ��֡ͷ
		data_buff[2] = command_length;	//����ָ���
		data_buff[3] = 0x81;						//��ָ�� 0x81
		data_buff[4] = address; 				//��ʼд���ַ
		data_buff[5] = data_length;
		HAL_UART_Transmit(&huart4, (uint8_t *)data_buff, sizeof(data_buff),0x00ff);
}

/***********************************
���������������洢��дָ���


�����ֲ�ο���
��2-8 ���ڷ���0x82ָ��֡�ṹ��д�����洢����Ԫ��
���� ָ��֡ͷ ָ��� ָ�� ��ʼ��ַ ��������
���ȣ��ֽڣ� 2 1 1(0x82) 2 2N
�����������洢��0x0003��Ԫд��0x00 ��0x01
���ͣ�0xA5 0x5A 0x05 0x82 0x00 0x03 0x00 0x01

***********************************/
uint8_t data_buff[8],i=0;
void VGUS_WriteTostorage(uint16_t data[], uint8_t command_length, uint16_t address)
{		
		
		data_buff[0] = 0xA5;
		data_buff[1] = 0x5A;									//ָ��֡ͷ
		data_buff[2] = command_length;				//����ָ���
		data_buff[3] = 0x82;									//дָ�� 0x82
		data_buff[4] = (address>>8)&0xff; 		//��ʼд���ַ
		data_buff[5] = address&0xff;					//��ַ
		for(i=0; i<(command_length - 3)/2;i++)
		{
			data_buff[i+6] = (data[i]>>8)&0xff;
			data_buff[i+7] = data[i]&0xff;
		}
		HAL_UART_Transmit(&huart4, (uint8_t *)data_buff, sizeof(data_buff),0x00ff);
}
/***********************************
���������������洢����ָ���


�����ֲ�ο���
��2-9 ���ڷ���0x83ָ��֡�ṹ���������洢�����ͣ�
���� ָ��֡ͷ ָ��� ָ�� ��ʼ��ַ ���ݳ���
���ȣ��ֽڣ� 2 1 1(0x83) 2 1(N)
��2-10 ���ڽ���0x83ָ��֡�ṹ���������洢�����գ����������͵ģ�
���� ָ��֡ͷ ָ��� ָ�� ��ʼ��ַ ���ݳ��� ��������
���ȣ��ֽڣ� 2 1 1(0x83) 2 1(N) 2N
������������ȡ�����洢��0x0003��0x0004��Ԫ
���ͣ�0xA5 0x5A 0x04 0x83 0x00 0x03 0x02
���أ�0xA5 0x5A 0x08 0x83 0x00 0x03 0x02 0x00 0x01 0xff 0xff

***********************************/

void VGUS_Read_Storage(uint16_t data[], uint8_t command_length, uint16_t address, uint8_t data_length)
{
		uint8_t data_buff[8];
		data_buff[0] = 0xA5;
		data_buff[1] = 0x5A;	//ָ��֡ͷ
		data_buff[2] = command_length;				//����ָ���
		data_buff[3] = 0x83;									//��ָ�� 0x83
		data_buff[4] = (address>>8)&0xff; 		//��ʼд���ַ
		data_buff[5] = address&0xff;					//��ַ
		data_buff[6] = data_length;
		HAL_UART_Transmit(&huart4, (uint8_t *)data_buff, sizeof(data_buff),0x00ff);
	
}
/***********************************
���������������洢����ָ���
������    ���͵�ѹ���ݸ�������������������Ҫ�����¶ȵ��������һλС������λ����	
***********************************/
void Send_Voltage(float voltage, uint16_t address)
{
	uint16_t data_to_send[1];
	/*����ת��*/
	data_to_send[0] = (int)(voltage*10);
	VGUS_WriteTostorage(data_to_send,5,address);
}
/***********************************
���������������洢����ָ���
������    ���͵������ݸ�����������������λ��mA ��˲���ҪС�� ��ʾ����Ϊ����
***********************************/
void Send_Current(uint16_t current, uint16_t address)
{
	uint16_t data_to_send[1];
	/*����ת��*/
	data_to_send[0] = current;
	VGUS_WriteTostorage(data_to_send,5,address);
}

/***********************************
���������������洢����ָ���
������    ����ʱ�����ݸ������� 
************************************/
void Send_Time(uint8_t time, uint16_t address)
{
	uint16_t data_to_send[1];
	/*����ת��*/
	data_to_send[0] = time;
	VGUS_WriteTostorage(data_to_send,5,address);
}

/***********************************
�����������������ݽ���״̬��
������    �����������ص����ݽ��н���	
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



