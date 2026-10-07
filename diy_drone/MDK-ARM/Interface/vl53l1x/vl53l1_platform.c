/**
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "vl53l1_platform.h"
#include <string.h>
#include <time.h>
#include <math.h>

int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	return 0;
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
	return 0;
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
	HAL_I2C_Mem_Write(&hi2c2, dev, index, I2C_MEMADD_SIZE_16BIT, &data, 1, 1000);
	return 0;
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
	// STM32使用小端序，VL53L1使用大端序，需要转换
	uint8_t data_buf[2] = {data >> 8, data & 0xFF};
	HAL_I2C_Mem_Write(&hi2c2, dev, index, I2C_MEMADD_SIZE_16BIT, data_buf, 2, 1000);
	return 0;
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
	// STM32使用小端序，VL53L1使用大端序，需要转换
	uint8_t data_buf[4] = {data >> 24, (data >> 16) & 0xFF, (data >> 8) & 0xFF, data & 0xFF};
	HAL_I2C_Mem_Write(&hi2c2, dev, index, I2C_MEMADD_SIZE_16BIT, data_buf, 4, 1000);
	return 0;
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
	HAL_I2C_Mem_Read(&hi2c2, dev, index, I2C_MEMADD_SIZE_16BIT, data, 1, 1000);
	return 0;
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
	// VL53L1读取的数据是大端序，STM32读取的数据是小端序，需要转换
	uint8_t data_buf[2] = {0};
	HAL_I2C_Mem_Read(&hi2c2, dev, index, I2C_MEMADD_SIZE_16BIT, data_buf, 2, 1000);
	*data = (data_buf[0] << 8) | data_buf[1];
	return 0;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
	// VL53L1读取的数据是大端序，STM32读取的数据是小端序，需要转换
	uint8_t data_buf[4] = {0};
	HAL_I2C_Mem_Read(&hi2c2, dev, index, I2C_MEMADD_SIZE_16BIT, data_buf, 4, 1000);
	*data = (data_buf[0] << 24) | (data_buf[1] << 16) | (data_buf[2] << 8) | data_buf[3];
	return 0;
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	return 0;
}
