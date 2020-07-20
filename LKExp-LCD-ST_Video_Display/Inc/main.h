/**
  ******************************************************************************
  * @file    JPEG/JPEG_DecodingUsingFs_DMA/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"

#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_lcd.h"
#include "stm32f769i_discovery_sdram.h"

/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#include "jpeg_utils.h"
#include "decode_dma.h"

/* Definition for USARTx clock resources */
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __USART1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF7_USART1

/* Exported variables --------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define LCD_FRAME_BUFFER         0xC0000000
#define JPEG_OUTPUT_DATA_BUFFER  0xC0200000 

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void OnError_Handler(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
