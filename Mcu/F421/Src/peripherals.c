/*
 * peripherals.c
 *
 *  Created on: Sep. 26, 2020
 *      Author: Alka
 */

// PERIPHERAL SETUP

#define KR_KEY_Reload ((uint16_t)0xAAAA)
#define KR_KEY_Enable ((uint16_t)0xCCCC)

#include "peripherals.h"

#include "ADC.h"
#include "common.h"
#include "functions.h"
#include "serial_telemetry.h"
#include "targets.h"
#ifdef USE_LED_STRIP
#include "WS2812.h"
#endif

void initCorePeripherals(void)
{
  //  system_clock_config();
    SCB->VTOR
        = FLASH_BASE
        | 0x1000;
    MX_GPIO_Init();
    MX_DMA_Init();
    TIM1_Init();
    TIM6_Init();
    TIM14_Init();
    AT_COMP_Init();
    TIM17_Init();
    TIM16_Init();
 //   UN_TIM_Init();
#ifdef USE_SERIAL_TELEMETRY
   // telem_UART_Init();
#endif
#ifdef USE_LED_STRIP
    WS2812_Init();
#endif
}

void initAfterJump(void)
{
    __enable_irq();
}

void system_clock_config(void)
{
    flash_psr_set(FLASH_WAIT_CYCLE_3);
    crm_reset();
    crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);
    while (crm_flag_get(CRM_HICK_STABLE_FLAG) != SET) {
    }
    crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_30);
    crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);
    while (crm_flag_get(CRM_PLL_STABLE_FLAG) != SET) {
    }
    crm_ahb_div_set(CRM_AHB_DIV_1);
    crm_apb2_div_set(CRM_APB2_DIV_1);
    crm_apb1_div_set(CRM_APB1_DIV_1);
    crm_auto_step_mode_enable(TRUE);
    crm_sysclk_switch(CRM_SCLK_PLL);
    while (crm_sysclk_switch_status_get() != CRM_SCLK_PLL) {
    }
    crm_auto_step_mode_enable(FALSE);
    system_core_clock_update();
}

void AT_COMP_Init(void)
{
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_CMP_PERIPH_CLOCK, TRUE);
    gpio_mode_QUICK(GPIOA, GPIO_MODE_ANALOG, GPIO_PULL_NONE, GPIO_PINS_1);
    gpio_mode_QUICK(GPIOA, GPIO_MODE_ANALOG, GPIO_PULL_NONE, GPIO_PINS_5);
    NVIC_SetPriority(ADC1_CMP_IRQn, 0);
    NVIC_EnableIRQ(ADC1_CMP_IRQn);
    cmp_enable(CMP1_SELECTION, TRUE);
}

void MX_IWDG_Init(void)
{
    WDT->cmd = WDT_CMD_UNLOCK;
    WDT->cmd = WDT_CMD_ENABLE;
    WDT->div = WDT_CLK_DIV_32;
    WDT->rld = 4000;
    WDT->cmd = WDT_CMD_RELOAD;
}

