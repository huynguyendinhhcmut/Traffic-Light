#include "HC12.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart2;

// Interrupt buffer and state
static char rx_buffer[22];
static uint16_t rx_index = 0;
static volatile uint8_t rx_complete = 0;
static HC12_Mode last_received_mode = HC12_MODE_UNKNOWN;
static uint8_t control_state_processed = 0;


//======================================== Convert MODE to STRING =================================

const char* HC12_ModeToStr(HC12_Mode mode)
{
    switch (mode)
    {
        case HC12_MODE_NORMAL: return "normal";
        case HC12_MODE_HEAVY: return "heavy";
        case HC12_MODE_NIGHT: return "night";
        case HC12_MODE_CONTROL: return "control";
        case HC12_MODE_CONTROL_STATE: return "state";
        default: return "unknown";
    }
}


//======================================== Convert STRING to MODE =================================

HC12_Mode HC12_StrToMode(const char* str)
{
    if (strcmp(str, "normal") == 0) return HC12_MODE_NORMAL;
    if (strcmp(str, "heavy") == 0) return HC12_MODE_HEAVY;
    if (strcmp(str, "night") == 0) return HC12_MODE_NIGHT;
    if (strcmp(str, "control") == 0) return HC12_MODE_CONTROL;
    if (strcmp(str, "state") == 0) return HC12_MODE_CONTROL_STATE;
    return HC12_MODE_UNKNOWN;
}


//======================================== Send MODE to SLAVE =================================

void HC12_SendMode(HC12_Mode mode)
{
    const char* str = HC12_ModeToStr(mode);
    HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), 100);
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
}


//======================================== Receive MODE =================================

uint8_t HC12_ReceiveMode(HC12_Mode expected_mode)
{
    char buffer[22];
    uint8_t ch;
    uint16_t i = 0;

    while (i < sizeof(buffer) - 1)
    {
        if (HAL_UART_Receive(&huart2, &ch, 1, 10) == HAL_OK)
        {
            if (ch == '\n') break;
            if (ch != '\r') buffer[i++] = ch;
        }
        else
        {
            return 0;
        }
    }

    buffer[i] = '\0';
    const char* expected_str = HC12_ModeToStr(expected_mode);
    return (strcmp(buffer, expected_str) == 0) ? 1 : 0;
}


//======================================== HC12 Initialize =================================

void HC12_Init(void)
{
    rx_index = 0;
    rx_complete = 0;
    last_received_mode = HC12_MODE_UNKNOWN;
    memset(rx_buffer, 0, sizeof(rx_buffer));
    HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_buffer[0], 1);
}


//======================================== Receive MODE (INTERRUPT) =================================

HC12_Mode HC12_IntReceivedMode(void)
{
    if (rx_complete)
    {
        HC12_Mode mode = HC12_StrToMode(rx_buffer);


        // DEBUG output using USB to TTL
        if (mode != HC12_MODE_UNKNOWN)
        {
            char debug_msg[50];
            snprintf(debug_msg, sizeof(debug_msg), "Received: %s, Mode: %d\r\n", rx_buffer, mode);
            HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), 100);
        }


        // Reset for next reception
        rx_index = 0;
        rx_complete = 0;
        memset(rx_buffer, 0, sizeof(rx_buffer));
        HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_buffer[0], 1);

        // If in Control Mode,
        if (mode == HC12_MODE_CONTROL_STATE)
                {
                    if (!control_state_processed)				// Normally STATE == 0, Run the State
                    {
                        control_state_processed = 1;			// If STATE == 1, RESET to not run this loop
                        last_received_mode = HC12_MODE_CONTROL;
                        return HC12_MODE_CONTROL_STATE;
                    }
                    else
                    {
                        return HC12_MODE_CONTROL;				// Return to Control_Mode, Until get new mode
                    }
                }
                else
                {
                    last_received_mode = mode;
                    control_state_processed = 0;
                    return mode;
                }
          }

    return last_received_mode;				// Re-sending MODE
}


//======================================== RX Interrupt =================================

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == huart2.Instance)
    {
        if (rx_buffer[rx_index] == '\n')
        {
            rx_buffer[rx_index] = '\0';
            rx_complete = 1;
        }
        else if (rx_buffer[rx_index] != '\r')
        {
            rx_index++;
            if (rx_index >= sizeof(rx_buffer) - 1)
            {
                rx_buffer[rx_index] = '\0';
                rx_complete = 1;
            }
        }
        if (!rx_complete)
        {
            HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_buffer[rx_index], 1);
        }
    }
}
