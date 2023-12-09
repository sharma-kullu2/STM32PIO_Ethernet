#include "arch.h"

//Default Sleep fn

void Sleep(uint32_t delay){ printf("==========Arch.c-------\n");} //empty implentation

void reg_wizchip_sleep_cbfunc(sleep fn){
    if (fn) msleep = fn;
    else msleep=Sleep;
}