/**
  ******************************************************************************
  * @file    sensors_data.h
  * @author  MCD Application Team
  * @brief   Header for configuration
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __sensors_data_h
#define __sensors_data_h
#ifdef __cplusplus
 extern "C" {
#endif

int init_sensors(void);
int PrepareSensorsData(char * Buffer, int Size, char * deviceID);

#ifdef __cplusplus
}
#endif
#endif /* __sensors_data_h */

