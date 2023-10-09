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
#include "stdbool.h"

/**
 * MCP bit rates (assuming 8Mhz clock)
 */
static const uint8_t MCP_8MHz_1000kBPS_CFG1 = 0x00;
static const uint8_t MCP_8MHz_1000kBPS_CFG2 = 0x80;
static const uint8_t MCP_8MHz_1000kBPS_CFG3 = 0x80;

static const uint8_t MCP_8MHz_500kBPS_CFG1 = 0x00;
static const uint8_t MCP_8MHz_500kBPS_CFG2 = 0x90;
static const uint8_t MCP_8MHz_500kBPS_CFG3 = 0x82;

static const uint8_t MCP_8MHz_250kBPS_CFG1 = 0x00;
static const uint8_t MCP_8MHz_250kBPS_CFG2 = 0xB1;
static const uint8_t MCP_8MHz_250kBPS_CFG3 = 0x85;

static const uint8_t MCP_8MHz_125kBPS_CFG1 = 0x01;
static const uint8_t MCP_8MHz_125kBPS_CFG2 = 0xB1;
static const uint8_t MCP_8MHz_125kBPS_CFG3 = 0x85;


HAL_StatusTypeDef MCP2515_init(SPI_HandleTypeDef *, GPIO_TypeDef *, uint16_t);
HAL_StatusTypeDef MCP2515_reset();
HAL_StatusTypeDef MCP2515_get_read_status(uint8_t *);
HAL_StatusTypeDef MCP2515_get_rx_status(uint8_t *);
bool MCP2515_set_normal_mode();
bool MCP2515_set_config_mode();
void get_MCP2515_canstat_data();
//HAL_StatusTypeDef MCP2515_get_bit_timing_condiguration(2);

HAL_StatusTypeDef MCP2515_read(uint8_t, uint16_t, uint8_t *);



#endif /* INC_MCP2515_H_ */
