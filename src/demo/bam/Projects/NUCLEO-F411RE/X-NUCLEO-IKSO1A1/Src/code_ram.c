/**
  ******************************************************************************
  * @file    Src/code_ram.c 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    26-August-2016
  * @brief   This file describes how to stop flash           
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "code_ram.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define DELAY()  do {uint32_t counter, control, load;\
                    \
	                  /* Conserve the sysytick context */\
	                  control = SysTick->CTRL; SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;\
	                  counter = SysTick->VAL; load = SysTick->LOAD;\
                    \
	                  /* Wait for 5us */\
	                  SysTick->VAL = 0; SysTick->LOAD = 80; SysTick->CTRL = 5;\
	                  while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));\
	                  SysTick->CTRL = 0;\
                    \
	                  /* Restore the systick context */\
	                  SysTick->VAL = counter; SysTick->LOAD = load;SysTick->CTRL = control;\
                   }while(0) 

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


 void CodeToRAM (void)
{
  /* Stop flash */	
	PWR->CR |= PWR_CR_FMSSR; 
	
	/* disable clock interface */	
	PWR->CR |= PWR_CR_FISSR;	
	
	/* Enter to sleep */
	__WFI();
	
  /* Renable flash clock interface */
	PWR->CR &= ~(PWR_CR_FISSR);
	
	/* Renable flash */
	PWR->CR &= ~(PWR_CR_FMSSR);	
	
  /* Data synchro barrier: bloc CPU while last action is not finished */
	__DSB(); 

	/* Wait for 5us */
	DELAY();

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