void TIM1_Init(void)
{
    crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
    TMR1->pr = 3000;
    TMR1->div = 0;

    TMR1->cm1 = 0x6868; // Channel 1 and 2 in PWM output mode
    TMR1->cm2 = 0x68; // channel 3 in PWM output mode
#ifdef USE_INVERTED_HIGH
    tmr_output_channel_polarity_set(TMR1, TMR_SELECT_CHANNEL_1,
        TMR_POLARITY_ACTIVE_LOW);
    tmr_output_channel_polarity_set(TMR1, TMR_SELECT_CHANNEL_2,
        TMR_POLARITY_ACTIVE_LOW);
    tmr_output_channel_polarity_set(TMR1, TMR_SELECT_CHANNEL_3,
        TMR_POLARITY_ACTIVE_LOW);
#endif

    tmr_output_channel_buffer_enable(TMR1, TMR_SELECT_CHANNEL_1, TRUE);
    tmr_output_channel_buffer_enable(TMR1, TMR_SELECT_CHANNEL_2, TRUE);
    tmr_output_channel_buffer_enable(TMR1, TMR_SELECT_CHANNEL_3, TRUE);

    tmr_period_buffer_enable(TMR1, TRUE);
    TMR1->brk_bit.dtc = DEAD_TIME;
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

    /*configure PA8/PA9/PA10(TIMER0/CH0/CH1/CH2) as alternate function*/
    gpio_mode_QUICK(PHASE_A_GPIO_PORT_LOW, GPIO_MODE_MUX, GPIO_PULL_NONE,
        PHASE_A_GPIO_LOW);

    gpio_mode_QUICK(PHASE_B_GPIO_PORT_LOW, GPIO_MODE_MUX, GPIO_PULL_NONE,
        PHASE_B_GPIO_LOW);

    gpio_mode_QUICK(PHASE_C_GPIO_PORT_LOW, GPIO_MODE_MUX, GPIO_PULL_NONE,
        PHASE_C_GPIO_LOW);

    gpio_pin_mux_config(PHASE_A_GPIO_PORT_LOW, PHASE_A_PIN_SOURCE_LOW,
        GPIO_MUX_2);
    gpio_pin_mux_config(PHASE_B_GPIO_PORT_LOW, PHASE_B_PIN_SOURCE_LOW,
        GPIO_MUX_2);
    gpio_pin_mux_config(PHASE_C_GPIO_PORT_LOW, PHASE_C_PIN_SOURCE_LOW,
        GPIO_MUX_2);

    /*configure PB13/PB14/PB15(TIMER0/CH0N/CH1N/CH2N) as alternate function*/
    gpio_mode_QUICK(PHASE_A_GPIO_PORT_HIGH, GPIO_MODE_MUX, GPIO_PULL_NONE,
        PHASE_A_GPIO_HIGH);
    //  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP,
    //  GPIO_OSPEED_50MHZ,PHASE_A_GPIO_HIGH);

    gpio_mode_QUICK(PHASE_B_GPIO_PORT_HIGH, GPIO_MODE_MUX, GPIO_PULL_NONE,
        PHASE_B_GPIO_HIGH);
    //   gpio_output_options_set(GPIOB, GPIO_OTYPE_PP,
    //   GPIO_OSPEED_50MHZ,PHASE_B_GPIO_HIGH);

    gpio_mode_QUICK(PHASE_C_GPIO_PORT_HIGH, GPIO_MODE_MUX, GPIO_PULL_NONE,
        PHASE_C_GPIO_HIGH);
    //    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP,
    //    GPIO_OSPEED_50MHZ,PHASE_C_GPIO_HIGH);

    gpio_pin_mux_config(PHASE_A_GPIO_PORT_HIGH, PHASE_A_PIN_SOURCE_HIGH,
        GPIO_MUX_2);
    gpio_pin_mux_config(PHASE_B_GPIO_PORT_HIGH, PHASE_B_PIN_SOURCE_HIGH,
        GPIO_MUX_2);
    gpio_pin_mux_config(PHASE_C_GPIO_PORT_HIGH, PHASE_C_PIN_SOURCE_HIGH,
        GPIO_MUX_2);
}

void TIM6_Init(void)
{
    crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);
    TMR6->pr = 0xFFFF;
    TMR6->div = 59;
}

void TIM14_Init(void)
{
    crm_periph_clock_enable(CRM_TMR14_PERIPH_CLOCK, TRUE);
    TMR14->pr = 1000000 / LOOP_FREQUENCY_HZ;
    TMR14->div = 119;

    NVIC_SetPriority(TMR14_GLOBAL_IRQn, 3);
    NVIC_EnableIRQ(TMR14_GLOBAL_IRQn);

    // TMR_Cmd(TMR14, ENABLE);
}

void TIM16_Init(void)
{
    crm_periph_clock_enable(CRM_TMR16_PERIPH_CLOCK, TRUE);
    TMR16->pr = 500;
    TMR16->div = 59;
    NVIC_SetPriority(TMR16_GLOBAL_IRQn, 0);
    NVIC_EnableIRQ(TMR16_GLOBAL_IRQn);
}

void TIM17_Init(void)
{
    crm_periph_clock_enable(CRM_TMR17_PERIPH_CLOCK, TRUE);
    TMR17->pr = 0xFFFF;
    TMR17->div = 59;
    TMR17->ctrl1_bit.prben = TRUE;

    // TMR_Cmd(TMR15, ENABLE);
}

void MX_DMA_Init(void)
{
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    NVIC_SetPriority(DMA1_Channel5_4_IRQn, 1);
    NVIC_EnableIRQ(DMA1_Channel5_4_IRQn);
}

void MX_GPIO_Init(void)
{
}

void UN_TIM_Init(void)
{
#ifdef USE_TIMER_3_CHANNEL_1
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);
    gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_MUX, GPIO_PULL_NONE, INPUT_PIN);
    gpio_pin_mux_config(INPUT_PIN_PORT, INPUT_PIN_SOURCE, GPIO_MUX_1);
#endif
#ifdef USE_TIMER_15_CHANNEL_1
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_TMR15_PERIPH_CLOCK, TRUE);
    gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_MUX, GPIO_PULL_NONE, INPUT_PIN);
