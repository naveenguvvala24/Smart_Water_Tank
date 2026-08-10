#include<lpc21xx.h>
#include"header.h"
#include<stdio.h>

void can_send_time_date(u8 hour, u8 min, u8 sec,u8 ampm, u8 date, u8 month, u8 year)
{
    CAN1 msg;

	  msg.id = 0x302;//id:0x302 is sending time and date frames
    msg.rtr = 0;
    msg.dlc = 8;

    msg.byteA = 0;
    msg.byteB = 0;

    msg.byteA |= (u32)hour;           
    msg.byteA |= (u32)min   << 8;
    msg.byteA |= (u32)sec   << 16;
    msg.byteA |= (u32)ampm  << 24;

    msg.byteB |= (u32)date;
    msg.byteB |= (u32)month << 8;
    msg.byteB |= (u32)year  << 16;

    can1_tx(msg); // frame sent in this formet "year|month|date|am or pm|sec|min|hour"
	                // eg:26 | 08 | 09 | 0 or 1 | 15 | 46 | 10
}

void can_send_tank_data(unsigned short int water_level,u8 distance,u8 temperature,u8 pump_status)
{
    CAN1 msg;

    msg.id = 0x301;// id 0x301 is sending water tank data frames
    msg.rtr = 0;
    msg.dlc = 8;

    msg.byteA = 0;
    msg.byteB = 0;

    
    msg.byteA|= (u32)(water_level & 0xFF);
    msg.byteA|= (u32)((water_level >> 8) & 0xFF) << 8;

    
    msg.byteA |= (u32)distance << 16;
    msg.byteA |= (u32)temperature << 24;

    msg.byteB |= (u32)pump_status;

    can1_tx(msg);// NA | NA | NA | 0 or 1 | 32 | 40 | 800 (IN 2 BYTES)
}


void eeprom_read(void){
    u8 sec,min,hour,ampm,date,month,year;
    sec   = i2c_read(0xA0, 0x00);
    min   = i2c_read(0xA0, 0x01);
    hour  = i2c_read(0xA0, 0x02);
    ampm  = i2c_read(0xA0, 0x03);
    date  = i2c_read(0xA0, 0x04);
    month = i2c_read(0xA0, 0x05);
    year  = i2c_read(0xA0, 0x06);

//note : while reading water level
    unsigned short int water_level;

    water_level  = i2c_read(0xA0, 0x07);
    water_level |= (unsigned short int)i2c_read(0xA0, 0x08) << 8;

    u8 distance;
    distance = i2c_read(0xA0, 0x09);

    u8 temperature;
    temperature = i2c_read(0xA0, 0x0A);
	  
		u8 pump_status;
		pump_status = i2c_read(0xA0,0x0B);
		
		can_send_time_date( hour, min, sec,ampm,date,month,year);
		delay_can_ms(2000);
		can_send_tank_data(water_level,distance,temperature,pump_status);
    delay_can_ms(2000);

    char can_data[100];
    sprintf(can_data,"TIME %02d:%02d:%02d %cM DATE %02d/%02d/20%02d Water Level %d Distance %d Temperature %d C Pump status %d",hour,min,sec,ampm,date,month,year,water_level,distance,temperature,pump_status);
		uart0_tx_string("\r\nDATA Transmited throught CAN\r\n");
		uart0_tx_string(can_data);
		uart0_tx_string("\r\n");
}

