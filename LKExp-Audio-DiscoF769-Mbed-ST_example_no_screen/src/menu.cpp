/**
  ******************************************************************************
  * @file    Audio/Audio_playback_and_record/Src/menu.c 
  * @author  MCD Application Team
  * @brief   This file implements Menu Functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "waveplayer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Global extern variables ---------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
AUDIO_DEMO_StateMachine     AudioDemo;
AUDIO_PLAYBACK_StateTypeDef AudioState;

/* Private function prototypes -----------------------------------------------*/
static void AUDIO_ChangeSelectMode(AUDIO_DEMO_SelectMode select_mode);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Manages AUDIO Menu Process.
  * @param  None
  * @retval None
  */
void AUDIO_MenuProcess(void)
{  
  if(appli_state == APPLICATION_READY)
  { 
    switch(AudioDemo.state)
    {
    case AUDIO_DEMO_IDLE:
      
      AudioDemo.state = AUDIO_DEMO_WAIT;
      break;    
      
    case AUDIO_DEMO_WAIT:
          AudioDemo.state = AUDIO_DEMO_PLAYBACK;
      break;
      
    case AUDIO_DEMO_EXPLORE: 
      if(appli_state == APPLICATION_READY)
      {
        if(AUDIO_ShowWavFiles() > 0)
        {
          debug("[ERROR] There is no WAV file on the SD card.\n");
          AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU); 
          AudioDemo.state = AUDIO_DEMO_IDLE;
        }
        else
        {
          AudioDemo.state = AUDIO_DEMO_WAIT;
        }
      }
      else
      {
        AudioDemo.state = AUDIO_DEMO_WAIT;
      }
      break;
      
    case AUDIO_DEMO_PLAYBACK:
      if(appli_state == APPLICATION_READY)
      {
        if(AudioState == AUDIO_STATE_IDLE)
        {
          if(AUDIO_ShowWavFiles() > 0)
          {
            debug("[ERROR] There is no WAV file on the SD card.\n");
            AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU); 
            AudioDemo.state = AUDIO_DEMO_IDLE;
          }
          else
          {
            /* Start Playing */
            AudioState = AUDIO_STATE_INIT;
          }
          
          if(AUDIO_PLAYER_Start(0) == AUDIO_ERROR_IO)
          {
            AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU); 
            AudioDemo.state = AUDIO_DEMO_IDLE;
          }
        }
        else /* Not idle */
        {
          if(AUDIO_PLAYER_Process() == AUDIO_ERROR_IO)
          {
            AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU);  
            AudioDemo.state = AUDIO_DEMO_IDLE;
          }
        }
      }
      else
      {
        AudioDemo.state = AUDIO_DEMO_WAIT;
      }
      break; 
      
    default:
      break;
    }
  }
  
  if(appli_state == APPLICATION_DISCONNECT)
  {
    appli_state = APPLICATION_IDLE;     
    AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU); 
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);    
  }
}

/*******************************************************************************
                            Static Functions
*******************************************************************************/

/**
  * @brief  Changes the selection mode.
  * @param  select_mode: Selection mode
  * @retval None
  */
static void AUDIO_ChangeSelectMode(AUDIO_DEMO_SelectMode select_mode)
{
  if(select_mode == AUDIO_SELECT_MENU)
  {
    AudioDemo.state = AUDIO_DEMO_IDLE; 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
