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

#define CAL_I0_K	3300.0//采样电流矫正K值
#define CAL_I0_B	0//采样电流矫正B值
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t ADC1_RESULT[100]={0};//ADC采样外设到内存的DMA数据保存寄存器
uint32_t Value;   //存储平均值
float I0_Value=0;

/*串口接收变量*/
struct Receive uart_receive = {0};
struct Receive_data uart_data = {0};

double   Vr0 = 0;
uint16_t Ir0[1] = {0};
uint16_t flag_test = 0;

double Vdac = 1.167,Vout = 5;	//输出电压模拟量1.16 
int32_t   Vdac_dig = 1444;//输出电压数字量——初始值的设置需要调整1448

int change = 0, power = 0;

/*临时定义的指令，代码调整的时候需要修改*/
uint8_t back_on_buff[7]={0xA5,0x5A,0x04,0x80,0x03,0x00,0x01};			//返回主界面（电源开）指令
uint8_t back_off_buff[7]={0xA5,0x5A,0x04,0x80,0x03,0x00,0x02};			//返回主界面（电源关）指令
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
	HAL_TIM_Base_Start_IT(&htim3);//启动定时器
	//HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC1_RESULT, 100);//启动ADC1采样 DMA数据传送采样输入输出电压电流

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
			/*		判断是哪一个按键    */
			if(uart_data.data[1] == 0xA0)//
			{
				power = 0;//打开电源
				HAL_UART_Transmit(&huart4, back_on_buff, sizeof(back_on_buff),0x00ff);
				SWITCH_ON();
			}
			else if(uart_data.data[1] == 0xA1)
			{
				power = 1;//关闭电源
				HAL_UART_Transmit(&huart4, back_off_buff, sizeof(back_off_buff),0x00ff);
				SWITCH_OFF();
			}
			else if(uart_data.data[1] == 0xB1){
				change = 1;//电压上调
			}
			else if(uart_data.data[1] == 0xB2){
				change = -1;//电压下调
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
			/*		更新显示数据			*/
					//发送新的电压数据
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
**     Description :    采样输出电流
**     Parameters  :
**     Returns     :
** ===================================================================
*/
void ADCSample(void)
{
	//输入输出采样参数求和，用以计算平均值
	static double I0AvgSum=0;
	
	Value = 0;
	
//	ADC_Dxpd();
//	Value = ADC_Average();
	for(int i=0;i<100;i++)
			Value += ADC1_RESULT[i];			//100次数字累加和
	Value = Value  / 100.0;					//取平均值
	
	//从DMA缓冲器中获取数据,并对其进行线性矫正-采用线性矫正
	
	I0_Value  = Value*33.0/40.96;
	if(I0_Value<=1)//8
		I0_Value = 0;//采样有零偏离，采样值很小时，直接为0
	
	//计算各个采样值的平均值-滑动平均方式
	I0AvgSum = I0AvgSum + I0_Value -(I0AvgSum/4);//求和，新增入一个新的采样值，同时减去之前的平均值。
	Ir0[0] = I0AvgSum/0.4 ;
	if(Ir0[0]<=10)
		Ir0[0] = 0;

//	if(Vout>7)
//		Ir0[0]+=10;
//	else if(Vout<4)
//		Ir0[0]-=10;
	
	//Vr0=V0_Value*3300.0/4096.0;  //ADC最大值为4096，代表3300mV
	//Ir0[0] = Vr0 / 0.1;		//I = V/R V单位是mv R单位是欧姆 I单位是mA
}



/*
** ===================================================================
**     Funtion Name :int fputc(int ch, FILE *f)
**     Description :    fput重新定义，将printf重新定向到串口一输出
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
