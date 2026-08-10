/*header.h*/

#define WIFI_SSID      "YOUR_WIFI_NAME"
#define WIFI_PASSWORD  "YOUR_WIFI_PASSWORD"

#define API_KEY        "YOUR_WRITE_API_KEY"

typedef unsigned int u32;
typedef signed int s32;
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short int u16;



extern void uart0_init(unsigned int);
extern void uart0_tx(unsigned char);
extern unsigned char uart0_rx(void);
extern void uart0_tx_string(char *);
//extern void uart0_binary(int);
//extern void delay_ms(unsigned int);
extern void uart0_integer(s32 num);
//void uart0_float(float num);


extern void lcd_data(unsigned char data);
extern void lcd_cmd(unsigned char cmd);
extern void lcd_string(char *ptr);
extern void lcd_init(void);
extern void lcd_integer(int num);
extern void delay_ms(unsigned int ms);

extern void extint(void)__irq;
extern void config_ext(void);


//extern void adc_init(void);
//extern u32 adc_read(u8);

//extern void config_vic_for_eint0(void);
//extern void config_vic_for_uart0(void);
//extern void config_vic_for_timer1(void);

//extern u8 spi0(u8 data);
//extern void spi0_init(void);
//extern u32 mcp3204_adc_read(u8 ch_num);


extern void i2c_init(void);
extern void i2c_write(u8 sa, u8 mr, u8 data);
extern u8 i2c_read(u8 sa,u8 mr);


extern void config_ext(void);

extern void uart0_time_div(char t);

extern void smart_tank_rtc(void);
extern u32 smart_tank_timer(void);


extern void adc_init(void);
extern unsigned int adc_read(unsigned char ch_num);
extern void smart_tank_relay(u32 a);
extern u32 smart_tank_adc_water_level(void);
extern u32 smart_tank_adc_ultrasonic(void);
extern int smart_tank_adc_temp(void);
extern u8 uart0_rx_esp01(u8 *data);

extern unsigned int ultrasonic_distance_cm(void);
extern void delay_us(unsigned int us);
extern void ultrasonic_init(void);
extern void eeprom_read(void);



extern void esp_tx(char *str);

extern u8 esp_init(void);

extern u8 wifi_connect(void);

extern u8 tcp_connect(void);

extern u8 tcp_close(void);

extern void thingspeak_send(void);

extern u8 wifi_status(void);

extern void uart1_init_esp(unsigned int baud);

extern void uart1_tx_esp(unsigned char data);

extern unsigned char uart1_rx_esp(void);

extern unsigned char uart1_rx_esp01(unsigned char *data);

extern void uart1_tx_string_esp(char *ptr);

extern void uart1_time_div_esp(unsigned char t);

extern void uart1_integer_esp(int num);

typedef struct CAN1_MSG{
	u32 id;
	u32 byteA;
	u32 byteB;
	u8 rtr;
	u8 dlc;
	u8 ff;
}CAN1;

extern void can1_tx(CAN1 v);
extern void can1_init(void);
extern void delay_can_ms(unsigned int ms);