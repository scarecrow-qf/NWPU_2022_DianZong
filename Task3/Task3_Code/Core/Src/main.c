/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "VGUS.h"
#include "switch.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define vout_address 0x0030

#define CAL_I0_K	3300.0//������������Kֵ
#define CAL_I0_B	0//������������Bֵ
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t ADC1_RESULT[100]={0};//ADC�������赽�ڴ��DMA���ݱ���Ĵ���
uint32_t Value;   //�洢ƽ��ֵ
float I0_Value=0;

/*���ڽ��ձ���*/
struct Receive uart_receive = {0};
struct Receive_data uart_data = {0};

double   Vr0 = 0;
uint16_t Ir0[1] = {0};
uint16_t flag_test = 0;

double Vdac = 1.167,Vout = 5;	//�����ѹģ����1.16 
int32_t   Vdac_dig = 1444;//�����ѹ������������ʼֵ��������Ҫ����1448

int change = 0, power = 0;

/*��ʱ�����ָ����������ʱ����Ҫ�޸�*/
uint8_t back_on_buff[7]={0xA5,0x5A,0x04,0x80,0x03,0x00,0x01};			//���������棨��Դ����ָ��
uint8_t back_off_buff[7]={0xA5,0x5A,0x04,0x80,0x03,0x00,0x02};			//���������棨��Դ�أ�ָ��
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void ADC_Dxpd(void);
double ADC_Average(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_DAC_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim3);//������ʱ��
	//HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC1_RESULT, 100);//����ADC1���� DMA���ݴ��Ͳ������������ѹ����

	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Vdac_dig);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	
	Send_Voltage(Vout,vout_address);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(uart_receive.data_Receive_done)
		{
			uart_data  = analysis_VGUS_data(uart_receive.receive_data);
			/*		�ж�����һ������    */
			if(uart_data.data[1] == 0xA0)//
			{
				power = 0;//�򿪵�Դ
				HAL_UART_Transmit(&huart4, back_on_buff, sizeof(back_on_buff),0x00ff);
				SWITCH_ON();
			}
			else if(uart_data.data[1] == 0xA1)
			{
				power = 1;//�رյ�Դ
				HAL_UART_Transmit(&huart4, back_off_buff, sizeof(back_off_buff),0x00ff);
				SWITCH_OFF();
			}
			else if(uart_data.data[1] == 0xB1){
				change = 1;//��ѹ�ϵ�
			}
			else if(uart_data.data[1] == 0xB2){
				change = -1;//��ѹ�µ�
			}
			else if(uart_data.data[1] == 0xC0)
			{
				if(power == 0)
				{
					HAL_UART_Transmit(&huart4, back_on_buff, sizeof(back_on_buff),0x00ff);
				}
				else if(power == 1)
				{
					HAL_UART_Transmit(&huart4, back_off_buff, sizeof(back_off_buff),0x00ff);
				}
			}
			if(change != 0)
			{
				Vout = 0.1*change + Vout;
				Vdac = -0.1*change/2.99 +Vdac;
				Vdac_dig = (int32_t)(Vdac*4096/3.3);
				HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Vdac_dig);
				change = 0;
			}
			/*		������ʾ����			*/
					//�����µĵ�ѹ����
			Send_Voltage(Vout,vout_address);
			uart_receive.data_Receive_done = 0;
		}

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*
** ===================================================================
**     Funtion Name :   void ADCSample(void)
**     Description :    �����������
**     Parameters  :
**     Returns     :
** ===================================================================
*/
void ADCSample(void)
{
	//�����������������ͣ����Լ���ƽ��ֵ
	static double I0AvgSum=0;
	
	Value = 0;
	
//	ADC_Dxpd();
//	Value = ADC_Average();
	for(int i=0;i<100;i++)
			Value += ADC1_RESULT[i];			//100�������ۼӺ�
	Value = Value  / 100.0;					//ȡƽ��ֵ
	
	//��DMA�������л�ȡ����,������������Խ���-�������Խ���
	
	I0_Value  = Value*33.0/40.96;
	if(I0_Value<=1)//8
		I0_Value = 0;//��������ƫ�룬����ֵ��Сʱ��ֱ��Ϊ0
	
	//�����������ֵ��ƽ��ֵ-����ƽ����ʽ
	I0AvgSum = I0AvgSum + I0_Value -(I0AvgSum/4);//��ͣ�������һ���µĲ���ֵ��ͬʱ��ȥ֮ǰ��ƽ��ֵ��
	Ir0[0] = I0AvgSum/0.4 ;
	if(Ir0[0]<=10)
		Ir0[0] = 0;

//	if(Vout>7)
//		Ir0[0]+=10;
//	else if(Vout<4)
//		Ir0[0]-=10;
	
	//Vr0=V0_Value*3300.0/4096.0;  //ADC���ֵΪ4096������3300mV
	//Ir0[0] = Vr0 / 0.1;		//I = V/R V��λ��mv R��λ��ŷķ I��λ��mA
}



/*
** ===================================================================
**     Funtion Name :int fputc(int ch, FILE *f)
**     Description :    fput���¶��壬��printf���¶��򵽴���һ���
**     Parameters  :
**     Returns     :
** ===================================================================
*/
int fputc(int ch, FILE *f)
 
{
 
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
 
  return ch;
 
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
