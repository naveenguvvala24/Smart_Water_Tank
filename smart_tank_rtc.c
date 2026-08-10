/*main_rtc.c*/
#include<lpc21xx.h>
#include "header.h"


//volatile int flag;
u8 am_pm;

u32 bcd_to_int(char rtc){
	return ((rtc/16)*10)+(rtc%16);
}

void time_div1(char t){
	lcd_data((t/16)+48);
	lcd_data((t%16)+48);
}

u32 smart_tank_timer(void){
	u32 m,s;
	m=i2c_read(0xD1,0x1);//read mins
	s=i2c_read(0xD1,0x0);//read secs
	if(((m==0x00)||(m==0x30)) && ((s==0x00) || (s==0x30))){
	//if((s==0x10) || (s==0x20) || (s==0x30) || (s==0x40)|| (s==0x50) || (s==0x00)){
		return 1;
	}
	else{
		return 0;
	}
}

void smart_tank_rtc(void){
	u8 h,h12,m,s,date,month,year;

	//uart0_tx_string("1 - init ok\r\n");

		h=i2c_read(0xD1,0x2);//read hrs
		if(h>0x12){
			h=(i2c_read(0xD1,0x2));//read hrs
			if(h>0x12){
				h=(h-0x12);//convert 24 hours to 12 hours
				//uart0_tx_string("24 to 12 - ok\r\n");
			}
		}
	
		
	
	
	/*read & display rtc time on lcd*/
		
		year=i2c_read(0xD1,0x6);//read year
		month=i2c_read(0xD1,0x5);//read month
		date=i2c_read(0xD1,0x4);//read date
		//day=i2c_read(0xD1,0x3);//read day
		//h=i2c_read(0xD1,0x2);//read hrs
		m=i2c_read(0xD1,0x1);//read mins
		s=i2c_read(0xD1,0x0);//read secs
		//uart0_time_div(m);
		
		
		
		lcd_cmd(0x80);
        lcd_string("Date:");
        uart0_tx_string("RTC Date :");
        
        uart0_time_div(date);
        uart0_tx('/');
		time_div1(date);
		lcd_data('/');

        uart0_time_div(month);
        uart0_tx_string("/20");
        uart0_time_div(year);
				uart0_tx_string("\r\n");

		time_div1(month);
		lcd_string("/20");
		time_div1(year);
		lcd_string("  ");

		//eeprom
		i2c_write(0xA0, 0x00, bcd_to_int(s));//s
		delay_ms(10);


		i2c_write(0xA0, 0x01, bcd_to_int(m));//m

		delay_ms(10);


		i2c_write(0xA0, 0x02, bcd_to_int(h));//h

		delay_ms(10);

		i2c_write(0xA0, 0x04, bcd_to_int(date));//DATE

		delay_ms(10);


		i2c_write(0xA0, 0x05, bcd_to_int(month));//MONTH

		delay_ms(10);


		i2c_write(0xA0, 0x06, bcd_to_int(year));//year

		delay_ms(10);

		
		lcd_cmd(0xc0);
       lcd_string("Time:");
        uart0_tx_string("RTC TIME:");

        uart0_time_div(h);
        uart0_tx(':');
		time_div1(h);
		lcd_data(':');
		
		uart0_time_div(m);
        uart0_tx(':');
        time_div1(m);
		lcd_data(':');

		uart0_time_div(s);
        uart0_tx(':');
		time_div1(s);
		lcd_string(" ");
		
        h12=i2c_read(0xD1,0x2);//read hrs
		if(h12>0x12){
		uart0_tx_string("PM");
    	lcd_string("PM");
		i2c_write(0xA0, 0x03, 'P');   // A = AM, P = PM
		delay_ms(10);
	  }else{
    	uart0_tx_string("AM");
		lcd_string("AM");
		i2c_write(0xA0, 0x03, 'A');   // A = AM, 1 = PM
		delay_ms(10);
		uart0_tx_string("\r\n");}

		
		
		delay_ms(500);
		
		
		
	}
	
		

	/*note : to reaad the data from eeprom  
sec   = i2c_read(0xA0, 0x00);
min   = i2c_read(0xA0, 0x01);
hour  = i2c_read(0xA0, 0x02);
ampm  = i2c_read(0xA0, 0x03);
date  = i2c_read(0xA0, 0x04);
month = i2c_read(0xA0, 0x05);
year  = i2c_read(0xA0, 0x06);
	*/
