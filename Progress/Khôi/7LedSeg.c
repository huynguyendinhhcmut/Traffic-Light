/*
 * 7LedSeg.c
 *
 *  Created on: Mar 7, 2025
 *      Author: khoid
 */


/*
STM32 Pin (GPIOB)			|	74HC595 Pin			|	Description
---------------------------------------------------------------------------------------------
PB15 (MOSI_PIN)				|	Pin 14 (DS)			|	Data Output
PB13 (SCK_PIN)				|	Pin 11 (SH_CP)		|	Shift Clock
PB12 (NSS_PIN)				|	Pin 12 (ST_CP)		|	Latch Clock (to update output)
Optional: PB10 (MOSI_PIN)	|	Pin 10 (MR)			|	Master Reset (tie to VCC if not used)
GND							|	Pin 13 (OE)			|	Output Enable (active low, tie to GND)
 */




#include "stm32f1xx_hal.h"
#include "7LedSeg.h"

extern SPI_HandleTypeDef hspi2;
#define SEVEN_LED_SEG_SPI &hspi2


// Numbers

const uint8_t digitCodes[10] = {
    ~0x3F,  // 0 -> 0b00111111
    ~0x06,  // 1 -> 0b00000110
    ~0x5B,  // 2 -> 0b01011011
    ~0x4F,  // 3 -> 0b01001111
    ~0x66,  // 4 -> 0b01100110
    ~0x6D,  // 5 -> 0b01101101
    ~0x7D,  // 6 -> 0b01111101
    ~0x07,  // 7 -> 0b00000111
    ~0x7F,  // 8 -> 0b01111111
    ~0x6F   // 9 -> 0b01101111
};



// Characters

const uint8_t characterCodes[6] = {
    0x77,  // A -> 0b01110111
    0x7C,  // B -> 0b01111100
    0x39,  // C -> 0b00111001
    0x5E,  // D -> 0b01011110
    0x79,  // E -> 0b01111001
    0x71   // F -> 0b01110001

};



// Function to initialize SPI
void initializeSPI(void) {
    // If already configured in STM32CubeMX, no additional settings are needed
    HAL_SPI_Init(SEVEN_LED_SEG_SPI);
}



// Function to clear the shift register
void clearShiftRegister(void) {
    uint8_t clearData = 0x00;  				// Send 0 to clear
	shiftOut(clearData);    				// End transmission (latch high)
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
    HAL_Delay(5);  // Small delay to stabilize display

    // Send units digit to second 7-segment display
    shiftOut(digitCodes[tens]);
    HAL_Delay(5);  // Small delay to stabilize display
}



// Function to CountDown number
void countDownNum(uint8_t number){
	// Loop displaying number
	for (int8_t i = number; i >= 0; i--) {
		displayNumber(i);      	// Display xx to 00
		HAL_Delay(1000);        // delay 1s
		}
}



// Function to control light mode
void turnOnRed() {
    HAL_GPIO_WritePin(RED_LIGHT_PORT, RED_LIGHT_PIN, GPIO_PIN_SET);    			// RED 		ON
    HAL_GPIO_WritePin(YELLOW_LIGHT_PORT, YELLOW_LIGHT_PIN, GPIO_PIN_RESET);		// YELLOW 	OFF
    HAL_GPIO_WritePin(GREEN_LIGHT_PORT, GREEN_LIGHT_PIN, GPIO_PIN_RESET);		// GREEN 	OFF
}
void turnOnYellow() {
    HAL_GPIO_WritePin(RED_LIGHT_PORT, RED_LIGHT_PIN, GPIO_PIN_RESET);			// RED 		OFF
    HAL_GPIO_WritePin(YELLOW_LIGHT_PORT, YELLOW_LIGHT_PIN, GPIO_PIN_SET);  		// Yellow 	ON
    HAL_GPIO_WritePin(GREEN_LIGHT_PORT, GREEN_LIGHT_PIN, GPIO_PIN_RESET);		// GREEN 	OFF
}
void turnOnGreen() {
    HAL_GPIO_WritePin(RED_LIGHT_PORT, RED_LIGHT_PIN, GPIO_PIN_RESET);			// RED 		OFF
    HAL_GPIO_WritePin(YELLOW_LIGHT_PORT, YELLOW_LIGHT_PIN, GPIO_PIN_RESET);		// YELLOW 	OFF
    HAL_GPIO_WritePin(GREEN_LIGHT_PORT, GREEN_LIGHT_PIN, GPIO_PIN_SET);    		// Green 	ON
}





// Function to display traffic light in certain time
/*
 * Input Counting value
 *
 * Mode		|	Time	|	unit
-----------------------------------
 * Green 	|	xx 		|	(s)
 * Yellow 	|	xx-3 	|	(s)
 * Red		|	3		|	(s)
 *
 *
 */
void traficDisplay(uint8_t number){
	// Ensure minimum time for traffic light sequence
	if (number < 3) number = 3;

	uint8_t green_time = number - 3;	// Green light duration
	uint8_t yellow_time = 3;			// Yellow light duration (fixed at 3s)
	uint8_t red_time = number;			// Red light duration

	// Green Light
	turnOnGreen();
	countDownNum(green_time);
	// Yellow Light
	turnOnYellow();
	countDownNum(yellow_time);
	// Red Light
	turnOnRed();
	countDownNum(red_time);
}
















