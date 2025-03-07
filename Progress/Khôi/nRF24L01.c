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
		HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_RESET);
}

// UnSelecting SLAVE Devices
void	CSN_nSelect		(void){
		HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_SET);
}

// Enable Devices
void	CE_Enable		(void){
		HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_RESET);
}

// Disable Devices
void	CE_Disable		(void){
		HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_SET);
}






// Write a single byte to Reg
void	nrf24_writeReg(uint8_t Reg, uint8_t Data) {

		uint8_t buf[2];			// data's name: buf
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
void	nrf24_writeReg_Multi(uint8_t Reg, uint8_t *Data, int size) {

		uint8_t buf[2];			// data's name: buf
		buf[0] = Reg|1<<5;		// size = 2

		// Pull the CSN Pin to LOW to select the Devices
		CSN_Select();

		HAL_SPI_Transmit(NRF24_SPI, buf, 1, 100);  	// Send Reg Addr
		HAL_SPI_Transmit(NRF24_SPI, Data, size, 1000);	// Send all Data at one

		// Pull the CSN Pin to HIGH to release the Devices
		CSN_nSelect();

}






// Read data from a Reg Addr
uint8_t nrf24_ReadReg(uint8_t Reg){

		uint8_t Data=0;

		// Pull the CSN Pin to LOW to select the Devices
		CSN_Select();

		HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100);
		// HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
		HAL_SPI_Receive(NRF24_SPI, &Data, 1, 100);

		// Pull the CSN Pin to HIGH to release the Devices
		CSN_nSelect();

		return Data;

}






// Read data from a Reg Addr
void nrf24_ReadReg_Multi(uint8_t Reg, uint8_t *Data, int size){

		// Pull the CSN Pin to LOW to select the Devices
		CSN_Select();

		HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100);
		HAL_SPI_Receive(NRF24_SPI, Data, size, 1000);

		// Pull the CSN Pin to HIGH to release the Devices
		CSN_nSelect();

}





// Send Command
void nrf24_cmd(uint8_t cmd){

		// Pull the CSN Pin to LOW to select the Devices
		CSN_Select();

		// HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
		HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);

		// Pull the CSN Pin to HIGH to release the Devices
		CSN_nSelect();
}





void nrf24_reset(uint8_t REG)
{
		if (REG == STATUS)
		{
			nrf24_writeReg(STATUS, 0x00);
		}

		else if (REG == FIFO_STATUS)
		{
			nrf24_writeReg(FIFO_STATUS, 0x11);
		}

		else {
			nrf24_writeReg(CONFIG, 0x08);
			nrf24_writeReg(EN_AA, 0x3F);
			nrf24_writeReg(EN_RXADDR, 0x03);
			nrf24_writeReg(SETUP_AW, 0x03);
			nrf24_writeReg(SETUP_RETR, 0x03);
			nrf24_writeReg(RF_CH, 0x02);
			nrf24_writeReg(RF_SETUP, 0x0E);
			nrf24_writeReg(STATUS, 0x00);
			nrf24_writeReg(OBSERVE_TX, 0x00);
			nrf24_writeReg(CD, 0x00);
			uint8_t rx_addr_p0_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
			nrf24_writeReg_Multi(RX_ADDR_P0, rx_addr_p0_def, 5);
			uint8_t rx_addr_p1_def[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
			nrf24_writeReg_Multi(RX_ADDR_P1, rx_addr_p1_def, 5);
			nrf24_writeReg(RX_ADDR_P2, 0xC3);
			nrf24_writeReg(RX_ADDR_P3, 0xC4);
			nrf24_writeReg(RX_ADDR_P4, 0xC5);
			nrf24_writeReg(RX_ADDR_P5, 0xC6);
			uint8_t tx_addr_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
			nrf24_writeReg_Multi(TX_ADDR, tx_addr_def, 5);
			nrf24_writeReg(RX_PW_P0, 0);
			nrf24_writeReg(RX_PW_P1, 0);
			nrf24_writeReg(RX_PW_P2, 0);
			nrf24_writeReg(RX_PW_P3, 0);
			nrf24_writeReg(RX_PW_P4, 0);
			nrf24_writeReg(RX_PW_P5, 0);
			nrf24_writeReg(FIFO_STATUS, 0x11);
			nrf24_writeReg(DYNPD, 0);
			nrf24_writeReg(FEATURE, 0);
		}
}








// Initialize nRF24
void nrf24_init(void){

		// Disable the Device before configuration
		CE_Disable();
		// Pull the CSN Pin to LOW to select the Devices
		CSN_nSelect();


		nrf24_writeReg(CONFIG, 0);			// Configure later

		nrf24_writeReg(EN_AA, 0);			// No Auto  Acknowledgment

		nrf24_writeReg(EN_RXADDR, 0);		// Enable Data Pipe Receiving (Not Enable any Pipe)

		nrf24_writeReg(SETUP_AW, 0x03);		// 5 Bytes for TX/RX Address

		nrf24_writeReg(SETUP_RETR, 0);		// No Auto ARK => No need to SET

		nrf24_writeReg(RF_CH, 0);			// RF Channel, Set up after TX/RX

		nrf24_writeReg(RF_SETUP, 0x0E);		// Power = 0db, Data rate = 2 Mbps

		// Enable the Device before configuration
		CE_Enable();
		// Pull the CSN Pin to HIGH to select the Devices
		CSN_Select();

}






// Set-up the TX mode

void nrf24_TxMode (uint8_t *Address, uint8_t channel)
{
	// disable the chip before configuring the device
	CE_Disable();

	nrf24_writeReg (RF_CH, channel);  				// select the channel

	nrf24_writeReg_Multi(TX_ADDR, Address, 5); 		// Write the TX address


	// power up the device
	uint8_t config = nrf24_ReadReg(CONFIG);
//	config = config | (1<<1);   					// write 1 in the PWR_UP bit
	config = config & (0xF2);    					// write 0 in the PRIM_RX, and 1 in the PWR_UP, and all other bits are masked
	nrf24_writeReg (CONFIG, config);

	// Enable the chip after configuring the device
	CE_Enable();
}







// transmit the data

uint8_t nrf24_Transmit (uint8_t *data)
{
	uint8_t cmdtosend = 0;

	// select the device
	CSN_Select();

	// payload command
	cmdtosend = W_TX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);

	// send the payload
	HAL_SPI_Transmit(NRF24_SPI, data, 32, 1000);

	// Unselect the device
	CSN_nSelect();

	HAL_Delay(1);

	uint8_t fifostatus = nrf24_ReadReg(FIFO_STATUS);

	// check the fourth bit of FIFO_STATUS to know if the TX fifo is empty
	if ((fifostatus&(1<<4)) && (!(fifostatus&(1<<3))))
	{
		cmdtosend = FLUSH_TX;
		nrf24_cmd(cmdtosend);

		// reset FIFO_STATUS
		nrf24_reset (FIFO_STATUS);

		return 1;
	}

	return 0;
}






