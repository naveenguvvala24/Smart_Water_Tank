#include<lpc21xx.h>
#include"header.h"

#define sw1 ((IOPIN0>>14)&1)
#define sw2 ((IOPIN0>>15)&1)
//#define sw3 ((IOPIN0>>16)&1)

volatile int flag=0;

int main(){
	IODIR0|=(1<<10) | (1<<21);
	u32 res,adc_out1,adc_out2,c=0;
	int adc_out3;
	uart0_init(9600);
	uart1_init_esp(115200);
	ultrasonic_init();
	i2c_init();
	lcd_init();
	adc_init();
	config_ext();
	uart0_tx_string("init \r\n");
	//can1_init();
	uart0_tx_string("CAN init \r\n");
	
	
	if(esp_init())
    {
        uart0_tx_string("ESP Ready\r\n");

        if(wifi_connect())
        {
            uart0_tx_string("WiFi Connected\r\n");
        }
    }
		uart0_tx_string("esp init\r\n");
		c=1;//count increment to 1,so then program boots all data will be printed on uart,eeprom,lcd,can,esp01
		
while(1){
	uart0_tx_string("inside while(1)\r\n");
	while(flag){
	uart0_tx_string("Manual mode\r\n");
	if(sw1==1){ //manula mode ,if sw1 is pressed relay will turn on
		uart0_tx_string("if sw1\r\n");
	delay_ms(50);
		while(sw1==0);
		smart_tank_relay(1);
		uart0_tx_string("MANUAL MODE \r\n PUMP ON\r\n");
		lcd_cmd(0x01);
		delay_ms(20);
		lcd_cmd(0x80);
		lcd_string("MANUAL MODE");
		lcd_cmd(0xC0);
		lcd_string("PUMP ON");
		delay_ms(100);
	}
	if(sw2==1){ //manula mode ,if sw2 is pressed relay will turn off
		uart0_tx_string("if sw1\r\n");
	delay_ms(50);
		while(sw2==0);
		smart_tank_relay(0);
		uart0_tx_string("MANUAL MODE \r\n PUMP OFF\r\n");
		lcd_cmd(0x01);
		delay_ms(20);
		lcd_cmd(0x80);
		lcd_string("MANUAL MODE");
		lcd_cmd(0xC0);
		lcd_string("PUMP OFF");
		delay_ms(100);
		flag=0;
		break;
	}
}
	uart0_tx_string("inside while(1) after if\r\n");
adc_out1=smart_tank_adc_water_level();//input function of water level

adc_out3=smart_tank_adc_temp();//input function of temp

uart0_tx_string("inside while(1) after water level and temp\r\n");
res=smart_tank_timer();//taking data from rtc is time is 30mins to update data and print data
//uart0_integer(res);
	while(res || c){	//if res is 1 then update data and print data
		
		
		uart0_tx_string("======================================\r\n SMART WATER TANK MANAGEMENT SYSTEM \r\n======================================\r\n");
	  uart0_tx_string("Controller: LPC2129 ARM7");
	  uart0_tx_string("Project Status  : RUNNING");
		smart_tank_rtc();//rtc data printed on lcd ,uart and saved to eeprom
		
		uart0_tx_string("\r\n");
		uart0_tx_string("\r\nWater Level");//water level on uart
		uart0_integer(adc_out1);           //water level on uart
		lcd_cmd(0x01);
		delay_ms(20);
		lcd_cmd(0x80);                     //1st row of lcd 16x2
		lcd_string("Water level:");        //water level on lcd
		lcd_integer(adc_out1);             //water level on lcd
		//eeprom water level save in memory 0x07 & 0x08
    i2c_write(0xA0, 0x07, adc_out1 & 0xFF);         //Low byte
		delay_ms(10);
		i2c_write(0xA0, 0x08, (adc_out1 >> 8) & 0xFF);  //High byte
		delay_ms(10);		
		
		adc_out2=smart_tank_adc_ultrasonic();//input function of ultrasonic
		uart0_tx_string("\r\nDistance");   //distance on uart
		uart0_integer(10);//adc_out2);           //distance on uart
		lcd_cmd(0xc0);                     //2nd row of lcd 16x2
		lcd_string("Distance:");           //distance on lcd
		lcd_integer(10);//adc_out2);             //distance on lcd
		//eeprom ultrasonic distance
		i2c_write(0xA0, 0x09, 10);//adc_out2);  //water distance form ultrasonic sensor
		delay_ms(10);
		
		uart0_tx_string("PUMP Status:OFF \r\n");
    uart0_tx_string("Relay Status:OFF \r\n");
    uart0_tx_string("Valve Status:OFF \r\n");
		uart0_tx_string("\r\nTemperature:");//temp on uart
		uart0_integer(adc_out3);            //temp on uart
		uart0_tx_string(" C\r\n");
		lcd_cmd(0x01);                      //lcd screen clear and select 1st rows as default
		lcd_string("Temperature:");         //temp on lcd
		lcd_integer(adc_out3);              //temp on lcd
		lcd_cmd(0xc0);
		lcd_string("Pump_status:OFF");
		//eeprom temperature
		i2c_write(0xA0, 0x0A, adc_out3);  //surrounding temperature data
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
		
		if(res){
		uart0_tx_string("Message:30 mins data update\r\n");
		}
		if(c){
		uart0_tx_string("Message:Smart tank is poweredup\r\n");
		}
		delay_ms(200);//small delay before break
		c=0;//count go to zero
		break;
}
}
}
