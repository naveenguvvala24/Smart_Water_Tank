
# Smart Automatic Water Tank Management and IoT Monitoring System

An embedded smart water-tank monitoring and control system built around
the **NXP LPC2129 ARM7TDMI-S** microcontroller.

The system monitors water level, ultrasonic distance, temperature, RTC
time/date, and pump status. It provides local LCD/UART monitoring,
EEPROM data logging, Wi-Fi/ThingSpeak cloud updates through an ESP-01,
and CAN communication using two CAN data frames.

## Features

-   LPC2129 ARM7-based control
-   Analog water-level monitoring
-   Ultrasonic distance measurement
-   Temperature measurement through ADC
-   RTC-based time/date management
-   EEPROM storage of operating data
-   Automatic pump ON/OFF control based on water level
-   Manual pump control using external interrupt + switches
-   16x2 LCD display
-   UART0 debug/status output
-   ESP-01 Wi-Fi communication at 115200 baud
-   ThingSpeak HTTP update
-   CAN communication using two application frames
-   Startup data reporting and periodic data update
-   Stored sensor values can be reconstructed from EEPROM

## System Architecture

<img width="1024" height="951" alt="image" src="https://github.com/user-attachments/assets/ecf7cc67-7b3c-4be7-b96a-f43d8af26475" />


## Main Controller

The main application initializes UART0, ESP-01 UART1, ultrasonic sensor,
I2C, LCD, ADC, and the external interrupt configuration before entering
the main control loop.

The current source initializes these modules in `main()` and then
performs manual-mode handling followed by automatic monitoring and data
reporting.

## Water-Level Control

The water-level ADC is read using the ADC channel used by
`smart_tank_adc_water_level()`.

Current control thresholds in the supplied source are:

-   ADC value `< 200`: tank treated as low
-   ADC value `>= 800`: tank treated as full
-   Other values: normal/intermediate range

When the tank is low, the application turns the pump/relay ON. When the
tank is full, it turns the pump/relay OFF.

The relay function also stores pump status in EEPROM:

``` text
EEPROM address 0x0B
1 = ON
0 = OFF
```

## Sensors

### Water Level

The water-level sensor is connected to ADC channel 0.

The current source converts the ADC result to a percentage using:

``` text
((ADC - 120) * 100) / (900 - 120)
```

The threshold/control logic is based on the raw ADC result.

### Ultrasonic Sensor

The ultrasonic driver uses:

``` text
TRIG = P0.12
ECHO = P0.13
```

The trigger is generated for approximately 10 us. The echo pulse is
measured using a software counter and converted using:

``` text
distance_cm = count / 58
```

The current project source also contains a temporary/test value of `10`
in several places where the measured ultrasonic value is intended to be
used. Replace these test values with the actual ultrasonic function
result when using the sensor in the final build.

### Temperature

Temperature is measured using ADC channel 2.

The source calculates:

``` text
Vout = ADC * 3.3 / 1023
Temperature = (Vout - 0.5) / 0.01
```

The result is converted to an integer.

## RTC and EEPROM

The RTC is accessed over I2C. The project reads time and date and stores
selected values in EEPROM.

EEPROM data map used by the current source:
``` text
  EEPROM Address   Data
  ---------------- -----------------------
  `0x00`           Seconds
  `0x01`           Minutes
  `0x02`           Hour
  `0x03`           AM/PM
  `0x04`           Date
  `0x05`           Month
  `0x06`           Year
  `0x07`           Water level low byte
  `0x08`           Water level high byte
  `0x09`           Ultrasonic distance
  `0x0A`           Temperature
  `0x0B`           Pump status
```
Water level is stored as a 16-bit value using addresses `0x07` and
`0x08`.

## Manual Mode

The main program uses:

``` text
P0.14 -> SW1
P0.15 -> SW2
```

An external interrupt sets the global `flag`, causing the application to
enter manual mode.

In manual mode:

-   SW1 commands pump ON
-   SW2 commands pump OFF and exits manual mode

