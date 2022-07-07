
## <b>FW_Updater Application Description</b>

Application for the GNSS Teseo-LIV3F firmware updating.

Example Description:

This application must be used along with the java application FWUPG.

Plug the X-NUCLEO-GNSS1A1 expansion board on top of a STM32 Nucleo-F401RE.
Connect the STM32 Nucleo board to your PC.
Drag and drop FW_Updater-*.bin (in Binary folder) on Nucleo drive.
Run the java tool FWUPG.jar (in Utilities/PC_Software/FirmwareUpdaterTool folder).
From the java GUI, after selecting the right serial port, click Open to start a 
connection with your STM32 Nucleo and X-NUCLEO-GNSS1A1 boards.
If the FW version on the Teseo-LIV3F module is not the latest one, click the 
Update FW >>> button to start the firmware upgrading process.

### <b>Keywords</b>

GNSS, UART

### <b>Directory contents</b>

 - app_gnss.c             FW_Updater initialization and applicative code
 
 - gnss_lib_config.c      Configure how the libGNSS accesses the GNSS module
 
 - main.c                 Main program body
 
 - stm32_bus_ex.c         Source file for GNSS1A1 Bus Extension
 
 - stm32**xx_hal_msp.c    Source code for MSP Initialization and de-Initialization

 - stm32**xx_it.c         Source code for interrupt Service Routines

 - stm32**xx_nucleo.c     Source file for the BSP Common driver 
						
 - stm32**xx_nucleo_bus.c Source file for the BSP BUS IO driver
 
 - system_stm32**xx.c     CMSIS Cortex-Mx Device Peripheral Access Layer System Source File

### <b>Hardware and Software environment</b>

  - This example runs on STM32 Nucleo devices equipped with a X-NUCLEO-GNSS1A1 expansion board
  - This example has been tested with STMicroelectronics:
    - NUCLEO-F401RE RevC board
    and can be easily tailored to any other supported device and development board.

ADDITIONAL_BOARD : X-NUCLEO-GNSS1A1 https://www.st.com/en/ecosystems/x-nucleo-gnss1a1.html
ADDITIONAL_COMP : Teseo-LIV3F https://www.st.com/en/positioning/teseo-liv3f.html
  
### <b>How to use it?</b>

In order to make the program work, you must do the following:

 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.
   
 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.9.0).
   Alternatively you can use the Keil uVision toolchain (this firmware
   has been successfully tested with V5.32.0) or the IAR toolchain (this firmware has 
   been successfully tested with Embedded Workbench V9.20.1).
   
 - Rebuild all files and load your image into target memory.
 
 - Run the example.
 
 - Alternatively, you can download the pre-built binaries in "Binary" 
   folder included in the distributed package.

### <b>Author</b>

SRA Application Team

### <b>License</b>

Copyright (c) 2022 STMicroelectronics.
All rights reserved.

This software is licensed under terms that can be found in the LICENSE file
in the root directory of this software component.
If no LICENSE file comes with this software, it is provided AS-IS.
