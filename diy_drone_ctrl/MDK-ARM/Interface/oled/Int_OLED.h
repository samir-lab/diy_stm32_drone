#ifndef __INT_OLED_H__
#define __INT_OLED_H__ 

#include "main.h"
#include "gpio.h"
#include "Com_debug.h"

#define OLED_CS_GPIO GPIOA
#define OLED_CS_Pin GPIO_PIN_4

#define OLED_DC_GPIO GPIOB
#define OLED_DC_Pin GPIO_PIN_1

#define OLED_RST_GPIO GPIOB
#define OLED_RST_Pin GPIO_PIN_0

#define OLED_SDIN_GPIO GPIOA
#define OLED_SDIN_Pin GPIO_PIN_7

#define OLED_SCLK_GPIO GPIOA
#define OLED_SCLK_Pin GPIO_PIN_5

//-----------------OLED端口定义----------------  					   
#define OLED_CS_Clr()  HAL_GPIO_WritePin(OLED_CS_GPIO, OLED_CS_Pin, GPIO_PIN_RESET)
#define OLED_CS_Set()  HAL_GPIO_WritePin(OLED_CS_GPIO, OLED_CS_Pin, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO, OLED_DC_Pin, GPIO_PIN_RESET)
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO, OLED_DC_Pin, GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RST_GPIO, OLED_RST_Pin, GPIO_PIN_RESET)
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RST_GPIO, OLED_RST_Pin, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_SDIN_GPIO, OLED_SDIN_Pin, GPIO_PIN_RESET)
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_SDIN_GPIO, OLED_SDIN_Pin, GPIO_PIN_SET)

#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_SCLK_GPIO, OLED_SCLK_Pin, GPIO_PIN_RESET)
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_SCLK_GPIO, OLED_SCLK_Pin, GPIO_PIN_SET)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

// OLED控制用函数   		    
void OLED_Init(void);
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);	
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p, uint8_t size, uint8_t mode);	 

#endif // __INT_OLED_H__
