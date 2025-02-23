/*
 * nRF24L01.c
 *
 *  Created on: Feb 23, 2025
 *      Author: khoid
 */

#include "stm32f1xx_hal.h"
#include "NRF24L01.h"

#define NRF24_CE_PORT	GPIOA
#define NRF24_CE_PIN	GPIO_PIN_3

#define NRF24_CSN_PORT	GPIOA
#define NRF24_CSN_PIN	GPIO_PIN_4

extern SPI_HandleTypeDef hspi1;
#define NRF24_SPI &hspi1




// Selecting SLAVE Devices
void	CSN_Select		(void){
		HAL_GPIO__WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_RESET);
}

// UnCelecting SLAVE Devices
void	CSN_nSelect		(void){
		HAL_GPIO__WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_SET);
}

// Enable Devices
void	CE_Enable		(void){
		HAL_GPIO__WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_RESET);
}

// Disable Devices
void	CE_Disable		(void){
		HAL_GPIO__WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_SET);
}





// Write a single byte to Reg
void	nrf24_writeReg(unit8_t Reg, unit8_t Data) {

		unit8_t buf[2];			// data's name: buf
		buf[0] = Reg|1<<5;		// size = 2
		buf[1] = Data;

		// Pull the CSN Pin to LOW to select the Devices
		CSN_Select();

		// HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
		HAL_SPI_Transmit(NRF24_SPI, buf, 2, 100);

		// Pull the CSN Pin to HIGH to release the Devices
		CSN_nSelect();

}


// Write multiple bytes starting form a Reg
void	nrf24_writeReg_Multi(unit8_t Reg, unit8_t *Data, int size) {

		unit8_t buf[2];			// data's name: buf
		buf[0] = Reg|1<<5;		// size = 2

		// Pull the CSN Pin to LOW to select the Devices
		CSN_Select();

		HAL_SPI_Transmit(NRF24_SPI, buf, 1, 100);  	// Send Reg Addr
		HAL_SPI_Transmit(NRF24_SPI, Data, size, 1000);	// Send all Data at one

		// Pull the CSN Pin to HIGH to release the Devices
		CSN_nSelect();

}


// Read data from a Reg Addr
unit8_t nrf24_ReadReg(unit8_t Reg){

		unit8_t Data=0;

		// Pull the CSN Pin to LOW to select the Devices
		CSN_Select();

		HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100);
		// HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
		HAL_SPI_Recieve(NRF24_SPI, &Data, 1, 100);

		// Pull the CSN Pin to HIGH to release the Devices
		CSN_nSelect();

		return Data;

}


// Read data from a Reg Addr
unit8_t nrf24_ReadReg_Multi(unit8_t Reg, unit8_t *Data, int size){

		unit8_t Data=0;

		// Pull the CSN Pin to LOW to select the Devices
		CSN_Select();

		HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100);
		HAL_SPI_Recieve(NRF24_SPI, Data, size, 1000);

		// Pull the CSN Pin to HIGH to release the Devices
		CSN_nSelect();

		return Data;

}


// Send Command
unit8_t nrf24_cmd(unit8_t cmd){

		// Pull the CSN Pin to LOW to select the Devices
		CSN_Select();

		// HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
		HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);

		// Pull the CSN Pin to HIGH to release the Devices
		CSN_nSelect();
}


// Initialize nRF24
unit8_t nrf24_init(void){

		// Disable the Device before configuration
		CE_Disable();
		// Pull the CSN Pin to LOW to select the Devices
		CSN_nSelect();


		nrf24_writeReg(CONFIG, 0);			// Configure later

		nrf24_writeReg(EN_AA, 0);			// No Auto  Acknowledgment

		nrf24_writeReg(EN_RXADDR, 0);		// Enable Data Pipe Receiving (Not Enable any Pipe)

		nrf24_writeReg(SETUP_AW, 0x03);		// 5 Bytes for TX/RX Address

		nrf24_writeReg(SETUP_RETR, 0);		// No Auto ARK => No need to SET

		nrf24_writeReg(RF_Ch, 0);			// RF Channel, Set up after TX/RX

		nrf24_writeReg(RF_SETUP, 0x0E);		// Power = 0db, Data rate = 2 Mbps

}





































