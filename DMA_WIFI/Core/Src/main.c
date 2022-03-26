/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stdio.h"
#include "ESP8266.h"
//#include "../../MyApplication/MyApplication.h"
//#include "MyApplication.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//uint8_t Rxbuf[40];
//uint8_t ACKbuf[] = "I'm a ACK package";
int isSerialPort = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void UART_receive_DMA(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*int delayTime = 220;*/
/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == KEY0_Pin) {
        /*if (delayTime < 12000) delayTime *= 2;
		else delayTime = 220;*/
		if (!Esp8266HasInited) return;
		if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == 1) {
			HAL_Delay(30);
			isSerialPort = !isSerialPort;
			HAL_GPIO_TogglePin(GPIOF, LED1_Pin);
		}
		HAL_Delay(30);
    }
}

/**
 * @description: 以阻塞的方式延迟微妙
 * @param {uint32_t} t 延迟时间
 * @return {*}
 */
void Delay_us(uint32_t t) {
    __IO uint32_t Delay = t * 72 / 8;//(SystemCoreClock / 8U / 1000000U)
    //见stm32f1xx_hal_rcc.c -- static void RCC_Delay(uint32_t mdelay)
  do {
    __NOP();
  } while (Delay --);
}

/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {                 //处理TIM2间隔定时中断
//        HAL_GPIO_TogglePin(GPIOF, LED1_Pin);
		HAL_GPIO_TogglePin(GPIOF, LED0_Pin);
    }
}

   /*
    * reads at most one less than the number of characters specified by n from
    * the stream pointed to by stream into the array pointed to by s. No
    * additional characters are read after a new-line character (which is
    * retained) or after end-of-file. A null character is written immediately
    * after the last character read into the array.
    * Returns: s if successful. If end-of-file is encountered and no characters
    *          have been read into the array, the contents of the array remain
    *          unchanged and a null pointer is returned. If a read error occurs
    *          during the operation, the array contents are indeterminate and a
    *          null pointer is returned.
    */
int fputc(int ch, FILE* f) {
	//通过查询的方式循环发song
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0x000A);
	return ch;
}

   /*
    * obtains the next character (if present) as an unsigned char converted to
    * an int, from the input stream pointed to by stream, and advances the
    * associated file position indicator (if defined).
    * Returns: the next character from the input stream pointed to by stream.
    *          If the stream is at end-of-file, the end-of-file indicator is
    *          set and fgetc returns EOF. If a read error occurs, the error
    *          indicator is set and fgetc returns EOF.
    */
int fgetc(FILE *f) {
	uint8_t ch = 0;
	HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
	//HAL_UART_Receive_DMA(&huart1, &ch, 1);
	return ch;
}

/*void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
	if (UartHandle->Instance == USART1) {
		HAL_UART_Transmit_DMA(&huart1, Rxbuf, sizeof(Rxbuf));
		HAL_UART_Receive_DMA(&huart1, Rxbuf, sizeof(Rxbuf));
		//HAL_UART_Transmit_IT(&huart1, Rxbuf, sizeof(Rxbuf));
		//HAL_UART_Receive_IT(&huart1, Rxbuf, sizeof(Rxbuf));
	}
}*/
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim2);        //启动定时器TIM2
	//HAL_UART_Receive_DMA(&huart1, Rxbuf, sizeof(Rxbuf));
	//HAL_UART_Receive_IT(&huart1, Rxbuf, sizeof(Rxbuf));
	//MyInit.Peripheral_Set();
	ESP8266_ModeInit();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_GPIO_WritePin(GPIOF, LED1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOF, LED0_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOF, BEEP_Pin, GPIO_PIN_RESET);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//while (HAL_GPIO_ReadPin(GPIOE, KEY0_Pin));
		//HAL_GPIO_TogglePin(GPIOF, LED1_Pin);
		//HAL_GPIO_TogglePin(GPIOF, LED0_Pin);
		//HAL_GPIO_TogglePin(GPIOF, BEEP_Pin);
	  //HAL_Delay(delayTime);
	  //HAL_GPIO_TogglePin(GPIOF, BEEP_Pin);
	  //Delay_us(1.0/delayTime*500000);
		UART_receive_DMA();
	  //System.Run();
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 64;
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
