;******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
;* File Name          : startup_stm32f411xe_ram.s
;* Author             : MCD Application Team
;* Version            : V1.2.0
;* Date               : 26-August-2016
;* Description        : STM32F411xExx devices vector table for MDK-ARM toolchain. 
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == Reset_Handler
;*                      - Set the vector table entries with the exceptions ISR address
;*                      - Branches to __main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the CortexM4 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;* <<< Use Configuration Wizard in Context Menu >>>   
;*******************************************************************************
; 
;* Redistribution and use in source and binary forms, with or without modification,
;* are permitted provided that the following conditions are met:
;*   1. Redistributions of source code must retain the above copyright notice,
;*      this list of conditions and the following disclaimer.
;*   2. Redistributions in binary form must reproduce the above copyright notice,
;*      this list of conditions and the following disclaimer in the documentation
;*      and/or other materials provided with the distribution.
;*   3. Neither the name of STMicroelectronics nor the names of its contributors
;*      may be used to endorse or promote products derived from this software
;*      without specific prior written permission.
;*
;* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
;* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
;* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
;* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
;* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
;*******************************************************************************

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET_ram, DATA, READONLY
                EXPORT  __Vectors_ram
                EXPORT  __Vectors_End_ram
                EXPORT  __Vectors_Size_ram

