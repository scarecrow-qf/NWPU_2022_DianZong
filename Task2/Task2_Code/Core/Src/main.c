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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "max31865.h"
#include "VGUS.h"
#include "beep.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define hour_address 			0x25
#define min_address 			0x29
#define second_address 		0x33
#define tempture_address 	0x0020
#define alarm_tempture_address 0x0090
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/*测试变量*/
int test_flag = 0;
uint16_t test_adress = 0x0020;
float test_data = 0;
/*会用到的全局变量*/
double tempture,warn_temp = 100;																//温度
uint8_t second = 0x00, min = 0x04, hour = 0x13,warn_time = 0;		//时间
uint8_t is_warning;																							//报警标志位
int time = 0,bee_times=0;																				//时间

uint16_t History_first_pos = 0x40;															//历史记录第一个开始的地址
/*串口接收变量*/
struct Receive uart_receive = {0};
struct Receive_data uart_data = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

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
  MX_TIM3_Init();
  MX_DMA_Init();
  MX_UART4_Init();
  MX_TIM4_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
	MAX31865_Init();
	MAX31865_Cfg();
	
	/* 定时器TIM3开启中断 */
	__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);							/* 清除定时器初始化过程中的更新中断标志，避免定时器一启动就中断，*/
	HAL_TIM_Base_Start_IT(&htim3);													/* 使能定时器3更新中断并启动定时器*/
	
	/* 定时器TIM4开启中断 */
	__HAL_TIM_CLEAR_IT(&htim4 , TIM_IT_UPDATE);	
	HAL_TIM_Base_Start_IT(&htim4);
	
	/*使能空闲中断，并使用receive_buff接收串口发送过来的数据*/
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart4, (uint8_t*)uart_receive.receive_buff, 50);     //设置DMA传输，讲串口1的数据搬运到recvive_buff中，
  
	/* 设置初始报警温度 可修改*/
	Send_Temperature(warn_temp,alarm_tempture_address);
		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		tempture=MAX31865_GetTemp() + 1;   //采样得到温度
		
		/*    设置报警温度    */
		if(uart_receive.data_Receive_done)
		{
			uart_data  = analysis_VGUS_data(uart_receive.receive_data);
			/*		判断是哪一个按键    */
			if(uart_data.data[1] == 0xA1)//-10°
				warn_temp -= 10;
			else if(uart_data.data[1] == 0xB2)
				warn_temp -= 1;
			else if(uart_data.data[1] == 0xC3)
				warn_temp += 1;
			else if(uart_data.data[1] == 0xD4)
				warn_temp += 10;
			/*		更新显示数据			*/
			Send_Temperature(warn_temp,alarm_tempture_address);
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

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* UART4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(UART4_IRQn);
}

/* USER CODE BEGIN 4 */
//定时器中断函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim3.Instance)
	{
		time++;		//每100ms，time加1
		//每隔1ms获取一次MAX31865数值记录当前温度
		if((tempture>warn_temp) && (tempture < 300))
		{
			if(!is_warning)
			{
				is_warning = 1;
				//发送历史报警温度
				
				Send_Temperature(tempture,History_first_pos + 0x30);
				Send_Time(second,History_first_pos + 0x20);
				Send_Time(min,   History_first_pos + 0x10);
				Send_Time(hour,  History_first_pos);
				History_first_pos += 0x02;									//每次发送完毕，历史报警存储地址增加两位
				if (History_first_pos >= 0x4A) 							//如果历史报警位置超过0x4A，回到原位置，覆盖最早的报警信息
					History_first_pos = 0x40;
			}
		}
		else if(is_warning)
		{
			if(warn_time>=50)
			{
				is_warning = 0;
				bee_times = 0;
				BEEP_OFF();
				warn_time = 0;
			}
		}
		
		if(time ==10)
		{	
			/*	时间计算	*/	
			time = 0;
			second++;
			if(second >= 60)
			{
				second = 0;
				min++;
				if(min >= 60)
				{
					min = 0;
					hour++;
				}	
			}
			
			/*	max31865相关信息	*/	
			//发送温度信息
			Send_Temperature(tempture,tempture_address);
			//发送时间信息
			Send_Time(hour,hour_address);
			Send_Time(min,min_address);
			Send_Time(second,second_address);
		}
		
	}
	
	
	if(htim->Instance == htim4.Instance)
	{
		if((!bee_times)&&(is_warning))	//如果蜂鸣器响次数为0且在报警状态
		{	
			warn_time++;
			//GPIO输出1
			BEEP_ON();
			if(warn_time >= 50)
			{
				//GPIO输出0
				BEEP_OFF();
				bee_times = 1;							//蜂鸣器响次数为1
				//warn_time = 0;
			}
		}
	}
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
