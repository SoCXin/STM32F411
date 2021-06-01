/**
  ******************************************************************************
  * @file    Src/main.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    26-August-2016
  * @brief   Main program body
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

typedef struct
{
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} DMA_Base_Registers;


RTC_HandleTypeDef RTCHandle;
GPIO_InitTypeDef  GPIO_InitStruct;
UART_HandleTypeDef UARTHandle;
I2C_HandleTypeDef I2CHandle;
UART_HandleTypeDef *huart;
/* Private macro -------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Private define ------------------------------------------------------------*/
#define UART_TIMEOUT_VALUE     ((uint32_t)22000)
#define I2C_TIMEOUT_FLAG       ((uint32_t)35)
#define I2C_TIMEOUT_BUSY_FLAG  ((uint32_t)25) 
#define I2C_NO_OPTION_FRAME    ((uint32_t)0xFFFF0000) /* XferOptions default value */
#define I2C_STATE_NONE         ((uint32_t)(HAL_I2C_MODE_NONE)) 
#define I2CHandle  I2C_SHIELDS_Handle

#define RTC_ASYNCH_PREDIV      ((uint32_t)0x20)
#define RTC_SYNCH_PREDIV       ((uint32_t)0x03FF)

#define CONFIG1 ((uint32_t)0x31)
#define CONFIG2 ((uint32_t)0x32)
#define CONFIG3 ((uint32_t)0x33)
#define CONFIG4 ((uint32_t)0x34)

#define TC_IT_ENABLE  ((uint32_t)0x01)
#define TC_IT_DISABLE ((uint32_t)0x02)

#define MESSAGE1SIZE        (COUNTOF(Msg1) - 1)
#define MESSAGE2SIZE        (COUNTOF(Msg2) - 1)
#define MESSAGE3SIZE        (COUNTOF(Msg3) - 1)
#define MESSAGE4SIZE        (COUNTOF(Msg4) - 1)
#define MESSAGE5SIZE        (COUNTOF(Msg5) - 1)
#define MEMSBUFFERSIZE      (COUNTOF(aMemsBuf) - 1)
#define BUFFERSIZE          (COUNTOF(aBuf) - 1)
#define RXBUFFERSIZE        (COUNTOF(aRxBuffer) - 1)


/* Private variables ---------------------------------------------------------*/
uint8_t  Config = 0;
uint8_t TimeStamp_FStop = 0;
uint8_t DMA_TC = 0;
char Ch;

/* Buffers used for displaying time stamp */
uint8_t aShowTime[70] = {0};
uint8_t aExitStandBy[70] = {0};
uint8_t aEnterSleep1[70] = {0};
uint8_t aExitSleep1[70] = {0};
uint8_t aEnterStop[70] = {0};
uint8_t aExitStop[70] = {0};
uint8_t aEnterSleep2[70] = {0};
uint8_t aExitSleep2[70] = {0};
uint8_t aEnterSleep3[70] = {0};
uint8_t aExitSleep3[70] = {0};
uint8_t aEnterSleep4[70] = {0};
uint8_t aExitSleep4[70] = {0};
uint8_t aEnterSleep5[70] = {0};
uint8_t aExitSleep5[70] = {0};
uint8_t aEnterSleep6[70] = {0};
uint8_t aExitSleep6[70] = {0};
uint8_t aMemsBuf[150] = {0};
uint8_t aBuf[1000] = {0};

/* Variables used for accelerometer */
int16_t aBuffer[3];
uint8_t aRxBuffer[6];


uint8_t Msg1[]= "\nWakeUp from STDBY ...\n\
Enter to SLEEP1 ...\n\n\n\
######################################################## \n\
#                                                      # \n\
# AN4515-Dynamic_Low_Power_Efficiency_In_The_STM32F411 # \n\
#                                                      # \n\
######################################################## \n\
\n\n\n\n\
************** Configuration Menu *********************\n\n\
1: Flash active while CPU in sleep\n\
2: Stop Flash while CPU in sleep \n\
3: Enable Time Stamp and Flash active while CPU in sleep\n\
4: Enable Time Stamp and stop Flash while CPU in sleep\n\n\
*******************************************************\n\n\
WakeUp from SLEEP1 and enter to STOP ...\n\n\
Read configuration menu and press any key board to wakeup\n\
from STOP\n>>";

uint8_t Msg2[]= "\nWakeUp from STOP and enter to SLEEP2 ...\n\
\n\
\nSelect Your Configuration ...\n>>";

uint8_t Msg3[]= "\nConfiguration Selected Correctly\n\
WakeUp from SLEEP2 ...\n\
ENTER to SLEEP3 ...";

uint8_t Msg4[]= "\nWrong Command !!\n\n\n\
Select Your Configuration ...\n\
ENTER to SLEEP2 ... \n>>";

uint8_t Msg5[]="\nWakeUp from SLEEP3 ...\n\n\
Enter to SLEEP4 ...\n\n\
### Move MEMS UP or DOWN ###\n";

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void StandbyMode(void);
static void SleepMode1(void);
static void SleepMode2(void);
static void StopMode(void);
static void GPIO_AnalogConfig(void);
static uint8_t ConfigSelection(void);
static void USART2_Configuration(void);
static void EXTILine3_Config(void);

void CodeToRAM (void);
static void RTC_CalendarConfig(void);
static uint8_t TimeStampDisplay(uint8_t *showtime);
static HAL_StatusTypeDef UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t TC_IT_Status);
static HAL_StatusTypeDef I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t TC_IT_Status);
static HAL_StatusTypeDef DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength, uint32_t TC_IT_Status);
static void DMA_Config(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAXferCplt(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout);
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnTXEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c);
static void I2C_DMAError(DMA_HandleTypeDef *hdma);

static void UART_DMAError(DMA_HandleTypeDef *hdma);
static void UART_EndTxTransfer(UART_HandleTypeDef *huart);
static void UART_EndRxTransfer(UART_HandleTypeDef *huart);

static HAL_StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout, uint32_t Tickstart);
static void DMA_Transmit(uint8_t *pData, uint16_t Size, uint32_t TC_IT_Status);
static void DMA_IRQHandler(DMA_HandleTypeDef *hdma);
static uint8_t I2C1_ReadData(uint16_t Addr, uint8_t Reg);
static void DMA_I2C_Config(void);
static void RTC_TamperConfig(void);
static void EXTILine5_Config(void);
static void GetAxesRaw(int16_t *pData);
static void GetAxes(int16_t *pData);
static void LSM6DS0Config(void);


