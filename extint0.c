#include<lpc21xx.h>
#include"header.h"

extern int flag;

void extint(void)__irq{
    flag=1;
    EXTINT=1;
    VICVectAddr=0;
}

void config_ext(void) {
    PINSEL1|=1;//p0.16
    EXTMODE=1;
    EXTPOLAR=0;
    EXTINT=1;
    VICVectAddr0=(u32)extint;
    VICVectCntl0=(1<<5)|14;
    VICIntSelect=0;
    VICIntEnable|=1<<14;
}