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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
	LIGHT_TRAFFIC_MODE,
	HEAVY_TRAFFIC_MODE,
	CONTROL_MODE,
	NIGHT_MODE
} TrafficLightMode;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
TrafficLightMode currentMode = LIGHT_TRAFFIC_MODE;
volatile uint8_t normal_mode_flag = 1;
volatile uint8_t heavy_mode_flag = 0;
volatile uint8_t night_mode_flag = 0;
volatile uint8_t control_mode_flag = 0;
volatile uint8_t lastbutton = 0;  // Only use in Control Mode
volatile uint8_t direction = 0; // 0: Master - Slave 2 GREEN, 1: Slave 1 - Slave 3 GREEN
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
uint8_t isButtonPressed(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void updateState(void);
void switchToLightTrafficMode(void);
void switchToHeavyTrafficMode(void);
void switchToControlMode(void);
void switchToNightMode(void);
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
  /* USER CODE BEGIN 2 */
  switchToLightTrafficMode();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  updateState();
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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3
                           PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB5 PB6 PB7 PB8
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/*uint8_t isButtonPressed(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET)
    {
        HAL_Delay(50);  // Thời gian debounce 50ms
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET)
        {
            // Đợi cho đến khi nút được thả ra
            while(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET);
            return 1;
        }
    }
    return 0;
}*/

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

void detect_button(void)
{
	if (isButtonPressed(GPIOB, GPIO_PIN_5)) {
		normal_mode_flag = 1;
		heavy_mode_flag = 0;
		night_mode_flag = 0;
		control_mode_flag = 0;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	}
	else if (isButtonPressed(GPIOB, GPIO_PIN_6)) {
		normal_mode_flag = 0;
		heavy_mode_flag = 1;
		night_mode_flag = 0;
		control_mode_flag = 0;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	}
	else if (isButtonPressed(GPIOB, GPIO_PIN_7)) {
			normal_mode_flag = 0;
			heavy_mode_flag = 0;
			night_mode_flag = 1;
			control_mode_flag = 0;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
	}
	else if (isButtonPressed(GPIOB, GPIO_PIN_8)) {
			normal_mode_flag = 0;
			heavy_mode_flag = 0;
			night_mode_flag = 0;
			control_mode_flag = 1;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	}
    else if (isButtonPressed(GPIOB, GPIO_PIN_9)) {
    	lastbutton = 1;
    	direction = ~direction;
    	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
    }
}

void switchToLightTrafficMode(void)
{
    currentMode = LIGHT_TRAFFIC_MODE;
    // Reset mode_timer mỗi lần gọi hàm
        uint32_t startTime = HAL_GetTick();  // Lấy thời gian bắt đầu

        // Đèn xanh sáng trong 20 giây
        while (HAL_GetTick() - startTime < 20000) {
        	detect_button();
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  // Bật đèn xanh PA10
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);  // Tắt PA8 và PA9
        }

        // Sau khi 2 giây, đèn vàng sáng trong 10 giây
        startTime = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
        while (HAL_GetTick() - startTime < 10000) {
        	detect_button();
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);  // Bật đèn vàng PA9
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_10, GPIO_PIN_RESET);  // Tắt PA8 và PA10
        }

        // Sau 3 giây, đèn đỏ sáng trong 30 giây
        startTime = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
        while (HAL_GetTick() - startTime < 30000) {
        	detect_button();
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);  // Bật đèn đỏ PA8
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_RESET);  // Tắt PA9 và PA10
        }
}

void switchToHeavyTrafficMode(void)
{
    currentMode = HEAVY_TRAFFIC_MODE;

    uint32_t mode_timer = HAL_GetTick();
    while (HAL_GetTick() - mode_timer < 60000) {
    	detect_button();
    	// 60s đèn xanh
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
    }

    mode_timer = HAL_GetTick();
    while (HAL_GetTick() - mode_timer < 10000) {
    	detect_button();
    	// 10s đèn vàng
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_10, GPIO_PIN_RESET);
    }

    mode_timer = HAL_GetTick();
    while (HAL_GetTick() - mode_timer < 70000) {
    	detect_button();
    	// 70s đèn đỏ
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
    }
}

void switchToNightMode(void)
{
    currentMode = NIGHT_MODE;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_10, GPIO_PIN_RESET);
    uint32_t time_now = HAL_GetTick();
    while (HAL_GetTick() - time_now < 1000) {
    	detect_button();
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    }

    time_now = HAL_GetTick();
    while (HAL_GetTick() - time_now < 1000) {
    	detect_button();
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
    }
}

void switchToControlMode(void)
{
    currentMode = CONTROL_MODE;
    if (lastbutton == 0) {
    	if (direction == 0) {
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
    	}
    	else {
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
    	}
    }

    else if (lastbutton != 0) {
    	uint32_t start_time = HAL_GetTick();
    	if (direction == 0) {
    		if (HAL_GetTick() - start_time < 5000) {
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
    		}

    		else if (HAL_GetTick() - start_time < 8000) {
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_10, GPIO_PIN_RESET);
    		}
    		else {
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
    			//lastbutton = 0;
    		}
    	}

    	else if (direction != 0) {
    		if (HAL_GetTick() - start_time < 5000) {
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
    		}
    		else if (HAL_GetTick() - start_time < 8000) {
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_10, GPIO_PIN_RESET);
    		}
    		else {
    		    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    		    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
    		    //lastbutton = 0;
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