#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(int ch)

__IO uint32_t VectorTable[101] __attribute__((section(".RAMVectorTable")));

#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  #ifdef __GNUC__
  uint32_t index=0;
  /* Relocate by software the vector table to the internal SRAM at 0x20000000 ***/
    /* Copy the vector table from the Flash (mapped at the base of the application
        load address 0x08004000) to the base address of the SRAM at 0x20000000. */
     for(index = 0; index < 101; index++)
     {
       VectorTable[index] = *(__IO uint32_t*)(0x08000000 + (index<<2));
     }
  #endif /* __GNUC__ */
     
  int8_t threshold_accelero = 0;
  uint32_t buffer_index = 0;
  uint32_t loop = 0;

  RTCHandle.Instance = RTC;

  /* Configure the system clock to 16 MHz */
  SystemClock_Config();

  /* Check that the system was resumed from StandBy mode with WakeUp */
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_WU) == RESET)
  {
    /* Configure the RTC peripheral */
    RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&RTCHandle) != HAL_OK)
    {
      /* Error */
      Error_Handler();
    }

    /* Configure RTC Calendar */
    RTC_CalendarConfig();
	
    /* Clear the Tamper Flag */
    __HAL_RTC_TAMPER_CLEAR_FLAG(&RTCHandle, RTC_FLAG_TAMP1F);
  
    /* Configure RTC Tamper */
    RTC_TamperConfig();
		
    /***** Entering in standby mode *****/
#ifdef DEBUG
   HAL_EnableDBGStandbyMode();
#endif

    StandbyMode();
  }
  else
  {		
    /* Desactivate Tamper */
    HAL_RTCEx_DeactivateTamper(&RTCHandle, RTC_TAMPER_1);
		
    /*** Display Time Stamp ***/
    TimeStampDisplay(aShowTime);
    sprintf((char*)aExitStandBy, "%s: WakeUp from STDBY ", aShowTime);

    HAL_Init();
    GPIO_AnalogConfig();
    USART2_Configuration();

    /*** Display Time Stamp ***/
    TimeStampDisplay(aShowTime);
    sprintf((char*)aEnterSleep1, "%s: Enter to SLEEP1", aShowTime);

    UART_Transmit_DMA(&UARTHandle, Msg1, MESSAGE1SIZE, TC_IT_ENABLE);

    /***** Entering in sleep mode *****/
#ifdef DEBUG
    HAL_EnableDBGSleepMode();
#endif
    SleepMode1();

    /*** Display Time Stamp ***/
    TimeStampDisplay(aShowTime);
    sprintf((char*)aExitSleep1, "%s: WakeUp from SLEEP1", aShowTime);

    /***** Entering in stop mode *****/
#ifdef DEBUG
    HAL_EnableDBGStopMode();
#endif
    StopMode();

    /* Reconfigure the RX pin as AF */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USART2_RX_PIN;
    GPIO_InitStruct.Alternate = USART2_RX_AF;
    HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);
    __USART2_CLK_ENABLE();

    /*** Display Time Stamp ***/
    TimeStampDisplay(aShowTime);
    sprintf((char*)aExitStop, "%s: WakeUp from STOP", aShowTime);

    /* Wait until the entire character is received */
    while (!(__HAL_UART_GET_FLAG(&UARTHandle, UART_FLAG_RXNE)))
    {}

    /* Read DR content and clear it */
    (void)USART2->DR;

    /*** Display Time Stamp ***/
    TimeStampDisplay(aShowTime);
    sprintf((char*)aEnterSleep2, "%s: Enter to SLEEP2", aShowTime);

    UART_Transmit_DMA(&UARTHandle, Msg2, MESSAGE2SIZE, TC_IT_DISABLE );

    /***** Entering in sleep mode *****/
#ifdef DEBUG
    HAL_EnableDBGSleepMode();
#endif
    SleepMode1();

    /*** Display Time Stamp ***/
    TimeStampDisplay(aShowTime);
    sprintf((char*)aExitSleep2, "%s: WakeUp from SLEEP2", aShowTime);

    while (__HAL_DMA_GET_FLAG(UARTHandle.hdmatx, DMA_FLAG_TCIF2_6) == RESET)
    {}
    DMA_IRQHandler(UARTHandle.hdmatx);

    while ((Config != CONFIG1) & (Config != CONFIG2) & (Config != CONFIG3) & (Config != CONFIG4))
    {
      ConfigSelection();
    }
  
    /* Initialize Accelerometer */
    if (BSP_IMU_6AXES_Init() != IMU_6AXES_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }
		
		/* LSM6DS0 configuration */  
		LSM6DS0Config();
		
    /*** Display Time Stamp ***/
    TimeStampDisplay(aShowTime);
    sprintf((char*)aEnterSleep4, "%s: Enter to SLEEP4", aShowTime);

    UART_Transmit_DMA(&UARTHandle, Msg5, MESSAGE5SIZE, TC_IT_DISABLE );
    /***** Entering in sleep mode *****/
#ifdef DEBUG
    HAL_EnableDBGSleepMode();
