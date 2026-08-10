#include<lpc21xx.h>
#include"header.h"

void lcd_data(unsigned char data){
unsigned int temp;

//higher nibbel

IOCLR1=0xFE<<16;
temp=((data&0xf0)<<16);
IOSET1=temp;
IOSET1=1<<17;
IOCLR1=1<<18;
IOSET1=1<<19;
delay_ms(2);
IOCLR1=1<<19;

//lower nibbel

IOCLR1=0xFE<<16;
temp=((data&0x0f)<<20);
IOSET1=temp;
IOSET1=1<<17;
IOCLR1=1<<18;
IOSET1=1<<19;
delay_ms(2);
IOCLR1=1<<19;
}

void lcd_cmd(unsigned char cmd){
unsigned int temp;

//higher nibbel

IOCLR1=0xFE<<16;
temp=((cmd&0xf0)<<16);
IOSET1=temp;
IOCLR1=1<<17;
IOCLR1=1<<18;
IOSET1=1<<19;
delay_ms(2);
IOCLR1=1<<19;

//lower nibbel

IOCLR1=0xFE<<16;
temp=((cmd&0x0f)<<20);
IOSET1=temp;
IOCLR1=1<<17;
IOCLR1=1<<18;
IOSET1=1<<19;
delay_ms(2);
IOCLR1=1<<19;
}

void lcd_init(){
IODIR1=0xFE<<16;
PINSEL2=0X0;
lcd_cmd(0x02);
lcd_cmd(0x28);
lcd_cmd(0x0e);
lcd_cmd(0x01);
}

void lcd_string(char *ptr){

while(*ptr!=0){
lcd_data(*ptr);
ptr++;
}

}

void lcd_integer(int num){
int a[10],i;
if(num==0)
lcd_data('0');

if(num < 0){
num=-num;
lcd_data('-');
}

while(num>0){
a[i]=num%10+48;
num=num/10;
i++;
}
for(i=i-1;i>=0;i--){
lcd_data(a[i]);
}
}

void lcd_cgram(void)
{
unsigned char a[] = {0x05,0x01,0x05,0x12,0x04,0x34,0x45,0x00}; // Custom character pattern
lcd_cmd(0x40);
for(int i = 0; i < 8; i++)
{
lcd_data(a[i]);
}
}
