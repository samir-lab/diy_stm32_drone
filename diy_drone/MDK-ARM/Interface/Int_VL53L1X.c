#include "Int_VL53L1X.h"

/**
 * @brief 初始化VL53L1X传感器
 */
uint16_t dev = 0x52;
void Int_VL53L1X_Init(void)
{
    // 0. 检查I2C总线是否正常
    HAL_StatusTypeDef i2c2_status = HAL_I2C_IsDeviceReady(&hi2c2, dev, 3, 5000);
    if (i2c2_status != HAL_OK)
    {
        debug_printf("VL53L1X I2C Bus Error\n");
        return;
    }

    // 1. 重置芯片 => XShot引脚先拉低再拉高
    HAL_GPIO_WritePin(VX_XSHUT_GPIO_Port, VX_XSHUT_Pin, GPIO_PIN_RESET);
    vTaskDelay(10);
    HAL_GPIO_WritePin(VX_XSHUT_GPIO_Port, VX_XSHUT_Pin, GPIO_PIN_SET);
    vTaskDelay(10);

    // 2. 读取数据，检查是否正常
    uint8_t byteData;
    uint16_t wordData;
    /* Those basic I2C read functions can be used to check your own I2C functions */
    VL53L1_RdByte(dev, 0x010F, &byteData);
    debug_printf("VL53L1X Model_ID: %X\n", byteData);
    VL53L1_RdByte(dev, 0x0110, &byteData);
    debug_printf("VL53L1X Module_Type: %X\n", byteData);
    VL53L1_RdWord(dev, 0x010F, &wordData);
    debug_printf("VL53L1X: %X\n", wordData);

    if(wordData == 0){
        debug_printf("VL53L1X Read Data Error\n");
        return;
    }

    // 3. 检查传感器是否正常启动
    uint8_t sensorState = 0;
    while(sensorState==0){
        VL53L1X_BootState(dev, &sensorState);
        vTaskDelay(2);
    }
    debug_printf("VL53L1X Chip booted\n");

    // 4.配置传感器
    /* This function must to be called to initialize the sensor with the default setting  */
    VL53L1X_SensorInit(dev);
    /* Optional functions to be used to change the main ranging parameters according the application requirements to get the best ranging performances */
    VL53L1X_SetDistanceMode(dev, 2); /* 1=short, 2=long */
    VL53L1X_SetTimingBudgetInMs(dev, 20); /* in ms possible values [20, 50, 100, 200, 500] */
    VL53L1X_SetInterMeasurementInMs(dev, 20); /* in ms, IM must be > = TB */
    VL53L1X_SetInterruptPolarity(dev, 0); /* force active-high; some chips remain in a low-polarity reset state and then never satisfy the old ready check */
    // VL53L1X_SetOffset(dev,20); /* offset compensation in mm */
    // VL53L1X_SetROI(dev, 16, 16); /* minimum ROI 4,4 */
    // VL53L1X_CalibrateOffset(dev, 140, &offset); /* may take few second to perform the offset cal*/
    // VL53L1X_CalibrateXtalk(dev, 1000, &xtalk); /* may take few second to perform the xtalk cal */
    debug_printf("VL53L1X Init Ok!\n");

    VL53L1X_StartRanging(dev);   /* This function has to be called to enable the ranging */
}

/**
 * @brief 判断VL53L1X传感器数据是否准备好
 * @return 1:准备好，0:未准备好
 */
uint8_t Int_VL53L1X_Is_Data_Ready(void)
{
    uint8_t dataReady = 0;
    VL53L1X_CheckForDataReady(dev, &dataReady);
    return dataReady == 0 ? 0 : 1;
}

/**
 * @brief 获取VL53L1X传感器的距离值
 */
uint16_t Int_VL53L1X_Get_Distance(void)
{
    // 等待测量完成
    // uint8_t dataReady = 0;
    // uint16_t timeout_counter = 0;
    // while (dataReady == 0)
    // {
    //     VL53L1X_CheckForDataReady(dev, &dataReady);
    //     timeout_counter++;
    //     if (timeout_counter >= 1000)
    //     {
    //         printf("No data ready for long time, please check your system\n");
    //         timeout_counter = 0;
    //     }
    //     VL53L1_WaitMs(dev, 1);
    // }
    // dataReady = 0;
    // timeout_counter = 0;

    uint16_t distance = 0;
    VL53L1X_GetDistance(dev, &distance);
    VL53L1X_ClearInterrupt(dev);
    return distance;
}