#endif
    SleepMode2();

    /*** Display Time Stamp ***/
    TimeStampDisplay(aShowTime);
    sprintf((char*)aExitSleep4, "%s: WakeUp from SLEEP4 ", aShowTime);

    while (__HAL_DMA_GET_FLAG(UARTHandle.hdmatx, DMA_FLAG_TCIF2_6) == RESET)
    {}
    DMA_IRQHandler(UARTHandle.hdmatx);

    /* DMA configuration for reception from I2C */
    DMA_I2C_Config();

    while (1)
    {
      /* Read Acceleration*/
			GetAxes(aBuffer);
			
      if ((TimeStamp_FStop == CONFIG3) | (TimeStamp_FStop == CONFIG4))
      {
        if (loop == 0)
        {
          loop++;
          TimeStampDisplay(aShowTime);
          sprintf((char*)aEnterSleep6, "%s: Enter to SLEEP6", aShowTime);
					
          if (aBuffer[1] < threshold_accelero )
					{
            sprintf((char*)aBuf, "\n************** Direction Display ***************\nDOWN Direction\n\n************** TimeStamp Display **************\n\n00:00:00:000: Enter STDBY\
                    %s%s%s%s%s%s%s%s%s%s%s%s%s%s\n\n\n## Move MEMS ##\n\n", aExitStandBy, aEnterSleep1, aExitSleep1, aEnterStop, aExitStop, aEnterSleep2, aExitSleep2, aEnterSleep3, aExitSleep3, aEnterSleep4, aExitSleep4, aEnterSleep5, aExitSleep5, aEnterSleep6);
          }
					else
					{
            sprintf((char*)aBuf, "\n************** Direction Display ***************\nUP Direction\n\n************** TimeStamp Display **************\n\n00:00:00:000: Enter STDBY\
                    %s%s%s%s%s%s%s%s%s%s%s%s%s%s\n\n\n## Move MEMS ##\n\n", aExitStandBy, aEnterSleep1, aExitSleep1, aEnterStop, aExitStop, aEnterSleep2, aExitSleep2, aEnterSleep3, aExitSleep3, aEnterSleep4, aExitSleep4, aEnterSleep5, aExitSleep5, aEnterSleep6);
					}
        }
        else
        {
          for (buffer_index = 0; buffer_index < BUFFERSIZE; buffer_index++)
          {
            aBuf[buffer_index] = 0;
          }
          TimeStampDisplay(aShowTime);
          sprintf((char*)aEnterSleep6, "%s: Enter to SLEEP6", aShowTime);
          if (aBuffer[1] < threshold_accelero )
					{
            sprintf((char*)aBuf, "\n************** Direction Display ***************\nDOWN Direction\n\n************** TimeStamp Display **************\n\
                    %s%s%s%s\n\n\n## Move MEMS ##\n\n", aExitSleep6, aEnterSleep5, aExitSleep5, aEnterSleep6);
          }
					else
					{
            sprintf((char*)aBuf, "\n************** Direction Display ***************\nUP Direction\n\n************** TimeStamp Display **************\n\
                    %s%s%s%s\n\n\n## Move MEMS ##\n\n", aExitSleep6, aEnterSleep5, aExitSleep5, aEnterSleep6);
					}
        }
        DMA_Transmit((uint8_t*)aBuf, BUFFERSIZE, TC_IT_DISABLE);
      }

      else /* TimeStamp_FStop != (CONFIG3 & CONFIG4)*/
      {
        if (aBuffer[1] < threshold_accelero )
				{
          sprintf((char*)aMemsBuf, "\n************** Direction Display ***************\nDOWN Direction\n\n\n## Move MEMS ##\n\n");
        }
				else
				{
          sprintf((char*)aMemsBuf, "\n************** Direction Display ***************\nUP Direction\n\n\n## Move MEMS ##\n\n");
				}
        TimeStampDisplay(aShowTime);
        sprintf((char*)aEnterSleep6, "%s: Enter to SLEEP6", aShowTime);
        DMA_Transmit((uint8_t*)aMemsBuf, MEMSBUFFERSIZE, TC_IT_DISABLE);
      }

      /***** Entering in sleep mode *****/
#ifdef DEBUG
      HAL_EnableDBGSleepMode();
#endif
      SleepMode2();

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aExitSleep6, "%s: WakeUp from SLEEP6", aShowTime);

      while (__HAL_DMA_GET_FLAG(UARTHandle.hdmatx, DMA_FLAG_TCIF2_6) == RESET)
      {}
      DMA_IRQHandler(UARTHandle.hdmatx);
    }
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = HSI
  *            SYSCLK(Hz)      	              = 16000000
  *            HCLK(Hz)                       = 16000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale3 mode
  *            Flash Latency(WS)              = 0
  * @param  None
  * @retval None
  */              
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK ;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
static void RTC_CalendarConfig(void)
{
	RTC_TimeTypeDef stimestructure;
  /* Configure the Time */
  /* Set Time: 00:00:00 */
  stimestructure.Hours = 0x00;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RTCHandle, &stimestructure, FORMAT_BCD) != HAL_OK)
  {
    /* Error */
    Error_Handler();
  }
}

/**
  * @brief  Display the current time
  * @param  showtime: pointer to buffer
  * @retval None
  */
uint8_t TimeStampDisplay(uint8_t *showtime)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
	
  uint16_t subsec=0;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RTCHandle, &stimestructureget, FORMAT_BIN);
  /* Call HAL_RTC_GetDate() after HAL_RTC_GetTime() to unlock the values
  in the higher-order calendar shadow registers */
  HAL_RTC_GetDate(&RTCHandle, &sdatestructureget, FORMAT_BIN);
  subsec = (uint32_t)(stimestructureget.SubSeconds);
  subsec = 999 - (subsec - subsec / 42);

  /* Display time Format: hh:mm:ss:sss */
  sprintf((char*)showtime, "\n%02d:%02d:%02d:%03d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds, subsec);

  return (uint8_t) *showtime;
}

/**
  * @brief  This function configures the system to enter Standby mode for
  *         current consumption measurement purpose.
  * @param  None
  * @retval None
  */
static void StandbyMode(void)
{
  /* Enable Power Clock */
  __PWR_CLK_ENABLE();

  /* Clear all related wakeup flags */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  /* Disable all used wakeup sources: Pin1(PA.0) */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

  /* Re-enable all used wakeup sources: Pin1(PA.0) */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

  /* Suspend Tick increment to prevent wakeup by Systick interrupt.
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /*## Enter Standby Mode ####################################################*/
  /* Request to enter STANDBY mode  */
  HAL_PWR_EnterSTANDBYMode();

  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
}

/**
  * @brief  This function configures the system to enter Sleep mode
  * @param  None
  * @retval None
  */
static void SleepMode1(void)
{
  /* Suspend Tick increment to prevent wakeup by Systick interrupt.
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /* Request to enter SLEEP mode */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
}

/**
  * @brief  This function configures the system to enter Sleep mode
  * @param  None
  * @retval None
  */
