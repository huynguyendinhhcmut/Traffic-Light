/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include "7LedSeg.h"
#include "HC12.h"
#include <stdio.h>



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define SEVEN_LED_SEG_SPI &hspi2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

extern SPI_HandleTypeDef hspi2;
volatile uint8_t normal_mode_flag = 0;
volatile uint8_t heavy_mode_flag = 0;
volatile uint8_t night_mode_flag = 0;
volatile uint8_t control_mode_flag = 0;
volatile uint8_t lastbutton = 0;  // Only use in Control Mode
volatile uint8_t direction = 0; // 0: Master - Slave 2 GREEN, 1: Slave 1 - Slave 3 RED
volatile uint8_t min=0;

// Numbers

const uint8_t digitCodes[11] = {
    ~0x3F,  // 0 -> 0b00111111
    ~0x06,  // 1 -> 0b00000110
    ~0x5B,  // 2 -> 0b01011011
    ~0x4F,  // 3 -> 0b01001111
    ~0x66,  // 4 -> 0b01100110
    ~0x6D,  // 5 -> 0b01101101
    ~0x7D,  // 6 -> 0b01111101
    ~0x07,  // 7 -> 0b00000111
    ~0x7F,  // 8 -> 0b01111111
    ~0x6F,  // 9 -> 0b01101111
	~0x00   // n -> 0b00000000
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */


uint8_t isButtonPressed(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void updateState(void);
void switchToLightTrafficMode(void);
void switchToHeavyTrafficMode(void);
void switchToControlMode(void);
void switchToNightMode(void);
void detect_mode(void);


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
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  HC12_Init(); // Initialize UART interrupts

  //char init_msg[] = "UART Initialized\r\n";
  //HAL_UART_Transmit(&huart2, (uint8_t*)init_msg, strlen(init_msg), 100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  // Process mode changes
	  detect_mode();

	  HAL_Delay(50);		// Time to get new mode

	  updateState();
/*
          HC12_Mode received_mode = HC12_GetReceivedMode();

          // Debug: Transmit received mode if valid
          if (received_mode != HC12_MODE_UNKNOWN)
          {
              char debug_msg[50];
              snprintf(debug_msg, sizeof(debug_msg), "Received: %s, Mode: %d\r\n",
                       HC12_ModeToStr(received_mode), received_mode);
              HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), 100);
          }
*/

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */





uint8_t isButtonPressed(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{

    static uint32_t lastDebounceTime = 0;
    uint32_t debounceDelay = 50; // Thời gian debounce 50ms

    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET)
    {
        uint32_t currentTime = HAL_GetTick();
        if ((currentTime - lastDebounceTime) >= debounceDelay)
        {
            lastDebounceTime = currentTime;
            // Đợi cho đến khi nút được thả ra
            while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET);
            return 1;
        }
    }
    return 0;
}

// Function to send data to the shift register via SPI
void shiftOut(uint8_t data) {
    HAL_GPIO_WritePin(NSS_PORT, NSS_PIN, GPIO_PIN_RESET);  				// Pull NSS low to start transmission
    HAL_SPI_Transmit(SEVEN_LED_SEG_SPI, &data, 1, HAL_MAX_DELAY);  		// Send data
    HAL_GPIO_WritePin(NSS_PORT, NSS_PIN, GPIO_PIN_SET);    				// Pull NSS high to end transmission
}




// Function to display a two-digit number
void displayNumber(uint8_t number) {
    if (number > 99) return;  			// Limit to 2 digits

    uint8_t tens = number / 10;      	// Extract tens digit
    uint8_t units = number % 10;     	// Extract units digit

    // Send tens digit to first 7-segment display
    shiftOut(digitCodes[units]);
    HAL_Delay(50);  // Small delay to stabilize display

    // Send units digit to second 7-segment display
    shiftOut(digitCodes[tens]);
    HAL_Delay(50);  // Small delay to stabilize display
}



// Function to CountDown number
void countDownNum(uint8_t number){
	// Loop displaying number
	for (int8_t i = number; i >= 0; i--) {
		uint32_t start_time = HAL_GetTick();
		displayNumber(i);      	// Display xx to 00
		while (HAL_GetTick() - start_time < 1000) {
			HAL_Delay(50);
		}
	}
}


void updateState(void)
{
    if (normal_mode_flag)
    {
        switchToLightTrafficMode();
    }
    else if (heavy_mode_flag)
    {
        switchToHeavyTrafficMode();
    }
    else if (night_mode_flag)
    {
        switchToNightMode();
    }
    else if (control_mode_flag)
    {
        switchToControlMode();
    }
}



void detect_mode(void)
{
	HC12_Mode received_mode = HC12_IntReceivedMode();

	    if (received_mode == HC12_MODE_UNKNOWN)
	        return;

	    // Set flags based on received mode
	    switch (received_mode)
	    {
	        case HC12_MODE_NORMAL:
	            normal_mode_flag = 1;
	            heavy_mode_flag = 0;
	            night_mode_flag = 0;
	            control_mode_flag = 0;
	            lastbutton = 0;
	            direction = 0;
	            min = 0;
	            break;

	        case HC12_MODE_HEAVY:
	            normal_mode_flag = 0;
	            heavy_mode_flag = 1;
	            night_mode_flag = 0;
	            control_mode_flag = 0;
	            lastbutton = 0;
	            direction = 0;
	            min = 0;
	            break;

	        case HC12_MODE_NIGHT:
	            normal_mode_flag = 0;
	            heavy_mode_flag = 0;
	            night_mode_flag = 1;
	            control_mode_flag = 0;
	            lastbutton = 0;
	            direction = 0;
	            min = 0;
	            break;

	        case HC12_MODE_CONTROL:
	            normal_mode_flag = 0;
	            heavy_mode_flag = 0;
	            night_mode_flag = 0;
	            control_mode_flag = 1;
	            lastbutton = 0;
	            direction = direction;
	            min = 0;
	            break;

	        case HC12_MODE_CONTROL_STATE:
	            normal_mode_flag = 0;
	            heavy_mode_flag = 0;
	            night_mode_flag = 0;
	            control_mode_flag = 1;
	            lastbutton = 1;

	            direction = ~direction;
	            min = 0;
	            break;

	        default:
	            break;
	    }




}


void switchToLightTrafficMode(void)
{

    // Reset mode_timer mỗi lần gọi hàm
        uint32_t startTime = HAL_GetTick();  // Lấy thời gian bắt đầu


        // Đèn đỏ sáng trong 25 giây
        while (HAL_GetTick() - startTime < 26000) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  // Bật đèn đỏ PA10
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);  // Tắt PA8 và PA9
            countDownNum(25);
            HAL_Delay (1000);
        }

        // đèn xanh sáng trong 20 giây
        startTime = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
        while (HAL_GetTick() - startTime < 21000) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);  // Bật đèn xanh PA8
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_RESET);  // Tắt PA9 và PA10
            countDownNum(20);
        }

        // đèn vàng sáng trong 5 giây
        startTime = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
        while (HAL_GetTick() - startTime < 7000) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);  // Bật đèn vàng PA9
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_10, GPIO_PIN_RESET);  // Tắt PA8 và PA10
            countDownNum(6);
        }


}

