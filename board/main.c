/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "board.h"
#include "application.h"
#include "stm32f10x_cfg.h"
#include "global.h"

/* stm32f103r8t6:64k 20k */
int main(int argc, char **argv)
{
    board_init();
    ApplicationStartup();
    
    /* should never reached here */
    return 0;
}

