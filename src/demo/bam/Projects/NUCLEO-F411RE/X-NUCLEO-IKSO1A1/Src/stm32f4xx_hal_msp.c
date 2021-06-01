/**
  ******************************************************************************
  * @file    Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    26-August-2016
  * @brief   HAL MSP module.
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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - DMA configuration for transmission request by peripheral
  *           - NVIC configuration for DMA interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  static DMA_HandleTypeDef hdma_tx;
//  static DMA_HandleTypeDef hdma_rx;

  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USART2_TX_GPIO_CLK_ENABLE();
  USART2_RX_GPIO_CLK_ENABLE();
  /* Enable USART2 clock */
  USART2_CLK_ENABLE();
  /* Enable DMA1 clock */
  DMA1_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USART2_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = USART2_TX_AF;

  HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USART2_RX_PIN;
  GPIO_InitStruct.Alternate = USART2_RX_AF;

  HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA streams ##########################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = USART2_TX_DMA_STREAM;
  hdma_tx.Init.Channel             = USART2_TX_DMA_CHANNEL;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
  hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
  hdma_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

  HAL_DMA_Init(&hdma_tx);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmatx, hdma_tx);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (USART1_TX) */
  HAL_NVIC_SetPriority(USART2_DMA_TX_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USART2_DMA_TX_IRQn);

}

/**
  * @brief RTC MSP Initialization
  *        This function configures the hardware resources used in this example
  * @param hrtc: RTC handle pointer
  *
  * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
  *        the RTC clock source; in this case the Backup domain will be reset in
  *        order to modify the RTC Clock source, as consequence RTC registers (including
  *        the backup registers) and RCC_BDCR register are set to their reset values.
  *
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTc clock source */

  /*##-1- Configue LSE as RTC clock soucre ###################################*/
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /*##-2- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();

}

/**
  * @brief RTC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_RTC_DISABLE();
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  USART2_FORCE_RESET();
  USART2_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(USART2_TX_GPIO_PORT, USART2_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(USART2_RX_GPIO_PORT, USART2_RX_PIN);

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