#endif

    //	RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1,ENABLE);

    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    INPUT_DMA_CHANNEL->ctrl = 0X98a; //  PERIPHERAL HALF WORD, MEMROY WORD ,
                                     //  MEMORY INC ENABLE , TC AND ERROR INTS
    NVIC_SetPriority(IC_DMA_IRQ_NAME, 1);
    NVIC_EnableIRQ(IC_DMA_IRQ_NAME);
    IC_TIMER_REGISTER->pr = 0xFFFF;
    IC_TIMER_REGISTER->div = 16;
    IC_TIMER_REGISTER->ctrl1_bit.prben = TRUE;
    IC_TIMER_REGISTER->ctrl1_bit.tmren = TRUE;
}

#ifdef USE_RGB_LED // has 3 color led
void LED_GPIO_init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* GPIO Ports Clock Enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_8);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

#endif

void reloadWatchDogCounter()
{
    WDT->cmd = WDT_CMD_RELOAD;
}

void disableComTimerInt()
{
    COM_TIMER->iden &= ~TMR_OVF_INT;
}

void enableComTimerInt()
{
    COM_TIMER->iden |= TMR_OVF_INT;
}

void setAndEnableComInt(uint16_t time)
{
    COM_TIMER->cval = 0;
    COM_TIMER->pr = time;
    COM_TIMER->ists = 0x00;
    COM_TIMER->iden |= TMR_OVF_INT;
}

uint16_t
getintervaTimerCount()
{
    return INTERVAL_TIMER->cval;
}

void setintervaTimerCount(uint16_t intertime)
{
    INTERVAL_TIMER->cval = intertime;
}

void setPrescalerPWM(uint16_t presc)
{
    TMR1->div = presc;
}

void setAutoReloadPWM(uint16_t relval)
{
    TMR1->pr = relval;
}

void setDutyCycleAll(uint16_t newdc)
{
    TMR1->c1dt = newdc;
    TMR1->c2dt = newdc;
    TMR1->c3dt = newdc;
}

void setPWMCompare1(uint16_t compareone)
{
    TMR1->c1dt = compareone;
}
void setPWMCompare2(uint16_t comparetwo)
{
    TMR1->c2dt = comparetwo;
}
void setPWMCompare3(uint16_t comparethree)
{
    TMR1->c3dt = comparethree;
}

void generatePwmTimerEvent()
{
    TMR1->swevt |= TMR_OVERFLOW_SWTRIG;
    ;
}

void resetInputCaptureTimer()
{
    IC_TIMER_REGISTER->pr = 0;
    IC_TIMER_REGISTER->cval = 0;
}

void enableCorePeripherals()
{
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2C, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3C, TRUE);

    TMR1->ctrl1_bit.tmren = TRUE;
    TMR1->brk_bit.oen = TRUE;
    TMR1->swevt |= TMR_OVERFLOW_SWTRIG;
#ifdef USE_RGB_LED
    LED_GPIO_init();
    GPIOB->scr = LL_GPIO_PIN_8; // turn on red
    GPIOB->clr = LL_GPIO_PIN_5;
    GPIOB->clr = LL_GPIO_PIN_3; //
#endif


    COM_TIMER->ctrl1_bit.tmren = TRUE;
    COM_TIMER->swevt |= TMR_OVERFLOW_SWTRIG;
    COM_TIMER->iden &= ~TMR_OVF_INT;

    UTILITY_TIMER->ctrl1_bit.tmren = TRUE;
    INTERVAL_TIMER->ctrl1_bit.tmren = TRUE;
    INTERVAL_TIMER->swevt |= TMR_OVERFLOW_SWTRIG;
    TEN_KHZ_TIMER->ctrl1_bit.tmren = TRUE;
    TEN_KHZ_TIMER->swevt |= TMR_OVERFLOW_SWTRIG;
    TEN_KHZ_TIMER->iden |= TMR_OVF_INT;

#ifdef USE_ADC
    ADC_Init();
#endif

#ifdef USE_ADC_INPUT

#else
    tmr_channel_enable(IC_TIMER_REGISTER, IC_TIMER_CHANNEL, TRUE);
    IC_TIMER_REGISTER->ctrl1_bit.tmren = TRUE;
#endif

    NVIC_SetPriority(EXINT15_4_IRQn, 2);
    NVIC_EnableIRQ(EXINT15_4_IRQn);
    EXINT->inten |= EXINT_LINE_15;
}