__Vectors_ram  DCD     0                          ; Top of Stack
                DCD     0                          ; Reset Handler
                DCD     NMI_Handler                ; NMI Handler
                DCD     HardFault_Handler          ; Hard Fault Handler
                DCD     MemManage_Handler          ; MPU Fault Handler
                DCD     BusFault_Handler           ; Bus Fault Handler
                DCD     UsageFault_Handler         ; Usage Fault Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     SVC_Handler                ; SVCall Handler
                DCD     DebugMon_Handler           ; Debug Monitor Handler
                DCD     0                          ; Reserved
                DCD     PendSV_Handler             ; PendSV Handler
                DCD     SysTick_Handler            ; SysTick Handler

                ; External Interrupts
                DCD     WWDG_IRQHandler                   ; Window WatchDog                                        
                DCD     PVD_IRQHandler                    ; PVD through EXTI Line detection                        
                DCD     TAMP_STAMP_IRQHandler             ; Tamper and TimeStamps through the EXTI line            
                DCD     RTC_WKUP_IRQHandler               ; RTC Wakeup through the EXTI line                       
                DCD     FLASH_IRQHandler                  ; FLASH                                           
                DCD     RCC_IRQHandler                    ; RCC                                             
                DCD     EXTI0_IRQHandler                  ; EXTI Line0                                             
                DCD     EXTI1_IRQHandler                  ; EXTI Line1                                             
                DCD     EXTI2_IRQHandler                  ; EXTI Line2                                             
                DCD     EXTI3_IRQHandler                  ; EXTI Line3                                             
                DCD     EXTI4_IRQHandler                  ; EXTI Line4                                             
                DCD     DMA1_Stream0_IRQHandler           ; DMA1 Stream 0                                   
                DCD     DMA1_Stream1_IRQHandler           ; DMA1 Stream 1                                   
                DCD     DMA1_Stream2_IRQHandler           ; DMA1 Stream 2                                   
                DCD     DMA1_Stream3_IRQHandler           ; DMA1 Stream 3                                   
                DCD     DMA1_Stream4_IRQHandler           ; DMA1 Stream 4                                   
                DCD     DMA1_Stream5_IRQHandler           ; DMA1 Stream 5                                   
                DCD     DMA1_Stream6_IRQHandler           ; DMA1 Stream 6                                   
                DCD     ADC_IRQHandler                    ; ADC1, ADC2 and ADC3s                            
                DCD     0                                 ; Reserved                                                
                DCD     0                                 ; Reserved                                               
                DCD     0                                 ; Reserved                                             
                DCD     0                                 ; Reserved                                               
                DCD     EXTI9_5_IRQHandler                ; External Line[9:5]s                                    
                DCD     TIM1_BRK_TIM9_IRQHandler          ; TIM1 Break and TIM9                   
                DCD     TIM1_UP_TIM10_IRQHandler          ; TIM1 Update and TIM10                 
                DCD     TIM1_TRG_COM_TIM11_IRQHandler     ; TIM1 Trigger and Commutation and TIM11
                DCD     TIM1_CC_IRQHandler                ; TIM1 Capture Compare                                   
                DCD     TIM2_IRQHandler                   ; TIM2                                            
                DCD     TIM3_IRQHandler                   ; TIM3                                            
                DCD     TIM4_IRQHandler                   ; TIM4                                            
                DCD     I2C1_EV_IRQHandler                ; I2C1 Event                                             
                DCD     I2C1_ER_IRQHandler                ; I2C1 Error                                             
                DCD     I2C2_EV_IRQHandler                ; I2C2 Event                                             
                DCD     I2C2_ER_IRQHandler                ; I2C2 Error                                               
                DCD     SPI1_IRQHandler                   ; SPI1                                            
                DCD     SPI2_IRQHandler                   ; SPI2                                            
                DCD     USART1_IRQHandler                 ; USART1                                          
                DCD     USART2_IRQHandler                 ; USART2                                          
                DCD     0                                 ; Reserved                                          
                DCD     EXTI15_10_IRQHandler              ; External Line[15:10]s                                  
                DCD     RTC_Alarm_IRQHandler              ; RTC Alarm (A and B) through EXTI Line                  
                DCD     OTG_FS_WKUP_IRQHandler            ; USB OTG FS Wakeup through EXTI line                        
                DCD     0                                 ; Reserved                  
                DCD     0                                 ; Reserved                 
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved                                   
                DCD     DMA1_Stream7_IRQHandler           ; DMA1 Stream7                                           
                DCD     0                                 ; Reserved                                             
                DCD     SDIO_IRQHandler                   ; SDIO                                            
                DCD     TIM5_IRQHandler                   ; TIM5                                            
                DCD     SPI3_IRQHandler                   ; SPI3                                            
                DCD     0                                 ; Reserved                                           
                DCD     0                                 ; Reserved                                           
                DCD     0                                 ; Reserved                   
                DCD     0                                 ; Reserved                   
                DCD     DMA2_Stream0_IRQHandler           ; DMA2 Stream 0                                   
                DCD     DMA2_Stream1_IRQHandler           ; DMA2 Stream 1                                   
                DCD     DMA2_Stream2_IRQHandler           ; DMA2 Stream 2                                   
                DCD     DMA2_Stream3_IRQHandler           ; DMA2 Stream 3                                   
                DCD     DMA2_Stream4_IRQHandler           ; DMA2 Stream 4
                DCD     0                                 ; Reserved  
                DCD     0                                 ; Reserved  
                DCD     0                                 ; Reserved                                              
                DCD     0                                 ; Reserved                                               
                DCD     0                                 ; Reserved                                               
                DCD     0                                 ; Reserved                                               
                DCD     OTG_FS_IRQHandler                 ; USB OTG FS                                      
                DCD     DMA2_Stream5_IRQHandler           ; DMA2 Stream 5                                   
                DCD     DMA2_Stream6_IRQHandler           ; DMA2 Stream 6                                   
                DCD     DMA2_Stream7_IRQHandler           ; DMA2 Stream 7                                   
                DCD     USART6_IRQHandler                 ; USART6                                           
                DCD     I2C3_EV_IRQHandler                ; I2C3 event                                             
                DCD     I2C3_ER_IRQHandler                ; I2C3 error                                             
                DCD     0                                 ; Reserved                     
                DCD     0                                 ; Reserved                       
                DCD     0                                 ; Reserved                         
                DCD     0                                 ; Reserved                                    
                DCD     0                                 ; Reserved  
                DCD     0                                 ; Reserved				                              
                DCD     0                                 ; Reserved
                DCD     FPU_IRQHandler                    ; FPU
                DCD     0                                 ; Reserved
		        DCD     0                                 ; Reserved
		        DCD     SPI4_IRQHandler                   ; SPI4
				DCD     SPI5_IRQHandler                   ; SPI5
                                         
__Vectors_End_ram

