/**
 *      __            ____
 *     / /__ _  __   / __/                      __  
 *    / //_/(_)/ /_ / /  ___   ____ ___  __ __ / /_ 
 *   / ,<  / // __/_\ \ / _ \ / __// _ \/ // // __/ 
 *  /_/|_|/_/ \__//___// .__//_/   \___/\_,_/ \__/  
 *                    /_/   github.com/KitSprout    
 * 
 *  @file    boardConfig.h
 *  @author  KitSprout
 *  @date    17-Oct-2017
 *  @brief   
 * 
 */

/* Define to prevent recursive inclusion ---------------------------------------------------*/
#ifndef __BOARDCONFIG_H
#define __BOARDCONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------*/
/* Exported types --------------------------------------------------------------------------*/
/* Exported constants ----------------------------------------------------------------------*/

#define KS_HW_BOARD_NAME              "KSDK"
#define KS_HW_MCU_NAME                "STM32F411CE"

#define KS_HW_USE_CLOCK_SOUCE_HSI     (0U)
#define KS_HW_USE_CLOCK_SOUCE_HSE     (1U)
#define KS_HW_CLOCK_SOUCE             KS_HW_USE_CLOCK_SOUCE_HSE

#define KS_FW_UART_HAL_LIBRARY        (0U)
#define KS_FW_SPI_HAL_LIBRARY         (0U)
#define KS_FW_I2C_HAL_LIBRARY         (0U)
#define KS_FW_USB_ENABLE              (0U)

#define KS_SYSCLK                     SystemCoreClock


/* -------- LED and KEY */

#define LED_R_PIN                     GPIO_PIN_13
#define LED_R_GPIO_PORT               GPIOC
#define LED_R_Set()                   __GPIO_SET(LED_R_GPIO_PORT, LED_R_PIN)
#define LED_R_Reset()                 __GPIO_RST(LED_R_GPIO_PORT, LED_R_PIN)
#define LED_R_Toggle()                __GPIO_TOG(LED_R_GPIO_PORT, LED_R_PIN)

#define LED_G_PIN                     GPIO_PIN_14
#define LED_G_GPIO_PORT               GPIOC
#define LED_G_Set()                   __GPIO_SET(LED_G_GPIO_PORT, LED_G_PIN)
#define LED_G_Reset()                 __GPIO_RST(LED_G_GPIO_PORT, LED_G_PIN)
#define LED_G_Toggle()                __GPIO_TOG(LED_G_GPIO_PORT, LED_G_PIN)

#define LED_B_PIN                     GPIO_PIN_15
#define LED_B_GPIO_PORT               GPIOC
#define LED_B_Set()                   __GPIO_SET(LED_B_GPIO_PORT, LED_B_PIN)
#define LED_B_Reset()                 __GPIO_RST(LED_B_GPIO_PORT, LED_B_PIN)
#define LED_B_Toggle()                __GPIO_TOG(LED_B_GPIO_PORT, LED_B_PIN)

#define KEY_PIN                       GPIO_PIN_8
#define KEY_GPIO_PORT                 GPIOA
#define KEY_Read()                    (__GPIO_READ(KEY_GPIO_PORT, KEY_PIN) == KEY_PIN)
#define KEY_EXTIx_IRQ                 EXTI9_5_IRQn
#define KEY_EXTIx_IRQn_PREEMPT        0x0F
#define KEY_EXTIx_IRQn_SUB            1

/* Exported functions ----------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif

/*************************************** END OF FILE ****************************************/