static void SleepMode2(void)
{
	/* Configures EXTI Line5 (connected to PB5 pin) in interrupt mode */
  EXTILine5_Config();
	
  /* Suspend Tick increment to prevent wakeup by Systick interrupt.
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /* Request to enter SLEEP mode */
  if ((TimeStamp_FStop == CONFIG2) | (TimeStamp_FStop == CONFIG4))
  {
		PWR->CR &= ~PWR_CR_LPDS;
    PWR->CR &= ~PWR_CR_FPDS;
    /* Routine executed from SRAM to stop the Flash */
    CodeToRAM();
  }

  else
  {
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  }
  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
}

/**
  * @brief  This function configures the system to enter Stop mode
  * @param  None
  * @retval None
  */
static void StopMode(void)
{
  /* FLASH Deep Power Down Mode enabled */
  HAL_PWREx_EnableFlashPowerDown();

  /* Configure Key Button */
  EXTILine3_Config();
  /* Suspend Tick increment to prevent wakeup by Systick interrupt.
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /*** Display Time Stamp ***/
  TimeStampDisplay(aShowTime);
  sprintf((char*)aEnterStop, "%s: Enter to STOP", aShowTime);

  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON | (1 << 10), PWR_STOPENTRY_WFI);

  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
}

/*
  * @brief  Configure all GPIO as analog to reduce current consumption on non used IOs
  * @param  None
  * @retval None
  */
static void GPIO_AnalogConfig (void)
{
  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  __GPIOB_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOE_CLK_ENABLE();
  __GPIOH_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

#ifndef DEBUG
  __GPIOA_CLK_ENABLE();
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  __GPIOA_CLK_DISABLE();
#endif /* not DEBUG */

  /* Disable GPIOs clock */
  __GPIOB_CLK_DISABLE();
  __GPIOC_CLK_DISABLE();
  __GPIOD_CLK_DISABLE();
  __GPIOE_CLK_DISABLE();
  __GPIOH_CLK_DISABLE();

}
/**
  * @brief  This function configures the application menu
  * @param  None
  * @retval None
  */
static uint8_t ConfigSelection(void)
{
  
  #ifdef __GNUC__
  setvbuf(stdin,NULL,_IONBF,0);
  #endif
  char c;
	
  /* Select the configuration */
  c = getchar();
  printf(" \r%c\n", c);
  Config = Ch;
  TimeStamp_FStop = 0;

  switch (Config)
  {
    case CONFIG1:
      TimeStamp_FStop = CONFIG1;
      UART_Transmit_DMA(&UARTHandle, Msg3, MESSAGE3SIZE, TC_IT_ENABLE );

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aEnterSleep3, "%s: Enter to SLEEP3", aShowTime);

      /***** Entering in sleep mode *****/
#ifdef DEBUG
      HAL_EnableDBGSleepMode();
#endif
      SleepMode1();

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aExitSleep3, "%s: WakeUp from SLEEP3", aShowTime);
    break;

    case CONFIG2:
      TimeStamp_FStop = CONFIG2;
      UART_Transmit_DMA(&UARTHandle, Msg3, MESSAGE3SIZE, TC_IT_ENABLE );

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aEnterSleep3, "%s: Enter to SLEEP3", aShowTime);

      /***** Entering in sleep mode *****/
#ifdef DEBUG
      HAL_EnableDBGSleepMode();
#endif
      SleepMode1();

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aExitSleep3, "%s: WakeUp from SLEEP3", aShowTime);
    break;
		
    case CONFIG3:
      TimeStamp_FStop = CONFIG3;
      UART_Transmit_DMA(&UARTHandle, Msg3, MESSAGE3SIZE, TC_IT_ENABLE );

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aEnterSleep3, "%s: Enter to SLEEP3", aShowTime);

      /***** Entering in sleep mode *****/
#ifdef DEBUG
      HAL_EnableDBGSleepMode();
#endif
      SleepMode1();

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aExitSleep3, "%s: WakeUp from SLEEP3", aShowTime);
    break;
		
    case CONFIG4:
      TimeStamp_FStop = CONFIG4;
      UART_Transmit_DMA(&UARTHandle, Msg3, MESSAGE3SIZE, TC_IT_ENABLE );

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aEnterSleep3, "%s: Enter to SLEEP3", aShowTime);

      /***** Entering in sleep mode *****/
#ifdef DEBUG
      HAL_EnableDBGSleepMode();
#endif
      SleepMode1();

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aExitSleep3, "%s: WakeUp from SLEEP3", aShowTime);
    break;

    default:
      UART_Transmit_DMA(&UARTHandle, Msg4, MESSAGE4SIZE, TC_IT_DISABLE );

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aEnterSleep3, "%s: Enter to SLEEP3", aShowTime);

      /***** Entering in sleep mode *****/
#ifdef DEBUG
      HAL_EnableDBGSleepMode();
#endif
      SleepMode1();

      /*** Display Time Stamp ***/
      TimeStampDisplay(aShowTime);
      sprintf((char*)aExitSleep3, "%s: WakeUp from SLEEP3", aShowTime);

      while (__HAL_DMA_GET_FLAG(UARTHandle.hdmatx, DMA_FLAG_TCIF2_6) == RESET)
      {}
      DMA_IRQHandler(UARTHandle.hdmatx);
    break;
  }
  return TimeStamp_FStop;
}

