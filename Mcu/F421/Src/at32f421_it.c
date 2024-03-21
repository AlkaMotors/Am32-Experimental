/* Includes
 * ------------------------------------------------------------------*/
#include "at32f421_it.h"

#include "adc.h"
#include "main.h"
#include "targets.h"
#include "crsf.h"
#include "peripherals.h"
#include "common.h"

extern void transfercomplete();
extern void PeriodElapsedCallback();
extern void interruptRoutine();
extern void doPWMChanges();
extern void tenKhzRoutine();
extern void sendDshotDma();
extern void receiveDshotDma();
extern void signalEdgeRoutine();
extern void processDshot();

extern char send_telemetry;
extern char telemetry_done;
extern char servoPwm;
extern char dshot;

int exti_int = 0;
int dma_int = 0;


/** @addtogroup AT32F421_StdPeriph_Templates
 * @{
 */

/** @addtogroup GPIO_LED_Toggle
 * @{
 */

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSV_Handler exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
}

void DMA1_Channel1_IRQHandler(void)
{
    if (dma_flag_get(DMA1_FDT1_FLAG) == SET) {
        DMA1->clr = DMA1_GL1_FLAG;
#ifdef USE_ADC
        ADC_DMA_Callback();
#endif
        if (dma_flag_get(DMA1_DTERR1_FLAG) == SET) {
            DMA1->clr = DMA1_GL1_FLAG;
        }
    }
}

void DMA1_Channel3_2_IRQHandler(void)
{
	
    if (dma_flag_get(DMA1_FDT2_FLAG) == SET) {
        DMA1->clr = DMA1_GL2_FLAG;
        DMA1_CHANNEL2->ctrl_bit.chen = FALSE;
    }
    if (dma_flag_get(DMA1_DTERR2_FLAG) == SET) {
        DMA1->clr = DMA1_GL2_FLAG;
        DMA1_CHANNEL2->ctrl_bit.chen = FALSE;
    }	
}

void DMA1_Channel5_4_IRQHandler(void)
{
	dma_int++;
#ifdef USE_TIMER_15_CHANNEL_1
    if (dshot) {
        DMA1->clr = DMA1_GL5_FLAG;
        INPUT_DMA_CHANNEL->ctrl_bit.chen = FALSE;
        transfercomplete();
        EXINT->swtrg = EXINT_LINE_15;
        return;
    }
    if (dma_flag_get(DMA1_HDT5_FLAG) == SET) {
        if (servoPwm) {
            IC_TIMER_REGISTER->cctrl_bit.c1p = TMR_INPUT_FALLING_EDGE;
            DMA1->clr = DMA1_HDT5_FLAG;
        }
    }

    if (dma_flag_get(DMA1_FDT5_FLAG) == SET) {
       DMA1->clr = DMA1_GL5_FLAG;
       INPUT_DMA_CHANNEL->ctrl_bit.chen = FALSE;
        transfercomplete();
        EXINT->swtrg = EXINT_LINE_15;
    }
    if (dma_flag_get(DMA1_DTERR5_FLAG) == SET) {
        DMA1->clr = DMA1_GL5_FLAG;
    }
#endif
#ifdef USE_TIMER_3_CHANNEL_1
    if (dshot) {
        DMA1->clr = DMA1_GL4_FLAG;
        INPUT_DMA_CHANNEL->ctrl_bit.chen = FALSE;
        transfercomplete();
        EXINT->swtrg = EXINT_LINE_15;
        return;
    }
    if (dma_flag_get(DMA1_HDT4_FLAG) == SET) {
        if (servoPwm) {
            IC_TIMER_REGISTER->cctrl_bit.c1p = TMR_INPUT_FALLING_EDGE;
            DMA1->clr = DMA1_HDT4_FLAG;
        }
    }
    if (dma_flag_get(DMA1_FDT4_FLAG) == SET) {
        DMA1->clr = DMA1_GL4_FLAG;
        INPUT_DMA_CHANNEL->ctrl_bit.chen = FALSE;
        transfercomplete();
        EXINT->swtrg = EXINT_LINE_15;
    }
    if (dma_flag_get(DMA1_DTERR4_FLAG) == SET) {
        DMA1->clr = DMA1_GL4_FLAG;
    }
#endif
}

/**
 * @brief This function handles ADC and COMP interrupts (COMP interrupts
 * through EXTI lines 21 and 22).
 */
void ADC1_CMP_IRQHandler(void)
{
    if ((EXINT->intsts & EXTI_LINE) != (uint32_t)RESET) {
        //	EXTI->PND = EXTI_LINE;
        EXINT->intsts = EXTI_LINE;
        interruptRoutine();
    }
}

/**
 * @brief This function handles TIM6 global and DAC underrun error interrupts.
 */
void TMR14_GLOBAL_IRQHandler(void)
{
    TMR14->ists = (uint16_t)~TMR_OVF_FLAG;
    tenKhzRoutine();
}

/**
 * @brief This function handles TIM14 global interrupt.
 */
void TMR16_GLOBAL_IRQHandler(void)
{

    TMR16->ists = 0x00;
    PeriodElapsedCallback();

    //	  }

    /* USER CODE END TIM14_IRQn 0 */
    /* USER CODE BEGIN TIM14_IRQn 1 */

    /* USER CODE END TIM14_IRQn 1 */
}

void TMR15_GLOBAL_IRQHandler(void)
{
    TMR15->ists = (uint16_t)~TMR_OVF_FLAG;
    TMR15->ists = (uint16_t)~TMR_C1_FLAG;
}

/**
 * @brief This function handles USART1 global interrupt / USART1 wake-up
 * interrupt through EXTI line 25.
 */
void USART1_IRQHandler(void)
{
  usart_flag_clear(USART1, USART_IDLEF_FLAG);
	DMA1_CHANNEL3->ctrl_bit.chen = FALSE;
	setChannels();
	receiveCRSF();	
}

void USART2_IRQHandler(void)
{
  usart_flag_clear(USART2, USART_IDLEF_FLAG);
	DMA1_CHANNEL5->ctrl_bit.chen = FALSE;
	setChannels();
	receiveCRSF();	
}

void TMR3_GLOBAL_IRQHandler(void)
{
    if ((TMR3->ists & TMR_C1_FLAG) != (uint16_t)RESET) {
        TMR3->ists = (uint16_t)~TMR_C1_FLAG;
    }
    if ((TMR3->ists & TMR_OVF_FLAG) != (uint16_t)RESET) {
        TMR3->ists = (uint16_t)~TMR_OVF_FLAG;
    }
}



void EXINT15_4_IRQHandler(void)
{
   
    if ((EXINT->intsts & EXINT_LINE_15) != (uint32_t)RESET) {
        EXINT->intsts = EXINT_LINE_15;
        processDshot();
    }
}

/******************************************************************************/
/*                 AT32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the */
/*  available peripheral interrupt handler's name please refer to the startup
 */
/*  file (startup_at32f413_xx.s).                                            */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

/**
 * @}
 */

/**
 * @}
 */
