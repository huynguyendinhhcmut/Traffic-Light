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
#include "i2c-lcd.h"
#include "HC12.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t dayofweek;
	uint8_t dayofmonth;
	uint8_t month;
	uint8_t year;
} Time;

Time time;

char buffer[20];


#define DS3231_ADDRESS 0x68 << 1


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SEVEN_LED_SEG_SPI &hspi2
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

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
static void MX_I2C2_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
uint8_t isButtonPressed(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void updateState(void);
void switchToLightTrafficMode(void);
void switchToHeavyTrafficMode(void);
void switchToControlMode(void);
void switchToNightMode(void);
void DS3231_Enable_Alarm1_OncePerSecond(I2C_HandleTypeDef *hi2c);
void detect_button(void);

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
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */



  HAL_Delay(300);			// Wait 100ms for LCD to power up properly

  lcd_init();


  // Enable Alarm 1 every second via INT/SQW
  DS3231_Enable_Alarm1_OncePerSecond(&hi2c2);


  // Sec | Min | Hour | DayOfWeek | DayOfMonth | Month | Year)
  // Set time on DS3231: 9:28:10, 3nd day of the week, 4th day of the month, April 2025
  //Set_Time(10, 28, 9, 3, 4, 5, 25);




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	 detect_button();

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
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

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

  /*Configure GPIO pin : SQW_Pin */
  GPIO_InitStruct.Pin = SQW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SQW_GPIO_Port, &GPIO_InitStruct);

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
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

uint8_t isButtonPressed(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{

    static uint32_t lastDebounceTime = 0;
    uint32_t debounceDelay = 50; // Thời gian debounce 50ms

    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET)
    {
        uint32_t currentTime = HAL_GetTick();
        if ((currentTime - lastDebounceTime) >= debounceDelay)
        {
            lastDebounceTime = currentTime;
            // Đợi cho đến khi nút được thả ra
            while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET);
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
			detect_button();
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



void detect_button(void)
{
	if (isButtonPressed(GPIOB, GPIO_PIN_5) || (min == 4)) {
		normal_mode_flag = 1;
		heavy_mode_flag = 0;
		night_mode_flag = 0;
		control_mode_flag = 0;
		lastbutton = 0;
		direction = 0;
		min = 0;
	}
	else if (isButtonPressed(GPIOB, GPIO_PIN_6) || (min == 2)) {
		normal_mode_flag = 0;
		heavy_mode_flag = 1;
		night_mode_flag = 0;
		control_mode_flag = 0;
		lastbutton = 0;
		direction = 0;
		min = 0;
	}
	else if (isButtonPressed(GPIOB, GPIO_PIN_7) || (min == 1) ) {
		normal_mode_flag = 0;
		heavy_mode_flag = 0;
		night_mode_flag = 1;
		control_mode_flag = 0;
		lastbutton = 0;
		direction = 0;
		min = 0;
	}
	else if (isButtonPressed(GPIOB, GPIO_PIN_8)) {
		normal_mode_flag = 0;
		heavy_mode_flag = 0;
		night_mode_flag = 0;
		control_mode_flag = 1;
		lastbutton = 0;
		direction = 0;
		min = 0;
	}
	else if (isButtonPressed(GPIOB, GPIO_PIN_9) && control_mode_flag) {
    	normal_mode_flag = 0;
    	heavy_mode_flag = 0;
    	night_mode_flag = 0;
    	control_mode_flag = 1;
    	lastbutton = 1;
    	direction = ~direction;
    	min = 0;
    }
}



void switchToLightTrafficMode(void)
{

    // Reset mode_timer mỗi lần gọi hàm
        uint32_t startTime = HAL_GetTick();  // Lấy thời gian bắt đầu
        lcd_put_cur (0,9);
        lcd_send_string("LIGHT  ");			// Display MODE

        HC12_SendMode(HC12_MODE_NORMAL);

        // Dèn xanh sáng trong 20 giây
        startTime = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
        while (HAL_GetTick() - startTime < 21000) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);  // Bật đèn xanh PA8
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_RESET);  // Tắt PA9 và PA10
            countDownNum(20);
        }

        // đèn vàng sáng trong 5 giây
        startTime = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
        while (HAL_GetTick() - startTime < 6000) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);  // Bật đèn vàng PA9
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_10, GPIO_PIN_RESET);  // Tắt PA8 và PA10
            countDownNum(5);
        }

        // Đèn đỏ sáng trong 25 giây
        startTime = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
        while (HAL_GetTick() - startTime < 26000) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  // Bật đèn đỏ PA10
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);  // Tắt PA8 và PA9
            countDownNum(25);
            HAL_Delay (1000);
        }



}



void switchToHeavyTrafficMode(void)
{

    uint32_t mode_timer = HAL_GetTick();
    lcd_put_cur (0,9);
    lcd_send_string("HEAVY  ");			// Display MODE

    HC12_SendMode(HC12_MODE_HEAVY);

    // Dèn xanh sáng trong 60 giây
    while (HAL_GetTick() - mode_timer < 61000) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);  // Bật đèn xanh PA8
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_RESET);  // Tắt PA9 và PA10
        countDownNum(60);
    }

    // đèn vàng sáng trong 5 giây
    mode_timer = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
    while (HAL_GetTick() - mode_timer < 6000) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);  // Bật đèn vàng PA9
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_10, GPIO_PIN_RESET);  // Tắt PA8 và PA10
        countDownNum(5);
    }

    // Đèn đỏ sáng trong 65 giây
    mode_timer = HAL_GetTick();  // Cập nhật lại thời gian bắt đầu
    while (HAL_GetTick() - mode_timer < 66000) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  // Bật đèn đỏ PA10
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);  // Tắt PA8 và PA9
        countDownNum(65);
        HAL_Delay (1000);
    }
}



