#ifndef __INT_PMW3901_H__
#define __INT_PMW3901_H__

#include "FreeRTOS.h"
#include "task.h"
#include "spi.h"
#include "Com_debug.h"
#include <stdlib.h>

#define PMW3901_CS_LOW            HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET)
#define PMW3901_CS_HIGH           HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET)
#define PMW3901_RESET_LOW         HAL_GPIO_WritePin(PMW_RST_GPIO_Port, PMW_RST_Pin, GPIO_PIN_RESET)
#define PMW3901_RESET_HIGH        HAL_GPIO_WritePin(PMW_RST_GPIO_Port, PMW_RST_Pin, GPIO_PIN_SET)

/* PMW3901 Optical Flow Sensor Register Map */
/* Refer to PixArt PMW3901MB-TXQT Datasheet */

/* Product ID & Revision Registers */
#define PMW3901_PRODUCT_ID          0x00    // R - Default: 0x49
#define PMW3901_REVISION_ID         0x01    // R - Default: 0x00
#define PMW3901_INVERSE_PRODUCT_ID  0x5F    // R - Default: 0x86 (Inverse of Product_ID)

/* Motion Data Registers */
#define PMW3901_MOTION              0x02    // R/W - Motion status
#define PMW3901_DELTA_X_L           0x03    // R - Lower byte of X displacement
#define PMW3901_DELTA_X_H           0x04    // R - Upper byte of X displacement
#define PMW3901_DELTA_Y_L           0x05    // R - Lower byte of Y displacement
#define PMW3901_DELTA_Y_H           0x06    // R - Upper byte of Y displacement

/* Quality and Diagnostic Registers */
#define PMW3901_SQUAL               0x07    // R - Surface quality
#define PMW3901_RAW_DATA_SUM        0x08    // R - Average raw data value
#define PMW3901_MAX_RAW_DATA        0x09    // R - Maximum raw data value
#define PMW3901_MIN_RAW_DATA        0x0A    // R - Minimum raw data value
#define PMW3901_SHUTTER_LOWER       0x0B    // R - Lower byte of shutter value
#define PMW3901_SHUTTER_UPPER       0x0C    // R - Upper 5 bits of shutter value

/* Special Function Registers */
#define PMW3901_OBSERVATION         0x15    // R/W - Observation register
#define PMW3901_MOTION_BURST        0x16    // R - Motion burst register

/* Control Registers */
#define PMW3901_POWER_UP_RESET      0x3A    // W - Write 0x5A to reset
#define PMW3901_SHUTDOWN            0x3B    // W - Write 0xB6 to shutdown

/* Raw Data Access Registers */
#define PMW3901_RAW_DATA_GRAB       0x58    // R/W - Raw data grab register
#define PMW3901_RAW_DATA_GRAB_STATUS 0x59   // R - Raw data grab status

/* Performance Optimization Registers (Proprietary) */
#define PMW3901_CONFIG_GROUP_0      0x7F    // R/W - Configuration group selector 0
// Note: Many performance registers are proprietary and accessed through 0x7F page selection


#define CHIP_ID         0x49  // 01001001
#define CHIP_ID_INVERSE 0xB6  // 10110110

typedef struct
{
    int16_t delta_x;// 往左为正，往右为负
    int16_t delta_y;// 往前为正，往后为负
    uint8_t motion;
    uint8_t squal;
    uint8_t raw_data;
    uint16_t shutter;
} PMW3901_Data_Type;

/**
 * @brief 初始化PMW3901
 */
void Int_PMW3901_Init(void);

/**
 * @brief 软复位PMW3901
 */
void Int_PMW3901_Reset(void);

/**
 * @brief 获取PMW3901运动数据
 * @param data 输出结构体
 */
void Int_PMW3901_Get_Data(PMW3901_Data_Type *data);

#endif // __INT_PMW3901_H__