The exact active-high/active-low behavior depends on the switch wiring
and pull-up/pull-down arrangement.

## ESP-01 and ThingSpeak

The ESP-01 communicates with the LPC2129 through UART1.

Current UART1 configuration:

``` text
TXD1 = P0.8
RXD1 = P0.9
Baud rate = 115200
```

The ESP-01 is controlled using AT commands.

The firmware:

1.  Sends `AT`
2.  Disables command echo using `ATE0`
3.  Selects station mode using `AT+CWMODE=1`
4.  Connects to the configured Wi-Fi network
5.  Opens a TCP connection to `api.thingspeak.com` on port 80
6.  Builds an HTTP GET request
7.  Sends the request using `AT+CIPSEND`
8.  Closes the TCP connection

The application reads sensor values from EEPROM before building the
ThingSpeak request.

## CAN Communication

The project is designed to send two CAN frames directly from the EEPROM
values rather than converting the sensor data to an ASCII string.

### Frame 1: Time and Date

Current implementation uses CAN ID:

``` text
0x302
```

Payload:
```text
  Byte   Value
  ------ ----------
  0      Hour
  1      Minute
  2      Second
  3      AM/PM
  4      Date
  5      Month
  6      Year
  7      Reserved
```
### Frame 2: Tank Data

Current implementation uses CAN ID:

``` text
0x301
```

Payload:
``` text
  Byte   Value
  ------ -----------------------
  0      Water level low byte
  1      Water level high byte
  2      Distance
  3      Temperature
  4      Pump status
  5      Reserved
  6      Reserved
  7      Reserved
```
This binary format is more efficient than sending a long text string
over CAN.

## Project Source Files
```text
  -----------------------------------------------------------------------
  File                                Purpose
  ----------------------------------- -----------------------------------
  `main.c`                            Main application flow and control
                                      loop

  `smart_tank_adc.c`                  Water-level, ultrasonic wrapper,
                                      temperature and pump control

  `smart_tank_eeprom_data_read.c`     EEPROM data retrieval and CAN frame
                                      preparation

  `smart_tank_esp-01.c`               ESP-01 AT-command handling and
                                      ThingSpeak communication

  `smart_tank_rtc.c`                  RTC reading, display and EEPROM
                                      time/date storage

  `smart_tank_ultrasonic_driver.c`    Ultrasonic trigger/echo measurement

  `uart1_esp-01_driver.c`             LPC2129 UART1 driver for ESP-01

  `header.h`                          Project declarations, types and
                                      configuration; maintain this in the
                                      project
  -----------------------------------------------------------------------
```
## Software Flow

<img width="1024" height="559" alt="image" src="https://github.com/user-attachments/assets/1bb51d31-14a5-4644-9920-f48c7263f576" />


## Hardware Interfaces
``` text
  Peripheral           LPC2129 interface
  -------------------- ----------------------------------------------------
  Water-level sensor   ADC channel 0
  Temperature sensor   ADC channel 2
  Ultrasonic TRIG      P0.12
  Ultrasonic ECHO      P0.13
  Manual SW1           P0.14
  Manual SW2           P0.15
  ESP-01 TX/RX         UART1 P0.8/P0.9
  LCD                  Project `header.h` driver configuration
  EEPROM/RTC           I2C
  Pump/Relay control   P0.10 and P0.21
  CAN1                 LPC2129 CAN1 peripheral + external CAN transceiver
```
## Build Environment

The source is written in Embedded C for the LPC2129 and uses:

-   Keil/ARM7-compatible LPC21xx toolchain
-   LPC2129 device header
-   Project-specific `header.h`
-   UART, I2C, LCD, ADC, CAN and peripheral drivers

## 👥 Project Team
- [Manu Kiran S](https://github.com/manukiran-S)
- [Naveen](https://github.com/naveenguvvala24)
- [Prajwal H Chalawadi](https://github.com/prajwalhchalawadi)
- [Anshu Patel](https://github.com/Anshu8849)
- [Ganesh](https://github.com/Kanalaganesh)

