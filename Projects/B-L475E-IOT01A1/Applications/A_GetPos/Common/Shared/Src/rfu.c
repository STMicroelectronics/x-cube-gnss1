/**
  ******************************************************************************
  * @file    rfu.c
  * @author  MCD Application Team
  * @brief   Remote firmware update over TCP/IP.
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
/* Includes ------------------------------------------------------------------*/
#ifdef RFU

#include <stdio.h>        
#include <string.h>
#include <stdlib.h>
#include "main.h"         
#ifdef OLD_RFU
#define SE_FW_HEADER_TOT_LEN    1
extern void __ICFEDIT_fw_download_area_start__;
const  uint32_t *l_fw_download_area_start = &__ICFEDIT_fw_download_area_start__;
extern void __ICFEDIT_fw_download_area_size__;
const  uint32_t *l_fw_download_area_size = &__ICFEDIT_fw_download_area_size__;
#else
#include "se_def.h"
#include "se_interface.h"
#endif
#include "http_util.h"
#include "msg.h"
#include "net.h"
#include "flash.h"
#include "iot_flash_config.h"


/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/


/**
 * @brief   Download a firmware image from an HTTP server into the alternate Flash bank.
 * @note    The current program, as well as the update, must fit in a single bank of the embedded FLASH:
 *          [0x08000000 - 0x08080000] on STM32L745.
 * @note    The HTTP server must support the "Range:" request header. This is the case with HTTP/1.1.
 * @param   In: url    Location of the new firmware (HTTP url: "http://<hostname>:<port>/<path>")
 * @retval  Error code
 *             RFU_OK (0) Success.
 *             <0         Failure.
 *                          RFU_ERR_HTTP  Error downloading over HTTP.
 *                          RFU_ERR_FF    Error decoding the simple code file format.
 *                          RFU_ERR_FLASH Error erasing or programming the Flash memory.
 */

#define ALIGN8(a)      ((a+7)/8)*8
#define ALIGN64(a)      ((a+63)/64)*64

int rfu_update(const char * const url)
{
  int rc = RFU_OK;  
  uint8_t  readbuffer[FLASH_PAGE_SIZE];
  uint8_t  fw_header_input[SE_FW_HEADER_TOT_LEN];
  http_sock_handle_t sockHnd;
  int file_size;
  int request_size;
  int read_size;
  int received_size;
  uint32_t areaAddr;
  uint32_t area_size;
#ifndef OLD_RFU
  SE_StatusTypeDef e_se_status;
  SE_FwImageFlashTypeDef area;
#endif
  
  rc = http_sock_open(&sockHnd, url);
  if (RFU_OK != rc) {
    msg_error("Could not open %s\n", url);
    return RFU_ERR_HTTP;
  }
  
  /* Read the file header and the first record header */
  read_size = http_sock_rcv(fw_header_input, SE_FW_HEADER_TOT_LEN, sockHnd);
  if ( SE_FW_HEADER_TOT_LEN != read_size )  
  {
    msg_error("can not read header part of file %s(expected %d bytes , get %d)\n", url,SE_FW_HEADER_TOT_LEN,read_size);
	http_sock_close(sockHnd);
    return RFU_ERR_HTTP;
  }
  
 file_size = http_sock_get_resource_size(sockHnd); 
 
#ifndef RFU_DRY_RUN
  printf("calling SE to getsecure area size\n");
  
#ifdef OLD_RFU
  areaAddr  = (uint32_t) l_fw_download_area_start;
  area_size = (uint32_t) l_fw_download_area_size;
#else
  if (SE_SFU_IMG_Download_Area(&e_se_status, &area) == SE_ERROR )
  {
	msg_error("can not reach se_sfu module information\n");
	http_sock_close(sockHnd);
    return RFU_ERR_HTTP;
  }

  areaAddr = area.DownloadAddr;
  area_size = area.MaxSizeInBytes;
  printf("calling SE to getsecure area size DowloadAddr %x  size %d\n",areaAddr,area_size);
#endif


  /* Clear download area*/
  printf("  -- -- Erasing download area ...\r\n\n"); 
  rc = (HAL_StatusTypeDef) FLASH_Erase_Size(areaAddr, area_size / 2U); /* erase first half of slot 1 */
  if (rc ) 
  {
    printf("ERROR: Unable to erase flash area at [%x - %x [\n", areaAddr, area_size / 2U);
  }
  /* Refresh Watchdog */
  WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);

  rc = (HAL_StatusTypeDef) FLASH_Erase_Size(areaAddr + area_size / 2U,
                                                  area_size / 2U); /* erase second half of slot 1 */
  if (rc ) 
  {
    printf("ERROR: Unable to erase flash area at [%x - %x [\n", areaAddr+ area_size / 2U,areaAddr+area_size);
  }

  /* Refresh Watchdog */
  WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);
  
#endif

 // flashing header
#ifndef OLD_RFU
#ifndef RFU_DRY_RUN
      rc = FLASH_Write(areaAddr, (uint32_t *) fw_header_input, read_size);
      if (rc ) 
      {
        printf("ERROR:Unable to write flash area at [%x - %x [\n", areaAddr,read_size);
      }
      areaAddr += area.ImageOffsetInBytes;
#endif
#endif
  
  
  received_size = read_size ;
  do
  {

     request_size = file_size - received_size;
     if (request_size > FLASH_PAGE_SIZE) request_size = FLASH_PAGE_SIZE;

     read_size = http_sock_rcv(readbuffer, request_size, sockHnd);
     if (read_size < 0)
     {
       /* HTTP download or socket error. Will retry. */
       continue;
     }
     
     received_size += read_size;
     
#ifndef RFU_DRY_RUN
      printf("flash %x  %d\n",areaAddr,ALIGN64(read_size));
      rc = FLASH_Write(areaAddr, (uint32_t *) readbuffer, ALIGN64(read_size));    
      if (rc ) 
      {
        printf("ERROR:Unable to write flash area at [%x - %x [\n", areaAddr,ALIGN64(read_size));
      }
      areaAddr += read_size;
#endif
  }
  while(received_size < file_size);
  
  printf("Downloaded total size %d bytes\n",received_size);

  http_sock_close(sockHnd);

  /* - System Reboot*/
#ifndef RFU_DRY_RUN
#ifndef OLD_RFU 
  SE_SFU_IMG_Install(&e_se_status, (uint8_t *) fw_header_input);
  if (e_se_status != SE_OK) {
    printf("ERROR , unable to install headers"); 
  }
  printf("  -- Image correctly downloaded - reboot\r\n\n");
  HAL_Delay(1000U);
  NVIC_SystemReset();
#endif
#endif
  return rc;
}
#endif

