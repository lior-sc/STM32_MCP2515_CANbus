/*
 * MCP2515.c
 *
 *  Created on: Sep 21, 2023
 *      Author: liors
 */

#include "MCP2515.h"

const uint32_t MCP2515_SPI_TIMEOUT = 10;
static SPI_HandleTypeDef *hspi;
static GPIO_TypeDef *CS_GPIO_Port;
static uint16_t CS_Pin;

// SPI INSTRUCTION SET
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

HAL_StatusTypeDef MCP2515_read(uint8_t address, uint16_t length, uint8_t buf[])
{
	HAL_StatusTypeDef ret;
	uint8_t tx_buf[2] = {0x00};

	tx_buf[0] = MCP2515_CAN_READ;
	tx_buf[1] = address;

	// send transmit-recieve via SPI
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
	ret = HAL_SPI_Transmit(hspi, tx_buf, 2, 100);

	if(ret!= HAL_OK){
		return ret;
	}

	ret = HAL_SPI_Receive(hspi, buf, length, 100);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

	return ret;
}

HAL_StatusTypeDef MCP2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data)
{
	HAL_StatusTypeDef ret;
	uint8_t tx_buf[4];
	tx_buf[0] = MCP2515_CAN_BIT_MODIFY;
	tx_buf[1] = address;
	tx_buf[2] = mask;
	tx_buf[3] = data;

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
	ret = HAL_SPI_Transmit(hspi, tx_buf, 4, 100);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

	return ret;
}


/**
 * @brief	The READ STATUS instruction allows single instruction access to some of the often used status bits for
 * message reception and transmission. The MCP2515 is selected by lowering the CS pin and the READ STATUS command
 * byte, shown in Figure 12-8, is sent to the MCP2515. Once the command byte is sent,the MCP2515 will return eight
 * bits of data that contain the status.
 *
 * @param	*buf: the value will be retured to this pointer. the pointer should be to a uint8_t type variable
 */
HAL_StatusTypeDef MCP2515_get_read_status(uint8_t * buf)
{
	uint8_t tx_buf[2] = {0x00};
	uint8_t rx_buf[2] = {0x00};

	tx_buf[0] = MCP2515_CAN_READ_STATUS;

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(hspi, tx_buf, rx_buf, 2, 100);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

	if(ret == HAL_OK)
	{
		*buf = rx_buf[1];
	}

	return ret;
}

/**
 * @brief 	The RX STATUS instruction is used to quickly determine which filter matched the message
 * and message type (standard, extended, remote). After the command byte is sent, the controller will
 * return 8 bits of data that contain the status data. If more clocksare sent after the eight bits
 * are transmitted, the controller will continue to output the same status bits as long as the CS pin
 * stays low and clocks are provided.
 *
 * @param	*buf: the value will be returned to this pointer. the pointer should be to a
 * 				  uint8_t type variable
 */
HAL_StatusTypeDef MCP2515_get_rx_status(uint8_t * buf)
{
	uint8_t tx_buf[2] = {0x00};
	uint8_t rx_buf[2] = {0x00};

	tx_buf[0] = MCP2515_CAN_RX_STATUS;

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(hspi, tx_buf, rx_buf, 2, 100);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

	if(ret == HAL_OK)
	{
		*buf = rx_buf[1];
	}

	return ret;
}

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

HAL_StatusTypeDef MCP2515_get_bit_timing_condiguration()
{
	uint8_t tx_buf[5] = {0x00};
	uint8_t rx_buf[5] = {0x00};

	tx_buf[0] = MCP2515_CAN_READ;
	tx_buf[1] = 0x28;

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(hspi, tx_buf, rx_buf, 5, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

	return ret;
}

bool MCP2515_set_config_mode()
{
	uint8_t canctrl_value = 0x00;
	uint8_t reqop_mask = 0b11100000;
	uint8_t config_mode_cmd = 0b100 << 5;

	uint8_t count = 0;
	uint8_t attempts = 10;
	do{
		MCP2515_bit_modify(MCP2515_CANCTRL, reqop_mask, config_mode_cmd);
		HAL_Delay(500);
		MCP2515_read(MCP2515_CANCTRL,1,&canctrl_value);
	}
	while((canctrl_value & reqop_mask) != config_mode_cmd && count++ < 10);

	if(count < attempts){
		return true;
	}

	else{
		return false;
	}
}

bool MCP2515_set_normal_mode()
{
	uint8_t canctrl_value = 0x00;
	uint8_t reqop_mask = 0b11100000;
	uint8_t config_mode_cmd = 0b000 << 5;

	uint8_t count = 0;
	uint8_t attempts = 10;
	do{
		MCP2515_bit_modify(MCP2515_CANCTRL, reqop_mask, config_mode_cmd);
		HAL_Delay(250);
		MCP2515_read(MCP2515_CANCTRL,1,&canctrl_value);
	}
	while((canctrl_value & reqop_mask) != config_mode_cmd && count++ < 10);

	if(count < attempts){
		return true;
	}

	else{
		return false;
	}
}

void get_MCP2515_canstat_data()
{
	  uint8_t tx_buf[10] = {0x00};
	  uint8_t rx_buf[10] = {0x00};

	  tx_buf[0] = MCP2515_CAN_READ;
	  tx_buf[1] = MCP2515_CANSTAT;

	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	  HAL_SPI_TransmitReceive(hspi, tx_buf, rx_buf, 10, 100);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

	  return;
}