__Vectors_Size_ram  EQU  __Vectors_End_ram - __Vectors_ram

                AREA    |.text|, CODE, READONLY
					
                IMPORT     NMI_Handler                ; NMI Handler
                IMPORT     HardFault_Handler          ; Hard Fault Handler
                IMPORT     MemManage_Handler          ; MPU Fault Handler
                IMPORT     BusFault_Handler           ; Bus Fault Handler
                IMPORT     UsageFault_Handler         ; Usage Fault Handler
                IMPORT     SVC_Handler                ; SVCall Handler
                IMPORT     DebugMon_Handler           ; Debug Monitor Handler
                IMPORT     PendSV_Handler             ; PendSV Handler
                IMPORT     SysTick_Handler            ; SysTick Handler

                IMPORT  WWDG_IRQHandler                   [WEAK]                                        
                IMPORT  PVD_IRQHandler                    [WEAK]                      
                IMPORT  TAMP_STAMP_IRQHandler             [WEAK]         
                IMPORT  RTC_WKUP_IRQHandler               [WEAK]                     
                IMPORT  FLASH_IRQHandler                  [WEAK]                                         
                IMPORT  RCC_IRQHandler                    [WEAK]                                            
                IMPORT  EXTI0_IRQHandler                  [WEAK]                                            
                IMPORT  EXTI1_IRQHandler                  [WEAK]                                             
                IMPORT  EXTI2_IRQHandler                  [WEAK]                                            
                IMPORT  EXTI3_IRQHandler                  [WEAK]                                           
                IMPORT  EXTI4_IRQHandler                  [WEAK]                                            
                IMPORT  DMA1_Stream0_IRQHandler           [WEAK]                                
                IMPORT  DMA1_Stream1_IRQHandler           [WEAK]                                   
                IMPORT  DMA1_Stream2_IRQHandler           [WEAK]                                   
                IMPORT  DMA1_Stream3_IRQHandler           [WEAK]                                   
                IMPORT  DMA1_Stream4_IRQHandler           [WEAK]                                   
                IMPORT  DMA1_Stream5_IRQHandler           [WEAK]                                   
                IMPORT  DMA1_Stream6_IRQHandler           [WEAK]                                   
                IMPORT  ADC_IRQHandler                    [WEAK]                                                                        
                IMPORT  EXTI9_5_IRQHandler                [WEAK]                                    
                IMPORT  TIM1_BRK_TIM9_IRQHandler          [WEAK]                  
                IMPORT  TIM1_UP_TIM10_IRQHandler          [WEAK]                
                IMPORT  TIM1_TRG_COM_TIM11_IRQHandler     [WEAK] 
                IMPORT  TIM1_CC_IRQHandler                [WEAK]                                   
                IMPORT  TIM2_IRQHandler                   [WEAK]                                            
                IMPORT  TIM3_IRQHandler                   [WEAK]                                            
                IMPORT  TIM4_IRQHandler                   [WEAK]                                            
                IMPORT  I2C1_EV_IRQHandler                [WEAK]                                             
                IMPORT  I2C1_ER_IRQHandler                [WEAK]                                             
                IMPORT  I2C2_EV_IRQHandler                [WEAK]                                            
                IMPORT  I2C2_ER_IRQHandler                [WEAK]                                               
                IMPORT  SPI1_IRQHandler                   [WEAK]                                           
                IMPORT  SPI2_IRQHandler                   [WEAK]                                            
                IMPORT  USART1_IRQHandler                 [WEAK]                                          
                IMPORT  USART2_IRQHandler                 [WEAK]                                                                                  
                IMPORT  EXTI15_10_IRQHandler              [WEAK]                                  
                IMPORT  RTC_Alarm_IRQHandler              [WEAK]                  
                IMPORT  OTG_FS_WKUP_IRQHandler            [WEAK]                        
                IMPORT  DMA1_Stream7_IRQHandler           [WEAK]                                                                                     
                IMPORT  SDIO_IRQHandler                   [WEAK]                                             
                IMPORT  TIM5_IRQHandler                   [WEAK]                                             
                IMPORT  SPI3_IRQHandler                   [WEAK]                                                               
                IMPORT  DMA2_Stream0_IRQHandler           [WEAK]                                  
                IMPORT  DMA2_Stream1_IRQHandler           [WEAK]                                   
                IMPORT  DMA2_Stream2_IRQHandler           [WEAK]                                    
                IMPORT  DMA2_Stream3_IRQHandler           [WEAK]                                    
                IMPORT  DMA2_Stream4_IRQHandler           [WEAK]                                                                                                     
                IMPORT  OTG_FS_IRQHandler                 [WEAK]                                       
                IMPORT  DMA2_Stream5_IRQHandler           [WEAK]                                   
                IMPORT  DMA2_Stream6_IRQHandler           [WEAK]                                   
                IMPORT  DMA2_Stream7_IRQHandler           [WEAK]                                   
                IMPORT  USART6_IRQHandler                 [WEAK]                                           
                IMPORT  I2C3_EV_IRQHandler                [WEAK]                                              
                IMPORT  I2C3_ER_IRQHandler                [WEAK]                                              
                IMPORT  FPU_IRQHandler                    [WEAK]
				IMPORT  SPI4_IRQHandler                   [WEAK]
                IMPORT  SPI5_IRQHandler                   [WEAK]

	END
;************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE*****
