/*
 * MCP2515.h
 *
 *  Created on: Sep 22, 2023
 *      Author: liors
 */

#ifndef INC_MCP2515_H_
#define INC_MCP2515_H_

#include "main.h"
#include "spi.h"
#include "REGS2515.h"

HAL_StatusTypeDef MCP2515_init(SPI_HandleTypeDef *, GPIO_TypeDef *, uint16_t);
HAL_StatusTypeDef MCP2515_reset();
HAL_StatusTypeDef MCP2515_get_read_status(uint8_t *);
HAL_StatusTypeDef MCP2515_get_rx_status(uint8_t * );

HAL_StatusTypeDef MCP2515_read(uint8_t, uint16_t, uint8_t *);



#endif /* INC_MCP2515_H_ */
