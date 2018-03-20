#include "board.h"
#include "application.h"
#include "stm32f10x_cfg.h"
#include "global.h"

int main(int argc, char **argv)
{
    board_init();
    ApplicationStartup();
    
    //should never reached here
    return 0;
}

