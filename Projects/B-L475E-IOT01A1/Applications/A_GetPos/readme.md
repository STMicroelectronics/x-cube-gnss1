
## <b>A_GetPos Application Description</b>

Application showing how to enable the Assisted-GNSS feature supported by the Teseo-LIV3F device.

Example Description:

This application shows how to enable Assisted-GNSS feature (both Predictive and Real-Time 
type) supported by the Teseo-LIV3F device.
To this aim, the application needs to access the Internet and is customized for B-L475E-IOT01A1 
providing the WiFi connectivity. For more details about Assisted GNSS, please refer to AN5160: 
RxNetworks Assisted GNSS Server Interface Specification and UM2399: ST Teseo III binary image - 
User manual available at st.com.
In addition, this application shows how real time GNSS data received by the GNSS Teseo-LIV3F 
device can be displayed through a serial connection and a serial terminal on a PC. 
The cJSON middleware support is included to parse the messages coming from the Assisted-GNSS network 
provider.
The A_GetPos application is built on top of the FreeRTOS support introducing a task (consumer) to parse 
the messages (enqueued in a shared queue) coming from the Teseo-LIV3F device; and a task (listener) to 
parse commands coming from the serial terminal.
Furthermore this sample application shows three advanced features supported by the Teseo-LIV3F device:

 - Geofencing

 - Odometer

 - Data Logging
The development of the application includes contribution by APG Division.
The Teseo-LIV3F device sends via a UART (or I2C) interface the received GNSS data to the STM32 
microcontroller, hosted on the B-L475E-IOT01A1 board, according to the NMEA 0183 Version 4.0 protocol.
The user can decide at compile time the interface to receive data (setting the macro USE_I2C in file
gnss1a1_conf.h).
The user can enable a specific feature at compile time (setting the corresponding macro in file gnss_app_cfg.h).

This A_GetPos sample application is able to:

 - establish a serial connection between the B-L475E-IOT01A1 and X-NUCLEO-GNSS1 boards and the PC

 - enable the Assisted-GNSS feature (both Predictive and Real-Time).

 - allow the user to select among different options for getting in a human readable format information related 
   to the acquired GNSS position, the satellites in view, the active satellites, and more.

 - NOTE: The A_GetPos Application for B-L475E-IOT01A1 board includes a RTC initialization mechanism which 
   relies on the usage of an external web server. The availability of this server is not guaranteed over time. 
   That example server name #defined in TIME_SOURCE_HTTP_POST shall therfore not be used in production.   
	
This A_GetPos sample application allows also the user to run commands enabling three advanced features:

 - Geofencing - allows the Teseo-LIV3F receiver to raise a NMEA message when the resolved GNSS position is 
   close to or entering or exiting from a specific circle

 - Odometer - provides information on the traveled distance using only the resolved GNSS position

 - Data Logging - allows the Teseo-LIV3F receiver to save locally on the flash the resolved GNSS position 
   to be retrieved on demand from the Host

After connecting the B-L475E-IOT01A1 board and the X-NUCLEO-GNSS1A1 expansion board and the multi antenna 
to the connector on the X-NUCLEO-GNSS1A1 expansion board, connect the B-L475E-IOT01A1 board to your PC.
Drag and drop A_GetPos*.bin (in Binary folder) on B-L475E-IOT01A1 drive.

Run a Serial Terminal (e.g. TeraTerm) on your PC and open a serial connection using the 
following parameters:

 - baud rate: 115200

 - data: 8 bit

 - parity: none

 - stop: 1bit

 - flow control: none

 - transmit delay: 5msec/char, 5msec/line

Reset the B-L475E-IOT01A1 board and select an option from the main menu appearing on Serial Terminal.

Preprocessor settings:
  Add in Compiler settings "USE_HAL_I2C_REGISTER_CALLBACKS=1" for I2C Bus ("USE_HAL_UART_REGISTER_CALLBACKS=1" for UART)
  
### <b>Keywords</b>

GNSS, I2C, UART, WIFI, RTOS, Geofencing, Odometer, Data Logging 

### <b>Directory contents</b>

 - a_gnss.c                High level API for Assisted GNSS
 
 - cloud.c                 Manage the connection to remote server
 
 - entropy_hardware_poll.c Entropy collector
 
 - es_wifi_io.c            IO operations to deal with the es-wifi module
 
 - flash_l4.c              Management of the STM32L4 internal flash memory
 
 - freertos.c              Code for freertos applications
 
 - gnss_lib_config.c       Configure how the libGNSS accesses the GNSS module
 
 - gnss_utils.c            Application specific utilities for console menu
 
 - heap.c                  Heap check functions
 
 - httpclient.c            HTTP client library test
 
 - http_lib.c              HTTP client library
 
 - iot_flash_config.c      Flash memory configuration 
 
 - main.c                  Main program body
 
 - mbedtls_net.c           Mbedtls network porting layer (callback implementation)
 
 - net.c                   Network abstraction at transport layer level
 
 - net_tcp_wifi.c          TCP implementation on ST WiFi connectivity API
 
 - net_tls_mbedtls.c       mbedTLS implementation
 
 - STM32CubeRTCInterface.c Std. time port to the RTC
 
 - stm32_bus_ex.c          Source file for GNSS1A1 Bus Extension
 
 - stm32**xx_hal_msp.c     Source code for MSP Initialization and de-Initialization

 - stm32**xx_it.c          Source code for interrupt Service Routines

 - stm32**xx_nucleo.c      Source file for the BSP Common driver 
						
 - stm32**xx_nucleo_bus.c  Source file for the BSP BUS IO driver
 
 - system_stm32**xx.c      CMSIS Cortex-Mx Device Peripheral Access Layer System Source File

 - timedate.c              RTC configuration

 - timingSystem.c          Wrapper to STM32 timing

 - wifi.c                  WiFi interface file
 
 - wifi_net.c              WiFi-specific NET initialization

### <b>Hardware and Software environment</b>

  - This example runs on B-L475E-IOT01A1 equipped board with a X-NUCLEO-GNSS1A1 (or X-NUCLEO-GNSS2A1) expansion board

  - To use UART channel, modify the Jumper configuration on X-NUCLEO-GNSS1A1 as follows:
    - J2 closed
    - J3 open
    - J4 open
    - J5 closed

ADDITIONAL_BOARD : X-NUCLEO-GNSS1A1 https://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-gnss1a1.html
ADDITIONAL_COMP : Teseo-LIV3F https://www.st.com/content/st_com/en/products/positioning/gnss-modules/teseo-liv3f.html

### <b>How to use it?</b>

In order to make the program work, you must do the following:

 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.

 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.9.0).
   Alternatively you can use the Keil uVision toolchain (this firmware
   has been successfully tested with V5.32.0) or the IAR toolchain (this firmware has 
   been successfully tested with Embedded Workbench V9.20.1).

 - The Preprocessor symbol ASSISTED_GNSS should be included in project option.

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
