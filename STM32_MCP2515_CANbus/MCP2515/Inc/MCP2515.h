/*
 * MCP2515.h
 *
 *  Created on: Sep 22, 2023
 *      Author: liors
 */

#ifndef INC_MCP2515_H_
#define INC_MCP2515_H_

#include "stm32f4xx_hal.h"
#include "main.h"
#include "spi.h"
#include "REGS2515.h"

HAL_StatusTypeDef MCP2515_init();
HAL_StatusTypeDef MCP2515_reset();




#endif /* INC_MCP2515_H_ */