/**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */
static void USART2_Configuration(void)
{
  /* Configure the UART peripheral */
  /* UART2 configured as follow:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = without parity
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UARTHandle.Instance        = USART2;
  UARTHandle.Init.BaudRate   = 9600;
  UARTHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UARTHandle.Init.StopBits   = UART_STOPBITS_1;
  UARTHandle.Init.Parity     = UART_PARITY_NONE;
  UARTHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UARTHandle.Init.Mode       = UART_MODE_TX_RX;

  if (HAL_UART_Init(&UARTHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  Sends an amount of data in non blocking mode. 
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t TC_IT_Status)
{
  uint32_t *tmp;
  
  /* Check that a Tx process is not already ongoing */
  if(huart->gState == HAL_UART_STATE_READY)
  {
    if((pData == NULL ) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(huart);

    huart->pTxBuffPtr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

    /* Set the UART DMA transfer complete callback */
    huart->hdmatx->XferCpltCallback = UART_DMATransmitCplt;

    /* Set the DMA error callback */
    huart->hdmatx->XferErrorCallback = UART_DMAError;

    /* Set the DMA abort callback */
    huart->hdmatx->XferAbortCallback = NULL;

    /* Enable the UART transmit DMA Stream */
    tmp = (uint32_t*)&pData;
    DMA_Start_IT(huart->hdmatx, *(uint32_t*)tmp, (uint32_t)&huart->Instance->DR, Size, TC_IT_Status);
    
    /* Clear the TC flag in the SR register by writing 0 to it */
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);
    
    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    
    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the UART CR3 register */
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAT);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Reads an amount of data in no-blocking mode with DMA from a specific memory address.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress Target device address
  * @param  MemAddress Internal memory address
  * @param  MemAddSize Size of internal memory address
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be read
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t TC_IT_Status)
{
  uint32_t tickstart = 0x00U;

  /* Init tickstart for timeout management*/
  tickstart = HAL_GetTick();

  __IO uint32_t count = 0U;

  /* Check the parameters */
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    /* Wait until BUSY flag is reset */
    count = I2C_TIMEOUT_BUSY_FLAG * (SystemCoreClock /25U /1000U);
    do
    {
      if(count-- == 0U)
      {
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State= HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_TIMEOUT; 
      }
    }
    while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) != RESET);

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Disable Pos */
    hi2c->Instance->CR1 &= ~I2C_CR1_POS;

    hi2c->State = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode = HAL_I2C_MODE_MEM;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;
    hi2c->XferOptions = I2C_NO_OPTION_FRAME;

    if(hi2c->XferSize > 0U)
    {
      /* Set the I2C DMA transfer complete callback */
      hi2c->hdmarx->XferCpltCallback = I2C_DMAXferCplt;

      /* Set the DMA error callback */
      hi2c->hdmarx->XferErrorCallback = I2C_DMAError;

      /* Set the unused DMA callbacks to NULL */
      hi2c->hdmarx->XferHalfCpltCallback = NULL;
      hi2c->hdmarx->XferM1CpltCallback = NULL;
      hi2c->hdmarx->XferM1HalfCpltCallback = NULL;
      hi2c->hdmarx->XferAbortCallback = NULL;

      /* Enable the DMA Stream */
      DMA_Start_IT(hi2c->hdmarx, (uint32_t)&hi2c->Instance->DR, (uint32_t)pData, Size, TC_IT_Status);

      /* Send Slave Address and Memory Address */
      if(I2C_RequestMemoryRead(hi2c, DevAddress, MemAddress, MemAddSize, I2C_TIMEOUT_FLAG, tickstart) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);
          return HAL_ERROR;
        }
        else
        {
          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);
          return HAL_TIMEOUT;
        }
      }

      if(Size == 1U)
      {
        /* Disable Acknowledge */
        hi2c->Instance->CR1 &= ~I2C_CR1_ACK;
      }
      else
      {
        /* Enable Last DMA bit */
        hi2c->Instance->CR2 |= I2C_CR2_LAST;
      }

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      /* Note : The I2C interrupts must be enabled after unlocking current process
                to avoid the risk of I2C interrupt handle execution before current
                process unlock */
      /* Enable ERR interrupt */
      __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_ERR);
      
     /* Enable DMA Request */
      hi2c->Instance->CR2 |= I2C_CR2_DMAEN;
    }
    else
    {
      /* Send Slave Address and Memory Address */
      if(I2C_RequestMemoryRead(hi2c, DevAddress, MemAddress, MemAddSize, I2C_TIMEOUT_FLAG, tickstart) != HAL_OK)
      {
        if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        {
          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);
          return HAL_ERROR;
        }
        else
        {
          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);
          return HAL_TIMEOUT;
        }
      }

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;

      hi2c->State = HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Master sends target device address followed by internal memory address for read request.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress Target device address
  * @param  MemAddress Internal memory address
  * @param  MemAddSize Size of internal memory address
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart)
{
  /* Enable Acknowledge */
  hi2c->Instance->CR1 |= I2C_CR1_ACK;

  /* Generate Start */
  hi2c->Instance->CR1 |= I2C_CR1_START;

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* Send slave address */
  hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);

  /* Wait until ADDR flag is set */
  if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  /* Clear ADDR flag */
  __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

  /* Wait until TXE flag is set */
  if(I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  /* If Memory address size is 8Bit */
  if(MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    hi2c->Instance->DR = I2C_MEM_ADD_LSB(MemAddress);
  }
  /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    hi2c->Instance->DR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXE flag is set */
    if(I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
    {
      if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Generate Stop */
        hi2c->Instance->CR1 |= I2C_CR1_STOP;
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Send LSB of Memory Address */
    hi2c->Instance->DR = I2C_MEM_ADD_LSB(MemAddress);
  }

  /* Wait until TXE flag is set */
  if(I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  /* Generate Restart */
  hi2c->Instance->CR1 |= I2C_CR1_START;

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* Send slave address */
  hi2c->Instance->DR = I2C_7BIT_ADD_READ(DevAddress);

  /* Wait until ADDR flag is set */
  if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != HAL_OK)
  {
    if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      return HAL_ERROR;
    }
    else
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for Master addressing phase.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  Flag specifies the I2C flag to check.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout, uint32_t Tickstart)
{
  while(__HAL_I2C_GET_FLAG(hi2c, Flag) == RESET)
  {
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
    {
      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;

      /* Clear AF Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

      hi2c->ErrorCode = HAL_I2C_ERROR_AF;
      hi2c->PreviousState = I2C_STATE_NONE;
      hi2c->State= HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout))
      {
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State= HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Starts the DMA Transfer with interrupt enabled.
  * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.  
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
static HAL_StatusTypeDef DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength, uint32_t TC_IT_Status)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* calculate DMA base and stream number */
  DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;
  
  /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));
 
  /* Process locked */
  __HAL_LOCK(hdma);
  
  if(HAL_DMA_STATE_READY == hdma->State)
  {
    /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;
    
    /* Initialize the error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;
    
    /* Configure the source, destination address and the data length */
    DMA_Config(hdma, SrcAddress, DstAddress, DataLength);
    
    /* Clear all interrupt flags at correct offset within the register */
    regs->IFCR = 0x3FU << hdma->StreamIndex;
		
   if (TC_IT_Status == TC_IT_ENABLE)
  {   
    /* Enable the transfer complete interrupt */
    hdma->Instance->CR  |= DMA_IT_TC;
  }
    /* Enable Common interrupts*/
    hdma->Instance->CR  |= DMA_IT_TE | DMA_IT_DME;
    hdma->Instance->FCR |= DMA_IT_FE;
	
    if(hdma->XferHalfCpltCallback != NULL)
    {
      hdma->Instance->CR  |= DMA_IT_HT;
    }
    
    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(hdma);
  }
  else
  {
    /* Process unlocked */
    __HAL_UNLOCK(hdma);	  
    
    /* Return error status */
    status = HAL_BUSY;
  }
  
  return status;
}

/**
  * @brief  Sets the DMA Transfer parameter.
  * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
static void DMA_Config(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
  /* Configure DMA Stream data length */
  hdma->Instance->NDTR = DataLength;

  /* Peripheral to Memory */
  if ((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {
    /* Configure DMA Stream destination address */
    hdma->Instance->PAR = DstAddress;

    /* Configure DMA Stream source address */
    hdma->Instance->M0AR = SrcAddress;
  }
	
  /* Memory to Peripheral */
  else
  {
    /* Configure DMA Stream source address */
    hdma->Instance->PAR = SrcAddress;

    /* Configure DMA Stream destination address */
    hdma->Instance->M0AR = DstAddress;
  }
}

/**
  * @brief  DMA UART transmit process complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
	  uint32_t tickstart = 0;
	
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  
	/* DMA Normal mode*/
  if ((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
  {
    huart->TxXferCount = 0;

    /* Disable the DMA transfer for transmit request by setting the DMAT bit
       in the UART CR3 register */
    huart->Instance->CR3 &= (uint32_t)~((uint32_t)USART_CR3_DMAT);

		/* Init tickstart for timeout managment */
    tickstart = HAL_GetTick();
		
    /* Wait for UART TC Flag */
    if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, tickstart, UART_TIMEOUT_VALUE) != HAL_OK)
    {
      /* Timeout occurred */
      huart->gState = HAL_UART_STATE_TIMEOUT;
      HAL_UART_ErrorCallback(huart);
    }
    else
    {
      /* No Timeout */
      /* Check if a receive process is ongoing or not */
      if (huart->gState == HAL_UART_STATE_BUSY_TX_RX)
      {
        huart->gState = HAL_UART_STATE_BUSY_RX;
      }
      else
      {
        huart->gState = HAL_UART_STATE_READY;
      }
      HAL_UART_TxCpltCallback(huart);
    }
  }
  /* DMA Circular mode */
  else
  {
    HAL_UART_TxCpltCallback(huart);
  }
}

/**
  * @brief  DMA I2C process complete callback.
  * @param  hdma DMA handle
  * @retval None
  */
static void I2C_DMAXferCplt(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
  uint32_t CurrentState = hi2c->State;
  uint32_t CurrentMode  = hi2c->Mode;

  if((CurrentState == HAL_I2C_STATE_BUSY_TX) || ((CurrentState == HAL_I2C_STATE_BUSY_RX) && (CurrentMode == HAL_I2C_MODE_SLAVE))) 
  {
    /* Disable DMA Request */
    hi2c->Instance->CR2 &= ~I2C_CR2_DMAEN;
    
    hi2c->XferCount = 0U;
    
    /* Enable EVT and ERR interrupt */
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_ERR);
  }
  else
  {
    /* Disable Acknowledge */
    hi2c->Instance->CR1 &= ~I2C_CR1_ACK;
    
    /* Generate Stop */
    hi2c->Instance->CR1 |= I2C_CR1_STOP;
    
    /* Disable Last DMA */
    hi2c->Instance->CR2 &= ~I2C_CR2_LAST;
    
    /* Disable DMA Request */
    hi2c->Instance->CR2 &= ~I2C_CR2_DMAEN;
    
    hi2c->XferCount = 0U;

    /* Check if Errors has been detected during transfer */
    if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
    {
      HAL_I2C_ErrorCallback(hi2c);
    }
    else
    {
      hi2c->State = HAL_I2C_STATE_READY;

      if(hi2c->Mode == HAL_I2C_MODE_MEM)
      {
        hi2c->Mode = HAL_I2C_MODE_NONE;

        HAL_I2C_MemRxCpltCallback(hi2c);
      }
      else
      {
        hi2c->Mode = HAL_I2C_MODE_NONE;

        HAL_I2C_MasterRxCpltCallback(hi2c);
      }
    }
  }
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of TXE flag.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnTXEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{    
  while(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == RESET)
  {
    /* Check if a NACK is detected */
    if(I2C_IsAcknowledgeFailed(hi2c) != HAL_OK)
    {
      return HAL_ERROR;
    }
		
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U) || ((HAL_GetTick()-Tickstart) > Timeout))
      {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State= HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;      
}

/**
  * @brief  This function handles Acknowledge failed detection during an I2C Communication.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c)
{
  if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
  {
    /* Clear NACKF Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    hi2c->ErrorCode = HAL_I2C_ERROR_AF;
    hi2c->PreviousState = I2C_STATE_NONE;
    hi2c->State= HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_ERROR;
  }
  return HAL_OK;
}

/**
  * @brief  DMA UART communication error callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void UART_DMAError(DMA_HandleTypeDef *hdma)
{
  uint32_t dmarequest = 0x00U;
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* Stop UART DMA Tx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT);
  if((huart->gState == HAL_UART_STATE_BUSY_TX) && dmarequest)
  {
    huart->TxXferCount = 0U;
    UART_EndTxTransfer(huart);
  }

  /* Stop UART DMA Rx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR); 
  if((huart->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
  {
    huart->RxXferCount = 0U;
    UART_EndRxTransfer(huart);
  }

  huart->ErrorCode |= HAL_UART_ERROR_DMA;
  HAL_UART_ErrorCallback(huart);
}

/**
  * @brief  End ongoing Tx transfer on UART peripheral (following error detection or Transmit completion).
  * @param  huart: UART handle.
  * @retval None
  */
static void UART_EndTxTransfer(UART_HandleTypeDef *huart)
{
  /* Disable TXEIE and TCIE interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

  /* At end of Tx process, restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;
}

/**
  * @brief  End ongoing Rx transfer on UART peripheral (following error detection or Reception completion).
  * @param  huart: UART handle.
  * @retval None
  */
static void UART_EndRxTransfer(UART_HandleTypeDef *huart)
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* At end of Rx process, restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
}
/**
  * @brief  DMA I2C communication error callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void I2C_DMAError(DMA_HandleTypeDef *hdma)
{
  I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;

  /* Disable Acknowledge */
  hi2c->Instance->CR1 &= ~I2C_CR1_ACK;
  hi2c->XferCount = 0;
  hi2c->State = HAL_I2C_STATE_READY;
  hi2c->ErrorCode |= HAL_I2C_ERROR_DMA;
  HAL_I2C_ErrorCallback(hi2c);
}
/**
  * @brief  This function handles UART Communication Timeout.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  Flag: specifies the UART flag to check.
  * @param  Status: The new Flag status (SET or RESET).
  * @param  Tickstart Tick start value
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout)
{
  /* Wait until flag is set */
  while((__HAL_UART_GET_FLAG(huart, Flag) ? SET : RESET) == Status) 
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout))
      {
        /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
        CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE));
        CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
        
        huart->gState  = HAL_UART_STATE_READY;
        huart->RxState = HAL_UART_STATE_READY;
        
        /* Process Unlocked */
        __HAL_UNLOCK(huart);
        
        return HAL_TIMEOUT;
      }
    }
  }
  
  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  Flag specifies the I2C flag to check.
  * @param  Status The new Flag status (SET or RESET).
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart)
{
  /* Wait until flag is set */
  while((__HAL_I2C_GET_FLAG(hi2c, Flag) ? SET : RESET) == Status) 
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout))
      {
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State= HAL_I2C_STATE_READY;
        
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        
        return HAL_TIMEOUT;
      }
    }
  }
  
  return HAL_OK;
}

