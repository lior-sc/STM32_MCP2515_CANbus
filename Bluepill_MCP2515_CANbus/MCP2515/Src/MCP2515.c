/*
 * MCP2515.c
 *
 *  Created on: Sep 21, 2023
 *      Author: liors
 */

#include "MCP2515.h"
#include "spi.h"

const uint32_t MCP2515_SPI_TIMEOUT = 10;
static SPI_HandleTypeDef *hspi;
static GPIO_TypeDef *CS_GPIO_Port;
static uint16_t CS_Pin;


HAL_StatusTypeDef MCP2515_init(SPI_HandleTypeDef *spi_handle, GPIO_TypeDef *MCP2515_CS_GPIO_Port, uint16_t MCP2515_CS_PIN)
{
	// set pointer to SPI and CS pin handles
	hspi = spi_handle;
	CS_GPIO_Port = MCP2515_CS_GPIO_Port;
	CS_Pin = MCP2515_CS_PIN;

	// reset MCP2515 registers
	HAL_StatusTypeDef ret = MCP2515_reset();

	return ret;
}

HAL_StatusTypeDef MCP2515_reset()
{
	uint8_t can_reset_cmd = MCP2515_CAN_RESET;
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef ret = HAL_SPI_Transmit(hspi, &can_reset_cmd, 1, MCP2515_SPI_TIMEOUT);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

	return ret;
}

/**
  * @brief  reads the value of a register in the MCP2515
  *
  * @note   This function uses GPIOx_BSRR register to allow atomic read/modify
  *         accesses. In this way, there is no risk of an IRQ occurring between
  *         the read and the modify access.
  *
  * @param  address: Register address
  * @param  size: number of bytes to read (consecutive registers)
  *          This parameter can be one of GPIO_PIN_x where x can be (0..15).
  * @param  buf: the buffer in which the data read from the registers will be stored

  * @retval function execution status. if all goes well should be HAL_OK
  */
HAL_StatusTypeDef MCP2515_read(uint8_t address, uint16_t size, uint8_t rx_buf[])
{
	uint8_t tx_buf[20] = {0x00};

	tx_buf[0] = MCP2515_CAN_READ;
	tx_buf[1] = address;

	// send transmit-recieve via SPI
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(hspi, tx_buf, rx_buf, size, 100);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

	return ret;
}
