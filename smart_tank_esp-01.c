#include <lpc21xx.h>
#include <stdio.h>
#include <string.h>
#include "header.h"


#define THINGSPEAK_API_KEY  "enter_your_API_write_key"

char http[350];
char cmd[40];
char esp_buffer[250];

void esp_clear_buffer(void)
{
    memset(esp_buffer,0,sizeof(esp_buffer));
}

u8 esp_wait_for(char *ack,unsigned int timeout)
{
    u8 ch;
    unsigned int i=0;

    esp_clear_buffer();

    while(timeout--)
    {
        if(uart1_rx_esp01(&ch))
        {
            if(i < sizeof(esp_buffer)-1)
            {
                esp_buffer[i++] = ch;
                esp_buffer[i] = '\0';
            }

            if(strstr(esp_buffer,ack))
                return 1;

            if(strstr(esp_buffer,"ERROR"))
                return 0;

            if(strstr(esp_buffer,"FAIL"))
                return 0;
        }

        delay_ms(1);
    }

    return 0;
}



void esp_tx(char *str)
{
    while(*str)
    {
        uart1_tx_esp(*str++);
    }
}

u8 esp_init(void)
{
    esp_tx("AT\r\n");

    if(!esp_wait_for("OK",3000))
        return 0;

    esp_tx("ATE0\r\n");

    if(!esp_wait_for("OK",3000))
        return 0;

    esp_tx("AT+CWMODE=1\r\n");

    if(!esp_wait_for("OK",3000))
        return 0;

    return 1;
}

u8 wifi_connect(void)
{
    sprintf(cmd,
    "AT+CWJAP=\"%s\",\"%s\"\r\n",
    WIFI_SSID,
    WIFI_PASSWORD);

    esp_tx(cmd);

    return esp_wait_for("OK",15000);
}

u8 tcp_connect(void)
{
    esp_tx("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");

    if(esp_wait_for("CONNECT",5000))
        return 1;

    if(esp_wait_for("ALREADY CONNECTED",5000))
        return 1;

    return 0;
}

u8 tcp_close(void)
{
    esp_tx("AT+CIPCLOSE\r\n");

    if(esp_wait_for("OK",3000))
        return 1;

    return 0;
}


void thingspeak_send(void)
{
    u8 sec,min,hour,ampm,date,month,year;
    u16 water_level;
    u8 distance;
    u8 temperature;
    u8 pump_status;

    unsigned long time_data;
    unsigned long date_data;

    /* Read RTC from EEPROM */

    sec   = i2c_read(0xA0,0x00);
    min   = i2c_read(0xA0,0x01);
    hour  = i2c_read(0xA0,0x02);
    ampm  = i2c_read(0xA0,0x03);

    date  = i2c_read(0xA0,0x04);
    month = i2c_read(0xA0,0x05);
    year  = i2c_read(0xA0,0x06);

    /* Water Level */

    water_level  = i2c_read(0xA0,0x07);
    water_level |= ((u16)i2c_read(0xA0,0x08) << 8);

    /* Distance */

    distance = i2c_read(0xA0,0x09);

    /* Temperature */

    temperature = i2c_read(0xA0,0x0A);

    /* Pump Status */

    pump_status = i2c_read(0xA0,0x0B);

    /* Convert Time -> HHMMSS */

    time_data = (hour * 10000UL) +
                (min  * 100UL) +
                 sec;

    /* Convert Date -> DDMMYY */

    date_data = (date  * 10000UL) +
                (month * 100UL) +
                 year;

    /************ TCP CONNECT ************/

    if(!tcp_connect())
    return;

    /************ HTTP GET ************/

    sprintf(http,

"GET /update?api_key=" THINGSPEAK_API_KEY
"&field1=%u"
"&field2=%u"
"&field3=%u"
"&field4=%lu"
"&field5=%lu"
"&field6=%u"
" HTTP/1.1\r\n"
"Host: api.thingspeak.com\r\n"
"Connection: close\r\n\r\n",

    water_level,
    distance,
    temperature,
    time_data,
    date_data,
    pump_status);

    /************ SEND LENGTH ************/

    sprintf(cmd,"AT+CIPSEND=%d\r\n",strlen(http));

    esp_tx(cmd);

    if(!esp_wait_for(">",3000))
    return;

    /************ SEND HTTP ************/

    esp_tx(http);

    if(!esp_wait_for("SEND OK",5000))
    return;

    /************ CLOSE TCP ************/

    tcp_close();
}

u8 wifi_status(void)
{
    u8 ch;
    char buffer[200];
    u16 i = 0;
    u32 timeout = 5000;

    memset(buffer,0,sizeof(buffer));

    /* Check WiFi Status */

    esp_tx("AT+CWJAP?\r\n");

    while(timeout--)
    {
        if(uart1_rx_esp01(&ch))
        {
            if(i < sizeof(buffer)-1)
            {
                buffer[i++] = ch;
                buffer[i] = '\0';
            }

            /* Connected */
            if(strstr(buffer,"+CWJAP:"))
                return 1;

            /* Not Connected */
            if(strstr(buffer,"No AP")   ||
               strstr(buffer,"ERROR")   ||
               strstr(buffer,"FAIL"))
                return 0;
        }

        delay_ms(1);
    }

    return 0;
}
