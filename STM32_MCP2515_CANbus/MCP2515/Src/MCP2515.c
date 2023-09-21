/*
 * MCP2515.c
 *
 *  Created on: Sep 21, 2023
 *      Author: liors
 */

#include "MCP2515.h"

const uint32_t MCP2515_SPI_TIMEOUT = 10;

HAL_StatusTypeDef MCP2515_init()
{
	HAL_StatusTypeDef ret = MCP2515_reset();

	return ret;
}

HAL_StatusTypeDef MCP2515_reset()
{
	HAL_StatusTypeDef ret;

	HAL_GPIO_WritePin(MCP2515_CS_GPIO_Port, MCP2515_CS_Pin, GPIO_PIN_RESET);
	ret = HAL_SPI_Transmit(&hspi1, (uint8_t *)MCP2515_CAN_RESET, 1, MCP2515_SPI_TIMEOUT);
	HAL_GPIO_WritePin(MCP2515_CS_GPIO_Port, MCP2515_CS_Pin, GPIO_PIN_SET);

	HAL_Delay(100);

	return ret;
}
