#include <lpc21xx.h>
#include "header.h"

#define THRE1   ((U1LSR>>5)&1)
#define RDR1    (U1LSR&1)



void uart1_init_esp(unsigned int baud)
{
    unsigned int pclk;
    unsigned int result;

    if(VPBDIV==0x00)
        pclk=15000000;

    else if(VPBDIV==0x01)
        pclk=60000000;

    else
        pclk=30000000;

    result=pclk/(16*baud);

    /* Select TXD1(P0.8) RXD1(P0.9) */

    PINSEL0 &= ~((3<<16)|(3<<18));
    PINSEL0 |=  ((1<<16)|(1<<18));

    U1LCR=0x83;

    U1DLL=result&0xFF;
    U1DLM=(result>>8)&0xFF;

    U1LCR=0x03;
}



void uart1_tx_esp(unsigned char data)
{
    while(THRE1==0);

    U1THR=data;
}



unsigned char uart1_rx_esp(void)
{
    while(RDR1==0);

    return U1RBR;
}



unsigned char uart1_rx_esp01(unsigned char *data)
{
    if(RDR1)
    {
        *data=U1RBR;

        return 1;
    }

    return 0;
}



void uart1_tx_string_esp(char *ptr)
{
    while(*ptr)
    {
        uart1_tx_esp(*ptr);

        ptr++;
    }
}



void uart1_time_div_esp(unsigned char t)
{
    uart1_tx_esp((t/10)+'0');

    uart1_tx_esp((t%10)+'0');
}



void uart1_integer_esp(int num)
{
    int a[10];

    int i=0;

    if(num==0)
    {
        uart1_tx_esp('0');

        return;
    }

    if(num<0)
    {
        uart1_tx_esp('-');

        num=-num;
    }

    while(num>0)
    {
        a[i]=(num%10)+'0';

        num/=10;

        i++;
    }

    while(i)
    {
        uart1_tx_esp(a[--i]);
    }
}