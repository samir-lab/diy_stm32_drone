#ifndef __APP_RECEIVE_DATA_H__
#define __APP_RECEIVE_DATA_H__

#include "Com_config.h"
#include "Int_SI24R1.h"

// 定义帧头校验3字节
#define FRAME_HEADER_CHECK_1 'G'
#define FRAME_HEADER_CHECK_2 'K'
#define FRAME_HEADER_CHECK_3 'Q'

// 最大重试次数
#define MAX_RETRY_COUNT 1000 // 6秒内都通讯失败

/**
 * @brief 接收遥控器数据
 * @return 0:接收成功 1:校验/接收失败
 */
uint8_t App_Receive_Data(void);

/**
 * @brief 检查遥控器连接状态
 * @param conn_result 连接结果，0:接收成功 1:接收失败
 */
void App_Check_Remote_State(uint8_t conn_result);

/**
 * @brief 检查飞行状态
 */
void App_Check_Flight_State(void);

#endif // __APP_RECEIVE_DATA_H__
