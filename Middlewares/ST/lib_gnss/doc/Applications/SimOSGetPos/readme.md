
## <b>SimOSGetPos Application Description</b>

Application showing, through a serial connection and a serial terminal on a PC, 
real time GNSS data received by the GNSS Teseo-LIV3F device.

Example Description:

Application showing how real time GNSS data ($GPGGA) received by the Teseo-LIV3F device 
can be displayed, through a serial connection and a serial terminal, on a PC.

The Teseo-LIV3F device sends via an I2C (or UART) interface the received GNSS data to the STM32 
microcontroller, hosted on the Nucleo board, according to the NMEA 0183 Version 4.0 protocol.
The I2C (or UART) channel can be set using the macro defined in gnss1a1_conf.h

This SimOSGetPos sample application is able to:

 - establish a serial connection between the STM32 Nucleo and X-NUCLEO-GNSS1 boards and the PC

 - parse periodic $GPGGA or $GNGGA sentences.
	
After connecting the STM32 Nucleo board and the X-NUCLEO-GNSS1A1 expansion board and the 
GPS/GLONASS antenna to the connector on the X-NUCLEO-GNSS1A1 expansion board, connect the 
STM32 Nucleo board to your PC.
Drag and drop *.bin (in Binary folder) on Nucleo drive.

Run a Serial Terminal (e.g. TeraTerm) on your PC and open a serial connection using the 
following parameters:

 - baud rate: 115200

 - data: 8 bit

 - parity: none

 - stop: 1bit

 - flow control: none

Reset the STM32 Nucleo board and select an option from the menu appearing on Serial Terminal.

This application can run also on a STM32 Nucleo board equipped with a X-NUCLEO-GNSS2A1 expansion board (mounting the Teseo-VIC3DA device).
If the UART is used as communication bus between the STM32 MCU and the Teseo-VIC3DA the baud rate must be set to 115200 (it is 9600 between the STM32 MCU and the Teseo-LIV3F).

### <b>Keywords</b>

GNSS, I2C, UART

### <b>Directory contents</b>

 - app_gnss.c             SimOSGetPos initialization and applicative code
 
 - gnss_lib_config.c      Configure how the libGNSS accesses the GNSS module
 
 - main.c                 Main program body
 
 - stm32_bus_ex.c         Source file for GNSS1A1 Bus Extension
 
 - stm32**xx_hal_msp.c    Source code for MSP Initialization and de-Initialization

 - stm32**xx_it.c         Source code for interrupt Service Routines

 - stm32**xx_nucleo.c     Source file for the BSP Common driver 
						
 - stm32**xx_nucleo_bus.c Source file for the BSP BUS IO driver
 
 - system_stm32**xx.c     CMSIS Cortex-Mx Device Peripheral Access Layer System Source File
  
### <b>Hardware and Software environment</b>

  - This example runs on STM32 Nucleo devices equipped with a X-NUCLEO-GNSS1A1 (or a X-NUCLEO-GNSS2A1 or X-NUCLEO-LIV4A1) expansion board
  - This example has been tested with STMicroelectronics:
    - NUCLEO-L073RZ board
    - NUCLEO-F401RE board
    - NUCLEO-L476RG board
    - NUCLEO-U575ZI-Q board
    - NUCLEO-H563ZI board
    and can be easily tailored to any other supported device and development board.

ADDITIONAL_BOARD : X-NUCLEO-GNSS1A1 https://www.st.com/en/ecosystems/x-nucleo-gnss1a1.html
ADDITIONAL_COMP : Teseo-LIV3F https://www.st.com/en/positioning/teseo-liv3f.html
ADDITIONAL_BOARD : X-NUCLEO-GNSS2A1 https://www.st.com/en/ecosystems/x-nucleo-gnss2a1.html
ADDITIONAL_COMP : Teseo-VIC3DA https://www.st.com/en/positioning/teseo-vic3da.html
ADDITIONAL_BOARD : X-NUCLEO-LIV4A1 https://www.st.com/en/ecosystems/x-nucleo-liv4a1.html
ADDITIONAL_COMP : Teseo-LIV4F https://www.st.com/en/positioning/teseo-liv4f.html


- Modify the Jumper configuration for : NUCLEO-U575ZI-Q and NUCLEO-H563ZI board

  | Teseo Module            | Jumper Setting | Jumpers                                 |
  |-------------------------|----------------|------------------------------------------|
  | **X-NUCLEO-LIV4A1**     | Open           | J14, J7, J5                             |
  |                         | Closed         | J1, J2, J4, J6, J9                      |
  |                         | 1-2            | J12, J10, J11                           |
  |                         | 2-3            | J8, J3                                  |
  | **X-NUCLEO-GNSS1A1**    | Closed         | J2, J13, J5, J9, J12, J11, J14, J15     |
  |                         | Open           | J3, J4, J6, J7, J8, J10                 |
  | **X-NUCLEO-GNSS2A1**    | Closed         | J11, J12, J14, J15                      |
  |                         | 1-2            | J29, J28, J23, J25                      |
  |                         | 2-3            | J27, J24, J26, J30                      |

- Modify the Jumper configuration for : NUCLEO-L073RZ, NUCLEO-F401RE and NUCLEO-L476RG baord

  | Teseo Module           | Jumper Setting | Jumpers                                 |
  |------------------------|----------------|------------------------------------------|
  | X-NUCLEO-LIV4A1        | Open           | J14, J7, J5                             |
  |                        | Closed         | J1, J2, J4, J6, J9                      |
  |                        | 1-2            | J8, J12, J10                            |
  |                        | 2-3            | J11, J3                                 |
  | X-NUCLEO-GNSS1A1       | Closed         | J3, J4, J6, J9, J11, J12, J13, J14, J15 |
  |                        | Open           | J2, J5, J7, J8, J10                     |
  | X-NUCLEO-GNSS2A1       | Closed         | J11, J12, J14, J15                      |
  |                        | 1-2            | J23, J25, J27, J29                      |
  |                        | 2-3            | J24, J26, J28, J30                      |
  
### <b>How to use it?</b>

In order to make the program work, you must do the following:

 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.
   
 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.19.0).
   Alternatively you can use the Keil uVision toolchain (this firmware
   has been successfully tested with V5.38.0) or the IAR toolchain (this firmware has 
   been successfully tested with Embedded Workbench V9.60.3).
   
 - Rebuild all files and load your image into target memory.
 
 - Run the example.
 
 - Alternatively, you can download the pre-built binaries in "Binary" 
   folder included in the distributed package.

### <b>Author</b>

SRA Application Team

### <b>License</b>

Copyright (c) 2025 STMicroelectronics.
All rights reserved.

This software is licensed under terms that can be found in the LICENSE file
in the root directory of this software component.
If no LICENSE file comes with this software, it is provided AS-IS.
