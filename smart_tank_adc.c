#include<lpc21xx.h>
#include"header.h"

void smart_tank_relay(u32 a){
    if(a==1){
        IOSET0=1<<10|1<<21;
			  i2c_write(0xA0,0x0B,1);   //1 = ON, 0 = OFF
        delay_ms(10);
    }
    else if(a==0){
			  IOCLR0=1<<10|1<<21;
			  i2c_write(0xA0,0x0B,0);   //1 = ON, 0 = OFF
        delay_ms(10);
        
    }
}

u32 smart_tank_adc_water_level(void){

    u32 adc_out_1;

    adc_out_1=adc_read(0);//adc water sensor
    if(adc_out_1 < 200)//low water
{
	  uart0_tx_string("======================================\r\n SMART WATER TANK MANAGEMENT SYSTEM \r\n======================================\r\n");
	  uart0_tx_string("Controller: LPC2129 ARM7\r\n");
	  uart0_tx_string("Project Status  : RUNNING\r\n");
	  smart_tank_rtc();//time and date when tank is low	
	  uart0_tx_string("\r\n");
    uart0_tx_string("Water Level:");
    uart0_integer(adc_out_1);
	  uart0_tx_string("\r\n");
	  
	  lcd_cmd(0x01);
		delay_ms(20);
		lcd_cmd(0x80);                     //1st row of lcd 16x2
		lcd_string("Water level:");        //water level on lcd
		lcd_integer(adc_out_1);             //water level on lcd
	
	  uart0_tx_string("Distance:");
    uart0_integer(10);//ultrasonic_distance_cm());
	  uart0_tx_string("\r\n");
	
	  lcd_cmd(0xc0);                     //1st row of lcd 16x2
		lcd_string("Distance:");        //Distance: on lcd
		lcd_integer(10);//ultrasonic_distance_cm());             //Distance: lcd
	  
	
    uart0_tx_string("PUMP :ON \r\n");
    uart0_tx_string("Relay :ON \r\n");
    uart0_tx_string("Valve Status:ON \r\n");
	  uart0_tx_string("TEMP:");
	  uart0_integer(smart_tank_adc_temp());
	  uart0_tx_string(" C\r\n");
		
		lcd_cmd(0x01);
		delay_ms(20);
		lcd_cmd(0x80);                     //1st row of lcd 16x2
		lcd_string("PUMP_STATUS:ON");        //water level on lcd
		lcd_cmd(0XC0);
		lcd_string("TEMP:");
		lcd_integer(smart_tank_adc_temp());             //water level on lcd
	
    //eeprom  water level save in memory 0x07 
    i2c_write(0xA0, 0x07,adc_out_1);//water level low
		delay_ms(10);
    //eeprom ultrasonic distance
		i2c_write(0xA0, 0x09, 10);//ultrasonic_distance_cm());  //water distance form ultrasonic sensor
		delay_ms(10);
    //eeprom temperature
		i2c_write(0xA0, 0x0A, smart_tank_adc_temp());  //surrounding temperature data
		delay_ms(10);	
		
		uart0_tx_string("EEPROM Status: DATA SAVED\r\n");
		if(wifi_status())
        {
            uart0_tx_string("Wi-Fi Status : CONNECTED\r\n");
            thingspeak_send();
					  uart0_tx_string("Cloud Status : UPDATED\r\n");
        }
        else
        {
            uart0_tx_string("WIFI Reconnecting...\r\n");

            wifi_connect();
        }

    delay_ms(20000);
				
		eeprom_read();
				
		uart0_tx_string("Message : TANK LOW \r\n");
				
		smart_tank_relay(1);
		
    return ((adc_out_1 - 120) * 100) / (900 - 120);

}
else if(adc_out_1 >= 800)//if tank is full
{
		uart0_tx_string("======================================\r\n SMART WATER TANK MANAGEMENT SYSTEM \r\n======================================\r\n");
	  uart0_tx_string("Controller: LPC2129 ARM7\r\n");
	  uart0_tx_string("Project Status  : RUNNING\r\n");
	  smart_tank_rtc();//time and date when tank is full
	  uart0_tx_string("\r\n");
    uart0_tx_string("Water Level:");
    uart0_integer(adc_out_1);
		uart0_tx_string("\r\n");
	
		lcd_cmd(0x01);
		delay_ms(20);
		lcd_cmd(0x80);                     //1st row of lcd 16x2
		lcd_string("Water level:");        //water level on lcd
		lcd_integer(adc_out_1);             //water level on lcd
	
	  uart0_tx_string("Distance:");
    uart0_integer(10);//ultrasonic_distance_cm());
	  uart0_tx_string("\r\n");
	
	  lcd_cmd(0xc0);                     //1st row of lcd 16x2
		lcd_string("Distance:");        //Distance: on lcd
		lcd_integer(10);//ultrasonic_distance_cm());             //Distance: lcd
	
	  uart0_tx_string("PUMP Status:OFF \r\n");
    uart0_tx_string("Relay Status:OFF \r\n");
    uart0_tx_string("Valve Status:OFF \r\n");
	  uart0_tx_string("TEMP:");
	  uart0_integer(smart_tank_adc_temp());
	  uart0_tx_string(" C\r\n");
	
	  lcd_cmd(0x01);
		delay_ms(20);
		lcd_cmd(0x80);                     //1st row of lcd 16x2
		lcd_string("PUMP_STATUS:OFF");        //water level on lcd
		lcd_cmd(0XC0);
		lcd_string("TEMP:");
		lcd_integer(smart_tank_adc_temp());             //water level on lcd  
	
    //eeprom  water level save in memory 0x07 & 0x08
    i2c_write(0xA0, 0x07, adc_out_1 & 0xFF);         //water level high,lower byte
		delay_ms(10);
		i2c_write(0xA0, 0x08, (adc_out_1 >> 8) & 0xFF);  //water level High,higher byte
		delay_ms(10);
		//eeprom ultrasonic distance
		i2c_write(0xA0, 0x09,10);// ultrasonic_distance_cm());  //water distance form ultrasonic sensor
		delay_ms(10);
		//eeprom temperature
		i2c_write(0xA0, 0x0A, smart_tank_adc_temp());  //surrounding temperature data
		delay_ms(10);
		
		uart0_tx_string("EEPROM Status: DATA SAVED\r\n");
		
		if(wifi_status())
        {
						uart0_tx_string("Wi-Fi Status : CONNECTED\r\n");
            thingspeak_send();
					  uart0_tx_string("Cloud Status : UPDATED\r\n");
        }
        else
        {
            uart0_tx_string("WIFI Reconnecting...\r\n");

            wifi_connect();
        }

    delay_ms(20000);
		
		eeprom_read();
				
		uart0_tx_string("Message : TANK FULL \r\n");
				
		smart_tank_relay(0);
		
	  return ((adc_out_1 - 120) * 100) / (900 - 120);
}
		else{
		return ((adc_out_1 - 120) * 100) / (900 - 120);
		}

}

u32 smart_tank_adc_ultrasonic(void){
  u32 adc_out_2; 
  adc_out_2=ultrasonic_distance_cm();
  
	return adc_out_2;
	}
  	
	//adc ultrasonic sensor 
		


int smart_tank_adc_temp(void){
  u32 adc_out_3;
  float vout,temp;  
	adc_out_3=adc_read(2);//adc temp sensor 
	vout=(adc_out_3*3.3)/1023;
	temp=(vout-0.5)/0.01;
	return (int)temp;
}

/* 
u8 sec,min,hour,ampm,date,month,year
sec   = i2c_read(0xA0, 0x00);
min   = i2c_read(0xA0, 0x01);
hour  = i2c_read(0xA0, 0x02);
ampm  = i2c_read(0xA0, 0x03);
date  = i2c_read(0xA0, 0x04);
month = i2c_read(0xA0, 0x05);
year  = i2c_read(0xA0, 0x06);

note : while reading water level
u16 water_level;

water_level  = i2c_read(0xA0, 0x07);
water_level |= (u16)i2c_read(0xA0, 0x08) << 8;

u8 distance;
distance = i2c read(0xA0, 0x09);

u8 temperature;
temperature - u2c read(0xA0, 0x0A);
*/

