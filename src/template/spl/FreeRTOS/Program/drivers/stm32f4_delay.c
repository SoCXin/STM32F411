
/* Includes --------------------------------------------------------------------------------*/
#include "stm32f4_system.h"
#include "stm32f4_delay.h"

/** @addtogroup STM32_Driver
 *  @{
 */

/* Private typedef -------------------------------------------------------------------------*/
/* Private define --------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------*/

/**
 *  @brief  delay_nop
 */
void delay_nop( __IO uint32_t __n )
{
  while (__n--) {
    __nop();
  }
}

/**
 *  @brief  delay_us
 */
void delay_us( __IO uint32_t __us )
{
  while (__us--) {
    __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();
    __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();
    __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();
    __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();
    __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();
    __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();
    __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();
    __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();
    __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();
    __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();  __nop();
  }
}

/*************************************** END OF FILE ****************************************/
