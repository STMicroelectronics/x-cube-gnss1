/**
  @page Firmware Updater Tool for STM32 X-NUCLEO-GNSS1A1 Expansion Board
  
  @verbatim
  ******************************************************************************
  * @file    readme.txt  
  * @author  SRA Application Team
  * @brief   This application allows the user to update the
  *          firmware of the X-NUCLEO-GNSS1A1 expansion board.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

@par Tool Description 

    - FWUPG is a Java based graphical tool allowing the user to update the 
      firmware of the X-NUCLEO-GNSS1A1 expansion board connected with an STM32 
      Nucleo board.
    - FWUPG is platform independent and supports Windows, Mac OS X and Linux
    - FWUPG uses the following libraries:
      - JARINJARLOADER
        License: Eclipse Public License 1.0 (https://opensource.org/licenses/EPL-1.0)
        URL: https://git.eclipse.org/c/jdt/eclipse.jdt.ui.git/plain/org.eclipse.jdt.ui/jar%20in%20jar%20loader/	
	  - jSerialComm, a platform-independent serial port access library for Java
	    License: Apache License, Version 2.0 (https://opensource.org/licenses/Apache-2.0)
		URL: https://github.com/Fazecast/jSerialComm
    
@par Version 3.1.0

    - FWUPG allows the user to upgrade the X-NUCLEO-GNSS1A1 firmware to version 4.6.8.2
	
@par Hardware and Software environment

    - FWUPG requires JRE (Java Runtime) 6+
    - FWUPG requires a X-NUCLEO-GNSS1A1 expansion board connected with an STM32 NUCLEO-F401RE
      board which, in turn, is plugged to the PC via a USB cable
    - The FW_Updater application should run on the STM32 Nucleo board
    - The tool has been tested with NUCLEO-F401RE RevC

@par How to use it ? 

In order to make the program work, you must do the following:
    - Windows/Mac: The tool can be launched by double-clicking the FWUPG.jar
    - Linux: Due to serial port access permission, the "user" should be added to "dialout"
      group before launching the tool.
      The tool can be launched by typing the following command line:
      $ java -jar FWUPG.jar	
    - The user should select the relevant serial port and then push the "Open" button.
      - NOTE: On Windows, the active serial port is typically the one highest numbered. 
    - The user can start the upgrading procedure by pushing the "Update FW >>>" button or using
      the "Tools" menu.
    - A progress bar shall inform the user about the upgrading status.
    - WARNING:
      - The user is not allowed to select an arbitrary firmware image browsing the file system,
        since only the latest firmware image is embedded in the deployable jar.
      - The user is strongly advised not to unplug the STM32 Nucleo board during the upgrading
        procedure.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
