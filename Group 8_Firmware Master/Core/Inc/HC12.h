#ifndef __HC12_H__
#define __HC12_H__

#include "stm32f1xx_hal.h"

// Define ports
#define Normal_PORT    		GPIOB  		//	Normal
#define Heavy_PORT    		GPIOB  		//	Heavy
#define Night_PORT  		GPIOB  		//	Night
#define Control_PORT   		GPIOB  		//	Control
#define Control_State_PORT	GPIOB		//	Control_State



// Define pins
#define Normal_PIN     		GPIO_PIN_5  	// PB5
#define Heavy_PIN     		GPIO_PIN_6  	// PB6
#define Night_PIN   		GPIO_PIN_7  	// PB7
#define Control_PIN    		GPIO_PIN_8  	// PB8
#define Control_State_PIN   GPIO_PIN_9  	// PB9

typedef enum {
    HC12_MODE_NORMAL,
    HC12_MODE_HEAVY,
    HC12_MODE_NIGHT,
    HC12_MODE_CONTROL,
	HC12_MODE_CONTROL_STATE,
    HC12_MODE_UNKNOWN
} HC12_Mode;



void HC12_Init(void); // Initialize UART interrupts
void HC12_SendMode(HC12_Mode mode);
uint8_t HC12_ReceiveMode(HC12_Mode expected_mode); // Keep for compatibility
const char* HC12_ModeToStr(HC12_Mode mode);
HC12_Mode HC12_StrToMode(const char* str);
HC12_Mode HC12_IntReceivedMode(void); // Get mode from interrupt buffer


#endif
