/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_sig.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"

/**
 * @brief get chip id
 * @param id - chip id
 * @param len - id length
 */
void Get_ChipID(uint32_t *data, uint8_t *len)
{
    assert_param(NULL != data);
    data[2] = *(uint32_t *)(0X1FFFF7E8);
    data[1] = *(uint32_t *)(0X1FFFF7EC);
    data[0] = *(uint32_t *)(0X1FFFF7F0);
    *len = 3;
}