/**
  * @brief  UART sends data via DMA
  * @param  None
  * @retval None
  */
static void DMA_Transmit(uint8_t *pData, uint16_t Size, uint32_t TC_IT_Status)
{
  UART_Transmit_DMA(&UARTHandle, pData, Size, TC_IT_Status );
  if (TC_IT_Status == TC_IT_ENABLE)
  {
    /***** Entering in sleep mode *****/
#ifdef DEBUG
    HAL_EnableDBGSleepMode();
#endif
    SleepMode1();
  }
}

/**
  * @brief  Handles DMA interrupt request.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Stream.
  * @retval None
  */
static void DMA_IRQHandler(DMA_HandleTypeDef *hdma)
{
  /* Transfer Complete Interrupt management ***********************************/
  if (((hdma->Instance->CR) & (uint32_t)(DMA_SxCR_DBM)) != 0)
  {
    /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));

    /* Current memory buffer used is Memory 1 */
    if ((hdma->Instance->CR & DMA_SxCR_CT) == 0)
    {
      if (hdma->XferM1CpltCallback != NULL)
      {
        /* Transfer complete Callback for memory1 */
        hdma->XferM1CpltCallback(hdma);
      }
    }
    /* Current memory buffer used is Memory 0 */
    else if ((hdma->Instance->CR & DMA_SxCR_CT) != 0)
    {
      if (hdma->XferCpltCallback != NULL)
      {
        /* Transfer complete Callback for memory0 */
        hdma->XferCpltCallback(hdma);
      }
    }
  }
  /* Disable the transfer complete interrupt if the DMA mode is not CIRCULAR */
  else
  {
    if ((hdma->Instance->CR & DMA_SxCR_CIRC) == 0)
    {
      /* Disable the transfer complete interrupt */
      __HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
    }
    /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));

    /* Update error code */
    hdma->ErrorCode |= HAL_DMA_ERROR_NONE;

    /* Change the DMA state */
    hdma->State = HAL_DMA_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    if (hdma->XferCpltCallback != NULL)
    {
      /* Transfer complete callback */
      hdma->XferCpltCallback(hdma);
    }
  }
}

