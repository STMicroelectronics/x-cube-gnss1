/**
  ******************************************************************************
  * @file    cloud.h
  * @author  MCD Application Team
  * @brief   main application header file.
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
#ifndef __cloud_h__
#define __cloud_h__

#ifdef __cplusplus
extern "C" {
#endif

/** Provided interface */  
int platform_init(void);
void platform_deinit(void);
bool dialog_ask(char *s);

/** Required interface */
extern bool app_needs_device_keypair(void);
extern int cloud_device_enter_credentials(void);  

// TODO: move out!
int bluemix_test(void);
int subscribe_publish_sensor_values(void);

#ifdef __cplusplus
}
#endif

#endif /* __cloud_h__ */
