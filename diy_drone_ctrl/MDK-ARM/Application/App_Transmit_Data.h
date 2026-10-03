#ifndef __App_Transmit_Data_H__
#define __App_Transmit_Data_H__

#include "Int_SI24R1.h"
#include "App_Data_Mixer.h"

// 定义帧头校验3字节
#define FRAME_HEADER_CHECK_1 'G'
#define FRAME_HEADER_CHECK_2 'K'
#define FRAME_HEADER_CHECK_3 'Q'

/**
 * @brief 自动切换SI24R1模式，发送数据
 */
void App_Transmit_Data(void);

#endif // __App_Transmit_Data_H__
