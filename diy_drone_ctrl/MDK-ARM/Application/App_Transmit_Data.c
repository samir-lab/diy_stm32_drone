#include "App_Transmit_Data.h"

extern Mixed_Ctrl_Data_Type mixed_ctrl_data;

// 通讯任务发送数据缓冲区
uint8_t comm_tx_buff[SI24R1_TX_PLOAD_WIDTH] = {0}; 
void App_Transmit_Data(void)
{
    // 调用SI24R1接口，发送数据
    // 1. 进入TX模式
    Int_SI24R1_TX_Mode();
    // 2. 发送数据 => 唯一性和可靠性
    // 2.1 帧头校验，指定发送给对应的设备，唯一性
    // 2.2 末尾添加校验和，可靠性

    // 数据本身11字节，帧头设置3字节，校验和设置4字节，总共18字节
    // 帧头
    comm_tx_buff[0] = FRAME_HEADER_CHECK_1;
    comm_tx_buff[1] = FRAME_HEADER_CHECK_2;
    comm_tx_buff[2] = FRAME_HEADER_CHECK_3;
    // 数据本身
    comm_tx_buff[3] = (mixed_ctrl_data.throttle >> 8) & 0xFF;
    comm_tx_buff[4] = mixed_ctrl_data.throttle & 0xFF;
    comm_tx_buff[5] = (mixed_ctrl_data.yaw >> 8) & 0xFF;    
    comm_tx_buff[6] = mixed_ctrl_data.yaw & 0xFF;
    comm_tx_buff[7] = (mixed_ctrl_data.pitch >> 8) & 0xFF;
    comm_tx_buff[8] = mixed_ctrl_data.pitch & 0xFF;
    comm_tx_buff[9] = (mixed_ctrl_data.roll >> 8) & 0xFF;
    comm_tx_buff[10] = mixed_ctrl_data.roll & 0xFF;
    comm_tx_buff[11] = mixed_ctrl_data.shutdown;
    comm_tx_buff[12] = mixed_ctrl_data.fixed_height;
    comm_tx_buff[13] = mixed_ctrl_data.fixed_position;
    // 校验和
    uint32_t checksum = 0;
    for(int i = 0; i < 14; i++)
    {
        checksum += comm_tx_buff[i];
    }
    comm_tx_buff[14] = (checksum >> 24) & 0xFF;
    comm_tx_buff[15] = (checksum >> 16) & 0xFF;
    comm_tx_buff[16] = (checksum >> 8) & 0xFF;
    comm_tx_buff[17] = checksum & 0xFF;

    // debug_printf("TX: %d,%d,%d,%d,%d,%d,%d\n", 
    //     mixed_ctrl_data.throttle, 
    //     mixed_ctrl_data.yaw, 
    //     mixed_ctrl_data.pitch, 
    //     mixed_ctrl_data.roll,
    //     mixed_ctrl_data.shutdown,
    //     mixed_ctrl_data.fixed_height,
    //     mixed_ctrl_data.fixed_position);

    Int_SI24R1_TxPacket(comm_tx_buff);
    // 3. 恢复RX模式
    Int_SI24R1_RX_Mode();
}
