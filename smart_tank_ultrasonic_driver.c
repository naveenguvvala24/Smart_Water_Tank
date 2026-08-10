#include <LPC21xx.h>
#include "header.h"

#define TRIG (1<<12)
#define ECHO (1<<13)


void ultrasonic_init(void)
{
    IODIR0 |= TRIG;      // Trigger Output
    IODIR0 &= ~ECHO;     // Echo Input

    IOCLR0 = TRIG;
}

void delay_us(unsigned int us)
{
    unsigned int i,j;

    for(i=0;i<us;i++)
    {
        for(j=0;j<15;j++);
    }
}


unsigned int ultrasonic_distance_cm(void)
{
    unsigned int count=0;
    unsigned int distance;

    IOCLR0 = TRIG;
    delay_us(2);

    IOSET0 = TRIG;
    delay_us(10);

    IOCLR0 = TRIG;

    while(!(IOPIN0 & ECHO));


    while(IOPIN0 & ECHO)
    {
        count++;
        delay_us(1);
    }

    distance = count / 58;

    return distance;
}
