#ifndef __COM_TOOL_H__
#define __COM_TOOL_H__

#include "main.h"

/**
 * @brief 限制某个数值在一定的范围内
 * @param value 要限制的数值
 * @param min 最小值
 * @param max 最大值
 * @return 限制后的数值
 */
int16_t Com_Tool_Limit(int16_t value, int16_t min, int16_t max);

#endif // __COM_TOOL_H__