/**
  * @brief  Configures EXTI Line3 (connected to PA3 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void EXTILine3_Config(void)
{
  /* Enable GPIOA clock */
  __GPIOA_CLK_ENABLE();

  /* Disable USART2 clock */
  __USART2_CLK_DISABLE();

  /* Configure PA3 pin as External Interrupt Mode
    with Rising/Falling edge trigger detection */
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING ;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Enable and set EXTI Line3 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

/**
  * @brief  DMA configuration for reception from I2C
  * @param  None
  * @retval None
  */
static void DMA_I2C_Config(void)
{
	static DMA_HandleTypeDef hdma_rx;
	
  /* Configure the DMA streams */
  /* Configure the DMA handler for Reception process */
  hdma_rx.Instance                 = I2C1_RX_DMA_STREAM;
  hdma_rx.Init.Channel             = I2C1_RX_DMA_CHANNEL;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_NORMAL;
  hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst            = DMA_MBURST_SINGLE;
  hdma_rx.Init.PeriphBurst         = DMA_MBURST_SINGLE;

  HAL_DMA_Init(&hdma_rx);

  /* Associate the initialized DMA handle to the I2C handle */
  __HAL_LINKDMA(&I2CHandle, hdmarx, hdma_rx);

  /* NVIC configuration for DMA transfer complete interrupt (I2C1_RX) */
  HAL_NVIC_SetPriority(I2C1_DMA_RX_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C1_DMA_RX_IRQn);
}

/**
  * @brief  Reads a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.
  * @param  Reg: The target register address to write
  * @retval Data read at register address
  */
static uint8_t I2C1_ReadData(uint16_t Addr, uint8_t Reg)
{

  /*** Display Time Stamp ***/
  TimeStampDisplay(aShowTime);
  sprintf((char*)aEnterSleep5, "%s: Enter to SLEEP5", aShowTime);
	
  I2C_Mem_Read_DMA(&I2C_SHIELDS_Handle, Addr, Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t *)aRxBuffer, RXBUFFERSIZE, TC_IT_ENABLE);
  
  while (DMA_TC == 0)
  {
    /***** Entering in sleep mode *****/
#ifdef DEBUG
    HAL_EnableDBGSleepMode();
#endif
    SleepMode1();
  }

  /*** Display Time Stamp ***/
  TimeStampDisplay(aShowTime);
  sprintf((char*)aExitSleep5, "%s: Wakeup from SLEEP5", aShowTime);

  /* Change the DMA state */
  I2CHandle.hdmarx->State = HAL_DMA_STATE_READY;
  /* Process Unlocked */
  __HAL_UNLOCK(I2CHandle.hdmarx);
  I2CHandle.hdmarx->XferCpltCallback(I2CHandle.hdmarx);

  DMA_TC = 0;

  return *(uint8_t *)aRxBuffer;
}

