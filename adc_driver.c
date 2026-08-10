#include<lpc21xx.h>
#include"header.h"

void adc_init(void){
PINSEL1|=0x15400000;
ADCR=0x00200400;
}

unsigned int adc_read(unsigned char ch_num){
unsigned int temp;
ADCR|=(1<<ch_num);
ADCR|=(1<<24);
while(((ADDR>>31)&1)==0);
ADCR^=(1<<ch_num);
ADCR^=(1<<24);
temp=((ADDR>>6)&0x3ff);
return temp;
}
