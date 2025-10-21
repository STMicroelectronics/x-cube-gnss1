/**
  ******************************************************************************
  * @file    gnss_utils.c
  * @author  SRA Application Team
  * @brief   This file contains application specific utilities
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>

#include "gnss_utils.h"
#include "teseo_liv3f_conf.h"

/* Defines -------------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void printHelp(void)
{
  PRINT_INFO("\n\r 1) GETPOS:\r\n\tGets and decode the first"
             " useful $GPGGA NMEA string with the global position information.\n");
  PRINT_INFO("\n\r 2) LASTPOS:\r\n\tPrints the last saved position from the GPS reception process.\n");
  PRINT_INFO("\n\r 3) WAKESTATUS:\r\n\tGets the activity state"
             " of the GPS data reception.\r\n\tWill be printed one of the two states: 0 / 1.\n");
  PRINT_INFO("\n\r 4) HELP:\r\n\tPrints command menu.\n");
  PRINT_INFO("\n\r 5) DEBUG:\r\n\tChanges the debug state of the application (default is ON).\r\n\t"
             " If debug is ON, when the getpos command is sent, the just decoded position will be printed.\n");
  PRINT_INFO("\n\r 6) TRACK:\r\n\tBegins the tracking position process.\r\n\t"
             " You have to choose the number of positions that you want to track and "
             "the delay between two\r\n\treceptions.\n");
  PRINT_INFO("\n\r 7) LASTTRACK:\r\n\tIf last tracking process went good, "
             "prints last tracked positions on the console.\n");
  PRINT_INFO("\n\r 8) GETFWVER:\r\n\tSends the ST proprietary $PSTMGETSWVER NMEA"
             " command (to be written on serial terminal) and "
             "decode the answer with all info about the FW version.\n");
  PRINT_INFO("\n\r 9) GETGNSMSG:\r\n\tGets and decode the first useful NMEA string"
             " (the $--GNS one) with fix data for single or combined satellite navigation system information.\n");
  PRINT_INFO("\n\r10) GETGPGST:\r\n\tGets and decode the first useful $GPGST"
             " NMEA string with the GPS Pseudorange Noise Statistics.\n");
  PRINT_INFO("\n\r11) GETGPRMC:\r\n\tGets and decode the first useful $GPRMC"
             " NMEA string with the Recommended Minimum Specific GPS/Transit data.\n");
  PRINT_INFO("\n\r12) GETGSAMSG:\r\n\tGets and decode the first useful NMEA string"
             " (the $--GSA one) with GNSS DOP and active satellites information.\n");
  PRINT_INFO("\n\r13) GETGSVMSG:\r\n\tGets and decode the first useful NMEA string"
             " (the $--GSV one) with GNSS Satellites in View information.\n\n\r");
#if (CONFIG_USE_FEATURE == 1)
  PRINT_INFO("\n\r14) EN-FEATURE:\r\n\tSends a proprietary ST NMEA command to "
             "enable/disable feature and returns the answer.\n");
#endif /* CONFIG_USE_FEATURE */

#if (CONFIG_USE_GEOFENCE == 1)
  PRINT_INFO("\n\r15) CONF-GEOFENCE:\r\n\tSends a proprietary ST NMEA command to "
             "config a geofence circle and returns the answer.\n");
  PRINT_INFO("\n\r16) REQ-GEOFENCE:\r\n\tSends a proprietary ST NMEA command to "
             "request geofence status and returns the answer.\n");
#endif /* CONFIG_USE_GEOFENCE */

#if (CONFIG_USE_ODOMETER == 1)
  PRINT_INFO("\n\r17) ODOMETER-OP:\r\n\tSends a proprietary ST NMEA command to "
             "start or stop odometer and returns the answer.\n");
#endif /* CONFIG_USE_ODOMETER */

#if (CONFIG_USE_DATALOG == 1)
  PRINT_INFO("\n\r18) DATALOG-OP:\r\n\tSends a proprietary ST NMEA command to "
             "config/start/stop/erase datalog and returns the answer.\n");
#endif /* CONFIG_USE_DATALOG */

  PRINT_INFO("\n\r19) EXT-HELP:\r\n\tPrints this extended help.\n");
  PRINT_INFO("\n\r20) SET-CDB REGISTER:\r\n\tApi to set the CDB Register.\n");
  PRINT_INFO("\n\r21) GET-CDB REGISTER:\r\n\tApi to get the CDB Register.\n");
  PRINT_INFO("\n\r22) SAVE-CDB REGISTER:\r\n\tApi to save the CDB Register.\n");
  PRINT_INFO("\n\r23) SYSTEM RESET:\r\n\tApi for the system reset.\n");
  PRINT_INFO("\n\r24) CDB FACTORY RESET:\r\n\tApi to restore factory setting parameters.\n");
  PRINT_INFO("\n\r> ");
  /* PRINT_INFO("\n\r 0) ENDAPP:\r\n\tEnds the application.\r\n\tTo restart it, "
                "press CTRL+B command on the console or restart the Nucleo.\n\n\r"); */
}

void showCmds(void)
{
  PRINT_INFO("Select a command:\n\r");
  PRINT_INFO(" 0 - agnss\n\r");
  PRINT_INFO(" 1 - getpos\n\r");
  PRINT_INFO(" 2 - lastpos\n\r");
  PRINT_INFO(" 3 - wakestatus\n\r");
  PRINT_INFO(" 4 - help\n\r");
  PRINT_INFO(" 5 - debug\n\r");
  PRINT_INFO(" 6 - track\n\r");
  PRINT_INFO(" 7 - lasttrack\n\r");
  PRINT_INFO(" 8 - getfwver\n\r");
  PRINT_INFO(" 9 - getgnsmsg\n\r");
  PRINT_INFO("10 - getgpgst\n\r");
  PRINT_INFO("11 - getgprmc\n\r");
  PRINT_INFO("12 - getgsamsg\n\r");
  PRINT_INFO("13 - getgsvmsg\n\r");
#if (CONFIG_USE_FEATURE == 1)
  PRINT_INFO("14 - en-feature\n\r");
#endif /* CONFIG_USE_FEATURE */

#if (CONFIG_USE_GEOFENCE == 1)
  PRINT_INFO("15 - conf-geofence\n\r");
  PRINT_INFO("16 - req-geofence\n\r");
#endif /* CONFIG_USE_GEOFENCE == 1 */

#if (CONFIG_USE_ODOMETER == 1)
  PRINT_INFO("17 - odometer-op\n\r");
#endif /* CONFIG_USE_ODOMETER */

#if (CONFIG_USE_DATALOG == 1)
  PRINT_INFO("18 - datalog-op\n\r");
#endif /* CONFIG_USE_DATALOG */

  PRINT_INFO("19 - ext-help\n\r");
  PRINT_INFO("20 - set-cdb\n\r");
  PRINT_INFO("21 - get-cdb\n\r");
  PRINT_INFO("22 - save-cdb\n\r");
  PRINT_INFO("23 - reset\n\r");
  PRINT_INFO("24 - cdb-factoryreset\n\r");
  PRINT_INFO("\n\rSave configuration (y/n)? ");
  PRINT_INFO("\n\r> ");
}

void showPrompt(void)
{
  PRINT_INFO("> ");
}
