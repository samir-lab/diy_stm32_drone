#include "App_Transmit_Data.h"

// 待发送遥控数据
Remote_Ctrl_Data_Type remote_ctrl_data;
// 接收到飞控数据
Flight_Ctrl_Data_Type flight_ctrl_data;

// 通讯任务发送数据缓冲区
uint8_t comm_tx_buffer[SI24R1_TX_PLOAD_WIDTH] = {0}; 
void App_Transmit_Data(void)
{
    // 进入发送模式
	Int_SI24R1_TX_Mode();
    // 调用SI24R1接口，发送数据
    // 1. 发送数据 => 唯一性和可靠性
    // 1.1 帧头校验，指定发送给对应的设备，唯一性
    // 1.2 末尾添加校验和，可靠性

    // 数据本身11字节，帧头设置3字节，校验和设置4字节，总共18字节
    // 帧头
    comm_tx_buffer[0] = FRAME_HEADER_CHECK_1;
    comm_tx_buffer[1] = FRAME_HEADER_CHECK_2;
    comm_tx_buffer[2] = FRAME_HEADER_CHECK_3;
    // 数据本身
    comm_tx_buffer[3] = (remote_ctrl_data.throttle >> 8) & 0xFF;
    comm_tx_buffer[4] = remote_ctrl_data.throttle & 0xFF;
    comm_tx_buffer[5] = (remote_ctrl_data.yaw >> 8) & 0xFF;    
    comm_tx_buffer[6] = remote_ctrl_data.yaw & 0xFF;
    comm_tx_buffer[7] = (remote_ctrl_data.pitch >> 8) & 0xFF;
    comm_tx_buffer[8] = remote_ctrl_data.pitch & 0xFF;
    comm_tx_buffer[9] = (remote_ctrl_data.roll >> 8) & 0xFF;
    comm_tx_buffer[10] = remote_ctrl_data.roll & 0xFF;
    comm_tx_buffer[11] = remote_ctrl_data.shutdown;
    comm_tx_buffer[12] = remote_ctrl_data.fixed_height;
    comm_tx_buffer[13] = remote_ctrl_data.fixed_position;
    comm_tx_buffer[14] = (remote_ctrl_data.pid_p_pitch >> 8) & 0xFF;
    comm_tx_buffer[15] = remote_ctrl_data.pid_p_pitch & 0xFF;
    comm_tx_buffer[16] = (remote_ctrl_data.pid_p_roll >> 8) & 0xFF;
    comm_tx_buffer[17] = remote_ctrl_data.pid_p_roll & 0xFF;
    // 校验和
    uint32_t checksum = 0;
    for(uint8_t i = 0; i < 18; i++)
    {
        checksum += comm_tx_buffer[i];
    }
    comm_tx_buffer[18] = (checksum >> 24) & 0xFF;
    comm_tx_buffer[19] = (checksum >> 16) & 0xFF;
    comm_tx_buffer[20] = (checksum >> 8) & 0xFF;
    comm_tx_buffer[21] = checksum & 0xFF;

    // debug_printf("TX: %d,%d,%d,%d,%d,%d,%d\n", 
    //     remote_ctrl_data.throttle, 
    //     remote_ctrl_data.yaw, 
    //     remote_ctrl_data.pitch, 
    //     remote_ctrl_data.roll,
    //     remote_ctrl_data.shutdown,
    //     remote_ctrl_data.fixed_height,
    //     remote_ctrl_data.fixed_position);

    uint8_t res = 1;
    uint8_t retry_count = 0;
    do
    {
        retry_count++;
        res = Int_SI24R1_TxPacket(comm_tx_buffer);
        vTaskDelay(10);
    } while (res == 1 && retry_count < MAX_RETRY_COUNT);
    
    // 发送完成，转为接收模式
	Int_SI24R1_RX_Mode();
}

// 通讯任务接收数据缓冲区
uint8_t comm_rx_buffer[SI24R1_TX_PLOAD_WIDTH] = {0}; 
void App_Receive_Data(void)
{
    // 1. 清空接收缓冲区
    memset(comm_rx_buffer, 0, SI24R1_TX_PLOAD_WIDTH);
    // 2. 接收数据到缓冲区
    uint8_t res = 1;
    uint8_t retry_count = 0;
    do
    {
        retry_count++;
        res = Int_SI24R1_RxPacket(comm_rx_buffer);
        vTaskDelay(10);
    } while (res == 1 && retry_count < MAX_RETRY_COUNT);

    if(res == 1)
    {
        return; // 接收失败
    }
    // 解析数据
    // 1.帧头校验
    if(comm_rx_buffer[0] != FRAME_HEADER_CHECK_1 
        || comm_rx_buffer[1] != FRAME_HEADER_CHECK_2 
        || comm_rx_buffer[2] != FRAME_HEADER_CHECK_3)
    {
        return; // 校验失败
    }
    // 2. 校验和
    uint32_t checksum = 0;
    // 高位在前
    uint32_t recived_checksum = (comm_rx_buffer[9] << 24) 
                                | (comm_rx_buffer[10] << 16) 
                                | (comm_rx_buffer[11] << 8) 
                                | comm_rx_buffer[12];
    for(uint8_t i = 0; i < 9; i++)
    {
        checksum += comm_rx_buffer[i];
    }
    if(checksum != recived_checksum)
    {
        return; // 校验失败
    }

    // 3. 解析数据到结构体
    flight_ctrl_data.voltage = (comm_rx_buffer[3] << 8) | comm_rx_buffer[4];
    flight_ctrl_data.pid_p_pitch = (comm_rx_buffer[5] << 8) | comm_rx_buffer[6];
    flight_ctrl_data.pid_p_roll = (comm_rx_buffer[7] << 8) | comm_rx_buffer[8];
}
