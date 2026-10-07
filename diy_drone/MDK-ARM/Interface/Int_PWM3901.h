#ifndef __INT_PWM3901_H__
#define __INT_PWM3901_H__

#include "FreeRTOS.h"
#include "task.h"
#include "spi.h"
#include "Com_debug.h"

#define PMW3901_CS_LOW            HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET)
#define PMW3901_CS_HIGH           HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET)
#define PMW3901_RESET_LOW         HAL_GPIO_WritePin(PMW_RST_GPIO_Port, PMW_RST_Pin, GPIO_PIN_RESET)
#define PMW3901_RESET_HIGH        HAL_GPIO_WritePin(PMW_RST_GPIO_Port, PMW_RST_Pin, GPIO_PIN_SET)

#define PMW3901_REG_PRODUCT_ID    0x00
#define PMW3901_REG_MOTION        0x02
#define PMW3901_REG_DELTA_X       0x03
#define PMW3901_REG_DELTA_Y       0x04
#define PMW3901_REG_SQUAL         0x05
#define PMW3901_REG_SHUTTER       0x06
#define PMW3901_REG_RAW_DATA      0x08

#define CHIP_ID         0x49  // 01001001
#define CHIP_ID_INVERSE 0xB6  // 10110110

typedef struct
{
    int16_t delta_x;
    int16_t delta_y;
    uint8_t motion;
    uint8_t squal;
    uint8_t raw_data;
    uint16_t shutter;
} PMW3901_Data_Type;

/**
 * @brief 初始化PMW3901
 */
void Int_PWM3901_Init(void);

/**
 * @brief 软复位PMW3901
 */
void Int_PWM3901_Reset(void);

/**
 * @brief 获取PMW3901运动数据
 * @param data 输出结构体
 */
void Int_PWM3901_Get_Data(PMW3901_Data_Type *data);

#endif // __INT_PWM3901_H__