void switchToHeavyTrafficMode(void)
{

    uint32_t mode_timer = HAL_GetTick();

    // Đèn đỏ sáng trong 65 giây
    while (HAL_GetTick() - mode_timer < 67000) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  // Bật đèn đỏ PA10
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);  // Tắt PA8 và PA9
        countDownNum(65);
    }

    // đèn xanh sáng trong 60 giây
    mode_timer = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
    while (HAL_GetTick() - mode_timer < 61000) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);  // Bật đèn xanh PA8
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_RESET);  // Tắt PA9 và PA10
        countDownNum(60);
    }

    // đèn vàng sáng trong 5 giây
    mode_timer = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
    while (HAL_GetTick() - mode_timer < 10000) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);  // Bật đèn vàng PA9
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_10, GPIO_PIN_RESET);  // Tắt PA8 và PA10
        countDownNum(9);
    }
}

void switchToNightMode(void)
{
	shiftOut(digitCodes[10]);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_10, GPIO_PIN_RESET);
    uint32_t time_now = HAL_GetTick();
    while (HAL_GetTick() - time_now < 500) {
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    }

    time_now = HAL_GetTick();
    while (HAL_GetTick() - time_now < 500) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
    }
}




void switchToControlMode(void)
{

    if (lastbutton == 0) {
    	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    	shiftOut(digitCodes[10]);
    	if (direction == 0) {
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10|GPIO_PIN_9, GPIO_PIN_RESET);
    	}
    	else {
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
    	}
    }

    else if (lastbutton == 1) {
    	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
    	if (direction == 0) {

    		uint32_t start_time  = HAL_GetTick();
    		lastbutton = 0;
    		while (HAL_GetTick() - start_time < 8000) {
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_8, GPIO_PIN_RESET);
    			countDownNum(8);
    		}
    	}

    	else if (direction !=0) {

    		lastbutton = 0;
    	    uint32_t start_time  = HAL_GetTick();
    	    while (HAL_GetTick() - start_time < 5000) {
    	    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    	    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10|GPIO_PIN_9, GPIO_PIN_RESET);
    	    	countDownNum(5);
    	    }

    	    start_time = HAL_GetTick();
    	    while (HAL_GetTick() - start_time < 3000) {
    	    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    	    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_10, GPIO_PIN_RESET);
    	    	countDownNum(3);
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
