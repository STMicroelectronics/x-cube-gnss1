
## <b>GetPos Application Description</b>

Application showing, through a serial connection and a serial terminal on a PC, 
real time GNSS data received by the GNSS Teseo-LIV3F device.
  
Example Description:

This application shows how real time GNSS data received by the Teseo-LIV3F device 
can be displayed, through a serial connection and a serial terminal, on a PC.

The GetPos application is built on top of the FreeRTOS support, introducing:

 - a task (consumer) to parse the messages (enqueued in a shared queue) coming from the Teseo-LIV3F device 

 - and a task (listener) to parse commands coming from the serial terminal.

Furthermore this sample application shows three advanced features supported by 
the Teseo-LIV3F device:

 - Geofencing

 - Odometer

 - Data Logging
 
The Teseo-LIV3F device sends via an I2C (or UART) interface the received GNSS data to the STM32 
microcontroller, hosted on the Nucleo board, according to the NMEA 0183 Version 4.0 protocol.

This GetPos sample application is able to:

 - establish a serial connection between the STM32 Nucleo and X-NUCLEO-GNSS1 boards and the PC

 - allow the user to select among different options for getting in a human readable format information 
   related to the acquired GNSS position, the satellites in view, the active satellites, and more.		
	
This GetPos sample application allows also the use to run commands enabling three advanced features:

 - Geofencing - allows the Teseo-LIV3F receiver to raise a NMEA message when the resolved GNSS position is
   close to or entering or exiting from a specific circle

 - Odometer - provides information on the traveled distance using only the resolved GNSS position

 - Data Logging - allows the Teseo-LIV3F receiver to save locally on the flash the resolved GNSS position 
   to be retrieved on demand from the Host

After connecting the STM32 Nucleo board and the X-NUCLEO-GNSS1A1 expansion board and the 
GPS/GLONASS antenna to the connector on the X-NUCLEO-GNSS1A1 expansion board,
connect the STM32 Nucleo board to your PC.
Drag and drop GetPos_*.bin (in Binary folder) on Nucleo drive.

Run a Serial Terminal (e.g. TeraTerm) on your PC and open a serial connection using the 
following parameters:

 - baud rate: 115200

 - data: 8 bit

 - parity: none

 - stop: 1bit

 - flow control: none

 - transmit delay: 5msec/char, 5msec/line

Reset the STM32 Nucleo board and select an option from the main menu appearing on Serial Terminal.

This application can run also on a STM32 Nucleo board equipped with a X-NUCLEO-GNSS2A1 expansion board (mounting the Teseo-VIC3DA device).
If the UART is used as communication bus between the STM32 MCU and the Teseo-VIC3DA the baud rate must be set to 115200 (it is 9600 between the STM32 MCU and the Teseo-LIV3F).
For this application it is suggested also to set, in file teseo_liv3f_conf.h, the following #define according to the targeting device:
#define TESEO_LIV3F_DEVICE /* ST GNSS device: can be TESEO_LIV3F_DEVICE or TESEO_VIC3DA_DEVICE */

### <b>Keywords</b>

GNSS, I2C, UART, RTOS

### <b>Directory contents</b>

 - app_azure_rtos.c             Memory byte pool initialization (only when ThreadX is used)

 - app_gnss.c                   GetPos initialization and applicative code
 
 - app_threadx.c                Application ThreadX Initialization (only when ThreadX is used)
 
 - freertos.c                   Code for freertos applications (only when FreeRTOS is used)
 
 - gnss_lib_config.c            Configure how the libGNSS accesses the GNSS module
 
 - gnss_utils.c                 Application specific utilities for console menu
 
 - main.c                       Main program body
 
 - stm32_bus_ex.c               Source file for GNSS1A1 Bus Extension
 
 - stm32**xx_hal_msp.c          Source code for MSP Initialization and de-Initialization
 
 - stm32**xx_hal_timebase_tim.c HAL time base based on the hardware TIM

 - stm32**xx_it.c               Source code for interrupt Service Routines

 - stm32**xx_nucleo.c           Source file for the BSP Common driver 
						
 - stm32**xx_nucleo_bus.c       Source file for the BSP BUS IO driver
 
 - system_stm32**xx.c           CMSIS Cortex-Mx Device Peripheral Access Layer System Source File
 
 - tx_initialize_low_level.s    ThreadX low level initialization (only when ThreadX is used)
  
### <b>Hardware and Software environment</b>

  - This example runs on STM32 Nucleo devices equipped with a X-NUCLEO-GNSS1A1 (or a X-NUCLEO-GNSS2A1) expansion board
  - This example has been tested with STMicroelectronics:
    - NUCLEO-L476RG RevC board
    - NUCLEO-U575ZI-Q board
    and can be easily tailored to any other supported device and development board.

ADDITIONAL_BOARD : X-NUCLEO-GNSS1A1 https://www.st.com/en/ecosystems/x-nucleo-gnss1a1.html
ADDITIONAL_BOARD : X-NUCLEO-GNSS2A1 https://www.st.com/en/ecosystems/x-nucleo-gnss2a1.html
ADDITIONAL_BOARD : X-NUCLEO-LIV4A1 https://www.st.com/en/ecosystems/x-nucleo-liv4a1.html

ADDITIONAL_COMP : Teseo-LIV3F https://www.st.com/en/positioning/teseo-liv3f.html
ADDITIONAL_COMP : Teseo-VIC3DA https://www.st.com/en/positioning/teseo-vic3da.html
ADDITIONAL_COMP : Teseo-LIV4F https://www.st.com/en/positioning/teseo-liv4f.html


- To use UART channel, modify the Jumper configuration on X-NUCLEO-GNSS1A1 as follows:
    -	J3 close, J2 open

 - To use UART channel, modify the Jumper configuration on X-NUCLEO-GNSS2A1 as follows:
    -	J28 in position 2-3
    -	J27 in position 2-3
    
 - To use UART channel, modify the Jumper configuration on X-NUCLEO-LIV4A1 as follows:
    -	J11 in position 2-3
    -	J8 in position 2-3
  
### <b>How to use it?</b>

In order to make the program work, you must do the following:

 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.
   
 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.15.1).
   Alternatively you can use the Keil uVision toolchain (this firmware
   has been successfully tested with V5.37.0) or the IAR toolchain (this firmware has 
   been successfully tested with Embedded Workbench V9.20.1).
   
 - Rebuild all files and load your image into target memory.
 
 - Run the example.
 
 - Alternatively, you can download the pre-built binaries in "Binary" 
   folder included in the distributed package.

### <b>Keil v5.38 Changes </b>

In order to make the project build for Keil v5.38 , follow the below steps:

- Change the include path from ../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F to ../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
- Change the path of port.c include in the Middlewares/Third_Party/FreeRTOS from ..\..\..\..\..\Middlewares\Third_Party\FreeRTOS\Source\portable\RVDS\ARM_CM4F\port.c
 to ..\..\..\..\..\Middlewares\Third_Party\FreeRTOS\Source\portable\GCC\ARM_CM4F\port.c

### <b>Author</b>

SRA Application Team

### <b>License</b>

Copyright (c) 2022 STMicroelectronics.
All rights reserved.

This software is licensed under terms that can be found in the LICENSE file
in the root directory of this software component.
If no LICENSE file comes with this software, it is provided AS-IS.