/**
  * @brief  Configures EXTI Line5 (connected to PB5 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void EXTILine5_Config(void)
{
  /* Enable GPIOA clock */
  __GPIOB_CLK_ENABLE();

  /* Configure PB5 pin as External Interrupt Mode
    with Falling edge trigger detection */
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING ;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Enable and set EXTI Line3 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/**
  * @brief  Configure the Tamper.
  * @param  None
  * @retval None
  */
static void RTC_TamperConfig(void)
{
  RTC_TamperTypeDef  stamperstructure;
  
  /*##-1- Configure the Tamper ###############################################*/
  HAL_RTCEx_DeactivateTamper(&RTCHandle, RTC_TAMPER_1);
    
  /* Use PC13 as Tamper 1 with interrupt mode */
  stamperstructure.Filter = RTC_TAMPERFILTER_DISABLE;
  stamperstructure.PinSelection = RTC_TAMPERPIN_PC13;
  stamperstructure.Tamper = RTC_TAMPER_1;
  stamperstructure.Trigger = RTC_TAMPERTRIGGER_FALLINGEDGE;
  stamperstructure.SamplingFrequency = RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV256;
  stamperstructure.PrechargeDuration = RTC_TAMPERPRECHARGEDURATION_1RTCCLK;
  stamperstructure.TamperPullUp = RTC_TAMPER_PULLUP_DISABLE;
  stamperstructure.TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_DISABLE;
  
  if(HAL_RTCEx_SetTamper_IT(&RTCHandle, &stamperstructure) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }
  
  /* Clear the Tamper interrupt pending bit */
  __HAL_RTC_TAMPER_CLEAR_FLAG(&RTCHandle,RTC_FLAG_TAMP1F);
}

/**
 * @brief  Read data from LSM6DS0 Accelerometer and calculate linear acceleration in mg
 * @param  pData the pointer where the accelerometer data are stored
 * @retval None
 */
static void GetAxes(int16_t *pData)
{
    int16_t pDataRaw[3];
    float sensitivity = 0.061;

    GetAxesRaw(pDataRaw);
	
    pData[0] = (int32_t)(pDataRaw[0] * sensitivity);
    pData[1] = (int32_t)(pDataRaw[1] * sensitivity);
    pData[2] = (int32_t)(pDataRaw[2] * sensitivity);
}

/**
 * @brief  Read raw data from LSM6DS0 Accelerometer output register
 * @param  pData the pointer where the accelerometer raw data are stored
 * @retval None
 */
static void GetAxesRaw(int16_t *Data)
{
 I2C1_ReadData(LSM6DS0_XG_MEMS_ADDRESS, LSM6DS0_XG_OUT_X_L_XL | LSM6DS0_I2C_MULTIPLEBYTE_CMD);

 Data[0] = ((((int16_t)aRxBuffer[1]) << 8)+(int16_t)aRxBuffer[0]);
 Data[1] = ((((int16_t)aRxBuffer[3]) << 8)+(int16_t)aRxBuffer[2]);
 Data[2] = ((((int16_t)aRxBuffer[5]) << 8)+(int16_t)aRxBuffer[4]);
}

/**
 * @brief  Configures LSM6DS0
 * @param  None
 * @retval None
 */
static void LSM6DS0Config(void)
{
	uint8_t ctrl = 0;
  /* Linear acceleration sensor interrupt generator configuration */
  ctrl = 0x0A;        
  LSM6DS0_IO_Write( &ctrl, LSM6DS0_XG_MEMS_ADDRESS, LSM6DS0_INT_GEN_CFG_XL, 1 );
		
  /* Accelerometer X-axis interrupt threshold config */
  ctrl = 0x25;
  LSM6DS0_IO_Write( &ctrl, LSM6DS0_XG_MEMS_ADDRESS, LSM6DS0_INT_GEN_THS_X_XL, 1 );
      
  /* Accelerometer Y-axis interrupt threshold config */
  ctrl = 0x25;
  LSM6DS0_IO_Write( &ctrl, LSM6DS0_XG_MEMS_ADDRESS, LSM6DS0_INT_GEN_THS_Y_XL, 1 );
      
  /* Accelerometer Z-axis interrupt threshold config */
  ctrl = 0x25;
  LSM6DS0_IO_Write( &ctrl, LSM6DS0_XG_MEMS_ADDRESS, LSM6DS0_INT_GEN_THS_Z_XL, 1 );

  /* Accelerometer interrupt duration config */
  ctrl = 0x00;
  LSM6DS0_IO_Write( &ctrl, LSM6DS0_XG_MEMS_ADDRESS, LSM6DS0_INT_GEN_DUR_XL, 1 );
      
  /* Accelerometer interrupt generator on INT pin */
  ctrl = 0x40;
  LSM6DS0_IO_Write( &ctrl, LSM6DS0_XG_MEMS_ADDRESS, LSM6DS0_XG_INT_CTRL, 1 );
      
  /* Linear acceleration sensor control config */
  ctrl = 0x60;
  LSM6DS0_IO_Write( &ctrl, LSM6DS0_XG_MEMS_ADDRESS, LSM6DS0_CTRL_REG6_XL, 1 );    
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{

  HAL_UART_Transmit(&UARTHandle, (uint8_t *)&ch, 1, 5000);

  return ch;
}

GETCHAR_PROTOTYPE
{
  while (HAL_UART_Receive(&UARTHandle, (uint8_t *)&Ch, 1, 5000) != HAL_OK)
  {}

  HAL_UART_Receive(&UARTHandle, (uint8_t *)&Ch, 1, 500);

  return Ch;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while (1)
  {}
}

/**
  * @brief SYSTICK callback
  * @param None
  * @retval None
  */
void HAL_SYSTICK_Callback(void)
{
  HAL_IncTick();
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
