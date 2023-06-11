
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include "Timer.h"
#include "RCC.h"
#include "NVIC.h"
#include "DMA.h"
#include "GPIO.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
TIM_Handle_Type tim2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);                                    
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                
                                

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

void flashWrite(uint32_t *address, uint32_t data){
	//HAL_FLASH_Unlock();
	FLASH_Erase_Sector(FLASH_SECTOR_1, FLASH_VOLTAGE_RANGE_3);  /*!< Device operating range: 2.6V to 3.6V  */
	*address = data;
	//HAL_FLASH_Lock();
}

// write ccr3 value to sector 1 of flash 0x08004000
void flashWriteCCR3(void){
	  uint32_t *flashMem = (uint32_t *)0x08004000;

	  for(int i = 0; i < 60; i++){
		  for(int j = 0; j < 50; j++){
			  flashWrite(flashMem, 6000 + (50 * i));
			  flashMem++;
		  }
	  }
}

void fillInMem(uint32_t Mem[]){
	  for(int i = 0; i < 60; i++){
		  for(int j = 0; j < 50; j++){
			  Mem[j + (50 * i)] = (1500 + (50 * i));
		  }
	  }
}

/*
Memory Preparation: The fillInMem(Mem); function call is used to populate a section of memory (Mem[]) with specific values. This data will later be transferred via DMA to the Timer's Compare Register (CCR).

GPIO Configuration: GPIOConfigurePin and GPIOConfigureAltFunc configure the GPIOB Pin 4 to be used as a PWM output (Alt Function 2 corresponds to the Timer's output).

Timer Configuration: Several lines of code configure Timer3 for PWM output. Timer3's Auto-Reload Register (ARR), Prescaler (PSC), and Counter (CNT) are set, then Output Compare Channel 1 is initialized in PWM mode with a preload value.

DMA Configuration: DMA1 is configured to transfer data from memory to a peripheral (Timer3's CCR1) with dmaStreamConfigure. DMA settings include word-sized data, memory-to-peripheral direction, the channel selection for the stream, and memory increment and circular mode. In circular mode, when the memory data has all been transferred, DMA will automatically wrap back to the start of the buffer, making it ideal for applications like waveform generation.

DMA Stream Setting: It sets the number of data items to be transferred (NDTR), the peripheral address (PAR - in this case, Timer3's CCR1), and the memory address (M0AR - the memory buffer Mem).

DMA Activation: It then enables the DMA stream and configures Timer3 to use DMA for Capture/Compare 1 event. When the Timer hits the compare match event, it will trigger the DMA to transfer a word of data from memory to the CCR1.

Timer Activation: It finally enables the Timer's Capture/Compare for Channel 1 and the Timer's counter.

Once the function is called, the PWM signal generated on the GPIO pin will vary in duty cycle according to the data values filled into Mem[]. This data is continually fed from memory to the Timer's CCR via DMA, without the need for CPU intervention.

This demo hence shows the use of DMA for generating a complex PWM waveform, illustrating how CPU resources can be saved in a real-time application.

*/
void demoTimer3PMW(void){
	uint32_t Mem[3000] = {0};
	fillInMem(Mem);

	GPIOConfigurePin(GPIOB, GPIOPin4, GPIO_ALT_FUNC|GPIO_PUSH_PULL|GPIO_VERY_HI_SPEED|GPIO_NO_PULL);
	GPIOConfigureAltFunc(GPIOB, GPIOPin4, AF2);  

	RESET_TIMER_3_CLK_GATING();
	UNRESET_TIMER_3_CLK_GATING();
	ENABLE_TIMER_3_CLK_GATING();

	timer3-> arr = 60000;
	timer3-> psc = 29;
	timer3-> cnt = 0;

	timerInitOutputCompare(timer3, 1, OC_PWM1 | CC_PRELOAD_EN , OC_NORMAL_POLARITY, 1500);

	UNRESET_DMA1_CLK_GATING();
	ENABLE_DMA1_CLK_GATING();
	dmaStreamConfigure(dma1, DMA_STREAM4, 						  \
		 		  	  	  	 DMA_PSIZE32 | DMA_MSIZE32 |  DMA_DIR_MtP|   \
		 					 DMA_CH5SEL | DMA_MINC | DMA_CIRC);

	dma1->S[DMA_STREAM4].NDTR = 3000;
	dma1->S[DMA_STREAM4].PAR = (uint32_t *)(0x40000400 + 0x34);	// address of CCR1
	dma1->S[DMA_STREAM4].M0AR = (uint32_t *)(&Mem); // address of sector 1 of flash


	// Enable dma1 stream1
	DMA_STREAM_ENABLE(dma1, DMA_STREAM4);
	// Enable timer 3 dma
	TIM_ENABLE_DMA(timer3, TIM_CC1DE);
	// Enable timer3 capture compare channel 1
	TIM_CAPTURE_COMPARE_ENABLE(timer3, 1);
	// Enable counter timer
	TIM_COUNTER_ENABLE(timer3);
	// Initialise the timer2

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  //MX_TIM2_Init();
  //MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  //*********custom functions (timer)***************





  nvicEnableInterrupt(28);
  TIM_INTERRUPT_ENABLE(timer2, TIM_UIE);


  nvicDisableInterrupt(45);


  nvicEnableInterrupt(12);
  nvicDisableInterrupt(12);



  //int n = nvicIsInterruptActive(45);

  //***********HAL functions*************************
  //HAL_TIM_Base_Start(&htim2);
  //HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_3);

  //HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
  //HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 0);

  //__HAL_TIM_SET_COUNTER(&htim2, 0);
  //__HAL_TIM_CLEAR_FLAG(&htim2, TIM_IT_UPDATE);
  //__HAL_TIM_CLEAR_FLAG(&htim2, TIM_IT_CC3);
  demoTimer3PMW();// configure ccds = 1 , cr2, arr


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {



	 // if(__HAL_TIM_GET_FLAG(&htim2,TIM_FLAG_UPDATE)){
		//  __HAL_TIM_CLEAR_FLAG(&htim2,TIM_IT_UPDATE);
	//	  HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
	 // }

//  if(__HAL_TIM_GET_FLAG(&htim2,TIM_FLAG_CC3)){
	//	  __HAL_TIM_CLEAR_FLAG(&htim2,TIM_FLAG_CC3);
	//	  HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
	//  }

	/*  if(TIM_GET_FLAG(timer2,TIM_FLAG_UPDATE)){
		  TIM_CLEAR_FLAG(timer2,TIM_IT_UPDATE);
		HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
		  }

	  if(TIM_GET_FLAG(timer2,TIM_FLAG_CC3)){
		  TIM_CLEAR_FLAG(timer2,TIM_FLAG_CC3);
		  HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		  }*/
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }

  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = TIMER2_DIVIDER;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = TIMER2_AR;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_INACTIVE;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim2);

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim3);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, LED3_Pin|LED4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED3_Pin LED4_Pin */
  GPIO_InitStruct.Pin = LED3_Pin|LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