void switchToNightMode(void)
{

    lcd_put_cur (0,9);
    lcd_send_string("NIGHT  ");			// Display MODE

    HC12_SendMode(HC12_MODE_NIGHT);

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

    lcd_put_cur (0,9);
    lcd_send_string("CONTROL");			// Display MODE

    HC12_SendMode(HC12_MODE_CONTROL);

    if (lastbutton == 0) {
    	shiftOut(digitCodes[10]);
    	if (direction == 0) {
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
    	}
    	else {
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
    	}
    }

    else if (lastbutton == 1) {
    	if (direction == 0) {
    		for (int i = 0; i < 100; i++) {
    		HC12_SendMode(HC12_MODE_CONTROL_STATE);
    		}
    		uint32_t start_time  = HAL_GetTick();
    		lastbutton = 0;
    		while (HAL_GetTick() - start_time < 5000) {
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
    			countDownNum(5);
    		}

    		start_time  = HAL_GetTick();
    		while (HAL_GetTick() - start_time < 3000) {
    		    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    		    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_10, GPIO_PIN_RESET);
    		    countDownNum(3);
    		}
    	}

    	else if (direction != 0) {
    		for (int i = 0; i < 100; i++) {
    		HC12_SendMode(HC12_MODE_CONTROL_STATE);
    		}

    		lastbutton = 0;
    	    uint32_t start_time  = HAL_GetTick();
    	    while (HAL_GetTick() - start_time < 8000) {
    	    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    	    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
    	    	countDownNum(8);
    	    }
    	}
    }
}




// Convert normal decimal numbers to binary coded decimal
uint8_t decToBcd(int val)
{
  return (uint8_t)( (val/10*16) + (val%10) );
}


// Convert binary coded decimal to normal decimal numbers
int bcdToDec(uint8_t val)
{
  return (int)( (val/16*10) + (val%16) );
}



/* function to set time */
void Set_Time (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
	uint8_t set_time[7];
	set_time[0] = decToBcd(sec);
	set_time[1] = decToBcd(min);
	set_time[2] = decToBcd(hour);
	set_time[3] = decToBcd(dow);
	set_time[4] = decToBcd(dom);
	set_time[5] = decToBcd(month);
	set_time[6] = decToBcd(year);

	HAL_I2C_Mem_Write(&hi2c2, DS3231_ADDRESS, 0x00, 1, set_time, 7, 1000);
}



/* function to get time */
void Get_Time (void)
{
	uint8_t get_time[7];
	HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x00, 1, get_time, 7, 1000);
	time.seconds = bcdToDec(get_time[0]);
	time.minutes = bcdToDec(get_time[1]);
	time.hour = bcdToDec(get_time[2]);
	time.dayofweek = bcdToDec(get_time[3]);
	time.dayofmonth = bcdToDec(get_time[4]);
	time.month = bcdToDec(get_time[5]);
	time.year = bcdToDec(get_time[6]);
}



/* function to enable alarm OnePerSecond */
void DS3231_Enable_Alarm1_OncePerSecond(I2C_HandleTypeDef *hi2c) {
    uint8_t alarm_regs[4] = {0x80, 0x80, 0x80, 0x80};  	// A1M1–A1M4 = 1
    uint8_t control_reg = 0x01;  						// INTCN = 0, A1IE = 1

    // Set alarm registers (0x07 to 0x0A)
    HAL_I2C_Mem_Write(hi2c, DS3231_ADDRESS, 0x07, 1, alarm_regs, 4, 100);

   // Set control register (0x0E)
   HAL_I2C_Mem_Write(hi2c, DS3231_ADDRESS, 0x0E, 1, &control_reg, 1, 100);

   // Clear any existing alarm flags in status register (0x0F)
   uint8_t status = 0x00;
   HAL_I2C_Mem_Write(hi2c, DS3231_ADDRESS, 0x0F, 1, &status, 1, 100);
}




void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
/* Prevent unused argument(s) compilation warning */
UNUSED(GPIO_Pin);


  if (GPIO_Pin == SQW_Pin) // PB1 triggered this
  {
    // Clear the A1F flag (write 0 to bit 0 of Status Register 0x0F)
	uint8_t status;
	HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x0F, 1, &status, 1, 100);
	status &= ~0x01; // Clear only A1F
	HAL_I2C_Mem_Write(&hi2c2, DS3231_ADDRESS, 0x0F, 1, &status, 1, 100);


    // Get time and update to LCD
	Get_Time();

	static uint8_t last_minute = 255; 					// Track the last minute to detect changes
	        if (time.minutes != last_minute) 			// Only process when minute changes
	        {
	            if (time.minutes == 20) {
	                min = 1; 									// Triggers NIGHT_MODE
	                detect_button(); 							// Trigger mode change
	                min = 0; 									// Reset min to prevent repeated triggers
	            } else if (time.minutes == 40) {
	                min = 2; 									// Triggers HEAVY_TRAFFIC_MODE
	                detect_button();
	                min = 0;
	            } else if (time.minutes == 59) {
	                min = 4; 									// Triggers LIGHT_TRAFFIC_MODE
	                detect_button();
	                min = 0;
	            }
	            last_minute = time.minutes; 					// Update last minute
	        }



	//lcd_clear();
	sprintf (buffer, "%02d:%02d:%02d", time.hour, time.minutes, time.seconds);
	lcd_put_cur (0,0);
	lcd_send_string(buffer);			// Display Time

	sprintf (buffer, "%02d/%02d/20%02d      ", time.dayofmonth, time.month, time.year);
	lcd_put_cur(1, 0);
	lcd_send_string(buffer);			// Display Date

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