void nrf24_RxMode (uint8_t *Address, uint8_t channel)
{
	// disable the chip before configuring the device
	CE_Disable();

	nrf24_reset (STATUS);

	nrf24_writeReg (RF_CH, channel);  		// select the channel

	// select data pipe 2
	uint8_t en_rxaddr = nrf24_ReadReg(EN_RXADDR);
	en_rxaddr = en_rxaddr | (1<<2);
	nrf24_writeReg (EN_RXADDR, en_rxaddr);

	/* We must write the address for Data Pipe 1, if we want to use any pipe from 2 to 5
	 * The Address from DATA Pipe 2 to Data Pipe 5 differs only in the LSB
	 * Their 4 MSB Bytes will still be same as Data Pipe 1
	 *
	 * For Eg->
	 * Pipe 1 ADDR = 0xAABBCCDD11
	 * Pipe 2 ADDR = 0xAABBCCDD22
	 * Pipe 3 ADDR = 0xAABBCCDD33
	 *
	 */
	nrf24_writeReg_Multi(RX_ADDR_P1, Address, 5);  // Write the Pipe1 address
	nrf24_writeReg(RX_ADDR_P2, 0xEE);  	// Write the Pipe2 LSB address

	nrf24_writeReg (RX_PW_P2, 32);  	// 32 bit payload size for pipe 2


	// power up the device in Rx mode
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1<<1) | (1<<0);
	nrf24_writeReg (CONFIG, config);

	// Enable the chip after configuring the device
	CE_Enable();
}








uint8_t isDataAvailable (int pipenum)
{
	uint8_t status = nrf24_ReadReg(STATUS);

	if ((status&(1<<6))&&(status&(pipenum<<1)))
	{

		nrf24_writeReg(STATUS, (1<<6));

		return 1;
	}

	return 0;
}







void nrf24_Receive (uint8_t *data)
{
	uint8_t cmdtosend = 0;

	// select the device
	CSN_Select();

	// payload command
	cmdtosend = R_RX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);

	// Receive the Payload
	HAL_SPI_Receive(NRF24_SPI, data, 32, 1000);

	// Unselect the device
	CSN_nSelect();

	HAL_Delay(1);

	cmdtosend = FLUSH_RX;
	nrf24_cmd(cmdtosend);
}







// Read all the Register data
void nrf24_ReadAll (uint8_t *data)
{
	for (int i=0; i<10; i++)
	{
		*(data+i) = nrf24_ReadReg(i);
	}

	nrf24_ReadReg_Multi(RX_ADDR_P0, (data+10), 5);

	nrf24_ReadReg_Multi(RX_ADDR_P1, (data+15), 5);

	*(data+20) = nrf24_ReadReg(RX_ADDR_P2);
	*(data+21) = nrf24_ReadReg(RX_ADDR_P3);
	*(data+22) = nrf24_ReadReg(RX_ADDR_P4);
	*(data+23) = nrf24_ReadReg(RX_ADDR_P5);

	nrf24_ReadReg_Multi(RX_ADDR_P0, (data+24), 5);

	for (int i=29; i<38; i++)
	{
		*(data+i) = nrf24_ReadReg(i-12);
	}

}











