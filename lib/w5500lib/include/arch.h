#ifndef __ARCH_H
#define __ARCH_H

#include <stdio.h>

/*FP for Sleep/Delay Fn*/
typedef void (*sleep)(uint32_t);

sleep msleep;

/*register callback*/
void reg_wizchip_sleep_cbfunc(sleep);

#endif