
## <b>Virtual_COM_Port Application Description</b>

Application for the GNSS Teseo mounted on an X-NUCLEO-GNSS1A1,X-NUCLEO-GNSS2A1 or X-NUCLEO-LIV4A1
expansion board to be used along with the Teseo-Suite tool.
 
Example Description:

Plug the X-NUCLEO-GNSS1A1 expansion board on top of a STM32 Nucleo board.
Connect the STM32 Nucleo board to your PC.
Drag and drop Virtual_COM_Port_*.bin (in Binary folder) on Nucleo drive.
Run the Teseo-Suite tool (available on st.com) on your Win PC, select a Serial Port 
and open a serial connection through the Teseo-Suite GUI using the following parameters:
 
 - Baud Rate: 115200

 - Data Bits: 8

 - Parity: None

 - Stop Bits: One

and enjoy.    
For all info on how to use the Teseo-Suite, please, refer to the related documentation
available on st.com.

When using this application along with a serial terminal (e.g. TeraTerm) 
all messages coming from the GNSS Teseo-LIV3 are printed on the screen.

This application can run also on a STM32 Nucleo board equipped with a X-NUCLEO-GNSS2A1 expansion board (mounting the Teseo-VIC3DA device).
If the UART is used as communication bus between the STM32 MCU and the Teseo-VIC3DA the baud rate must be set to 115200 (it is 9600 between the STM32 MCU and the Teseo-LIV3F).

### <b>Keywords</b>

GNSS, I2C, UART, VCOM, RTOS

### <b>Directory contents</b>

 - app_azure_rtos.c             Memory byte pool initialization (only when ThreadX is used)
 
 - app_gnss.c                   Virtual_COM_Port initialization and applicative code
 
 - app_threadx.c                Application ThreadX Initialization (only when ThreadX is used)
 
 - freertos.c                   Code for freertos applications (only when FreeRTOS is used)
 
 - gnss_lib_config.c            Configure how the libGNSS accesses the GNSS module
  
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

  - This example runs on STM32 Nucleo devices equipped with a X-NUCLEO-GNSS1A1 (or a X-NUCLEO-GNSS2A1 or X-NUCLEO-LIV4A1) expansion board
  - This example has been tested with STMicroelectronics:
    - NUCLEO-F401RE RevC board
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