void crsf_UART_Init(void)
{
	  if(serial_mode == 5){
		crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
				
		gpio_init_type gpio_init_struct;
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_PINS_6;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOB, &gpio_init_struct);
		gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE6, GPIO_MUX_0);
    
		dma_reset(DMA1_CHANNEL3);
    dma_init_type dma_init_struct;
    dma_default_para_init(&dma_init_struct);
    dma_init_struct.buffer_size = 64;
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_base_addr = (uint32_t)&crsf_buffer;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt;
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init_struct.loop_mode_enable = TRUE;
    dma_init(DMA1_CHANNEL3, &dma_init_struct);

    usart_init(USART1, 420000, USART_DATA_8BITS, USART_STOP_1_BIT);
   // usart_transmitter_enable(USART1, TRUE);
    usart_receiver_enable(USART1, TRUE);
    usart_single_line_halfduplex_select(USART1, TRUE);
    usart_dma_receiver_enable(USART1, TRUE);
    usart_enable(USART1, TRUE);
		
		NVIC_SetPriority(USART1_IRQn, 2);
    NVIC_EnableIRQ(USART1_IRQn);
		
		usart_interrupt_enable(USART1, USART_IDLE_INT, TRUE); 	
		}
	




		if(serial_mode == 6){
		crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	    gpio_init_type gpio_init_struct;
   crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);

    /* configure the usart2 tx pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_PINS_2;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOA, &gpio_init_struct);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE2, GPIO_MUX_1);

    dma_reset(DMA1_CHANNEL5);

    dma_init_type dma_init_struct;
    dma_default_para_init(&dma_init_struct);
    dma_init_struct.buffer_size = 64;
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_base_addr = (uint32_t)&crsf_buffer;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_base_addr = (uint32_t)&USART2->dt;
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init_struct.loop_mode_enable = TRUE;
    dma_init(DMA1_CHANNEL5, &dma_init_struct);

  //  DMA1_CHANNEL5->ctrl |= DMA_FDT_INT;
   // DMA1_CHANNEL3->ctrl |= DMA_DTERR_INT;

    /* configure usart1 param */
    usart_init(USART2, 420000, USART_DATA_8BITS, USART_STOP_1_BIT);
 //   usart_transmitter_enable(USART2, TRUE);
    usart_receiver_enable(USART2, TRUE);
    usart_single_line_halfduplex_select(USART2, TRUE);
    usart_dma_receiver_enable(USART2, TRUE);
    usart_enable(USART2, TRUE);
		
		NVIC_SetPriority(USART2_IRQn, 2);
    NVIC_EnableIRQ(USART2_IRQn);
		
		usart_interrupt_enable(USART2, USART_IDLE_INT, TRUE); 
		
		}
     
//    		// input on telemetry
//    gpio_init_type gpio_init_struct;


//    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);

//    /* configure the usart2 tx pin */
//    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
//    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
//    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
//    gpio_init_struct.gpio_pins = GPIO_PINS_6;
//    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
//    gpio_init(GPIOB, &gpio_init_struct);
//    

//    dma_reset(DMA1_CHANNEL3);

//    dma_init_type dma_init_struct;
//    dma_default_para_init(&dma_init_struct);
//    dma_init_struct.buffer_size = 64;
//    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
//    dma_init_struct.memory_base_addr = (uint32_t)&crsf_buffer;
//    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
//    dma_init_struct.memory_inc_enable = TRUE;
//    dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt;
//    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
//    dma_init_struct.peripheral_inc_enable = FALSE;
//    dma_init_struct.priority = DMA_PRIORITY_LOW;
//    dma_init_struct.loop_mode_enable = TRUE;
//    dma_init(DMA1_CHANNEL3, &dma_init_struct);

//   // DMA1_CHANNEL3->ctrl |= DMA_FDT_INT;
//   // DMA1_CHANNEL3->ctrl |= DMA_DTERR_INT;

//    /* configure usart1 param */
//    usart_init(USART1, 420000, USART_DATA_8BITS, USART_STOP_1_BIT);
//    usart_transmitter_enable(USART1, TRUE);
//    usart_receiver_enable(USART1, TRUE);
//    usart_single_line_halfduplex_select(USART1, TRUE);
//    usart_dma_receiver_enable(USART1, TRUE);
//    usart_enable(USART1, TRUE);
//		
//		NVIC_SetPriority(USART1_IRQn, 2);
//    NVIC_EnableIRQ(USART1_IRQn);
//		
//		usart_interrupt_enable(USART1, USART_IDLE_INT, TRUE); 

}

void receiveCRSF(){
	if(serial_mode == 5){
	  DMA1_CHANNEL3->dtcnt = 64;
	  usart_dma_receiver_enable(USART1, TRUE);
    DMA1_CHANNEL3->ctrl_bit.chen = TRUE;
	}
		if(serial_mode == 6){
	  DMA1_CHANNEL5->dtcnt = 64;
	  usart_dma_receiver_enable(USART2, TRUE);
    DMA1_CHANNEL5->ctrl_bit.chen = TRUE;
	}
}
