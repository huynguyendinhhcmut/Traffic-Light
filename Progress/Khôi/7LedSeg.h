/*
 * 7LedSeg.h
 *
 *  Created on: Mar 7, 2025
 *      Author: khoid
 */


#ifndef SEVEN_LED_SEG_H
#define SEVEN_LED_SEG_H

#include "stm32f1xx_hal.h"



// Define ports for STM32F103C8T6
#define NSS_PORT    		GPIOB  		// NSS (Slave Select) pin
#define SCK_PORT    		GPIOB  		// SCK (Clock) pin
#define MISO_PORT  			GPIOB  		// MISO (Master In Slave Out) pin
#define MOSI_PORT   		GPIOB  		// MOSI (Master Out Slave In) pin
#define RED_LIGHT_PORT		GPIOA		//
#define YELLOW_LIGHT_PORT	GPIOA		//
#define GREEN_LIGHT_PORT	GPIOA		//




// Define pins for STM32F103C8T6
#define NSS_PIN     		GPIO_PIN_12  	// PB12
#define SCK_PIN     		GPIO_PIN_13  	// PB13
#define MISO_PIN   			GPIO_PIN_14  	// PB14
#define MOSI_PIN    		GPIO_PIN_15  	// PB15
#define RED_LIGHT_PIN		GPIO_PIN_10		// PA10
#define YELLOW_LIGHT_PIN	GPIO_PIN_9		// PA9
#define GREEN_LIGHT_PIN		GPIO_PIN_8		// PA8


// Define functions
void initializeSPI(void);
void clearShiftRegister(void);
void shiftOut(uint8_t data);
void displayNumber(uint8_t number);
void countDownNum(uint8_t number);
void traficDisplay(uint8_t number);

#endif  // SEVEN_LED_SEG_H
