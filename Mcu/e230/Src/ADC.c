/*
 * ADC.c
 *
 *  Created on: May 20, 2020
 *      Author: Alka
 */
#include "ADC.h"

#include "functions.h"

#ifdef USE_ADC_INPUT
uint16_t ADCDataDMA[4];
#else
uint16_t ADCDataDMA[3];
#endif

extern uint16_t ADC_raw_temp;
extern uint16_t ADC_raw_volts;
extern uint16_t ADC_raw_current;
extern uint16_t ADC_raw_input;

void ADC_DMA_Callback()
{ // read dma buffer and set extern variables

#ifdef USE_ADC_INPUT
    ADC_raw_temp = ADCDataDMA[3];
    ADC_raw_volts = ADCDataDMA[1] / 2;
    ADC_raw_current = ADCDataDMA[2];
    ADC_raw_input = ADCDataDMA[0];

#else
    ADC_raw_temp = ADCDataDMA[2];
#ifdef PA6_VOLTAGE
    ADC_raw_volts = ADCDataDMA[1];
    ADC_raw_current = ADCDataDMA[0];
#else
    ADC_raw_volts = ADCDataDMA[0];
    ADC_raw_current = ADCDataDMA[1];
#endif
#endif
}

void ADC_Init(void)
{
 
    rcu_periph_clock_enable(RCU_ADC);
 
  //  rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
//	  RCU_CFG0 &= ~RCU_CFG0_ADCPSC;
//    RCU_CFG2 &= ~(RCU_CFG2_ADCSEL | RCU_CFG2_IRC28MDIV | RCU_CFG2_ADCPSC2);
	  RCU_CFG0 |= RCU_ADC_CKAPB2_DIV6;
    RCU_CFG2 |= RCU_CFG2_ADCSEL;
	//  RCU_CFG2 |= 0x01U<<(8);

//    /* config the GPIO as analog mode */
//    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,
//        GPIO_PIN_3 | GPIO_PIN_6);
    GPIO_CTL(GPIOA)
        = ((((((GPIO_CTL(GPIOA)))) & (~(((GPIO_PIN_3 * GPIO_PIN_3) * (0x3UL << (0U))))))
            | (((GPIO_PIN_3 * GPIO_PIN_3) * GPIO_MODE_ANALOG))));
	    GPIO_CTL(GPIOA)
        = ((((((GPIO_CTL(GPIOA)))) & (~(((GPIO_PIN_6 * GPIO_PIN_6) * (0x3UL << (0U))))))
            | (((GPIO_PIN_6 * GPIO_PIN_6) * GPIO_MODE_ANALOG))));
	
    dma_parameter_struct dma_data_parameter;

//    /* ADC DMA_channel configuration */
//    dma_deinit(DMA_CH0);
//      DMA_CHMADDR(DMA_CH0) = (uint32_t)&ADCDataDMA;
//			DMA_CHCNT(DMA_CH0) = (3 & DMA_CHANNEL_CNT_MASK);
//			DMA_CHPADDR(DMA_CH0) = (uint32_t)&ADC_RDATA;
//			   DMA_CHCTL(DMA_CH0)
//        = 0x0000098b; // just set the whole reg in one go to enable
//    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA);
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(&ADCDataDMA);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = 3U;
    dma_data_parameter.priority = DMA_PRIORITY_LOW;
    dma_init(DMA_CH0, &dma_data_parameter);
      
    dma_circulation_enable(DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA_CH0);

      ADC_CTL0 |= ADC_SCAN_MODE;
//      ADC_CTL1 |= ADC_CONTINUOUS_MODE; 
      ADC_CTL1 &= ~((uint32_t)ADC_CTL1_DAL);    // datalign right
			ADC_RSQ0 |= RSQ0_RL((uint32_t)(3U-1U));  // adc data length
			ADC_CTL1 |= ADC_CTL1_TSVREN; // temp sensor enable
			
			ADC_RSQ2 |= ((uint32_t)ADC_CHANNEL_3 << (5U*0));
			ADC_RSQ2 |= ((uint32_t)ADC_CHANNEL_6 << (5U*1));
			ADC_RSQ2 |= ((uint32_t)ADC_CHANNEL_16 << (5U*2));
			
      ADC_SAMPT1 |= (uint32_t)(ADC_SAMPLETIME_55POINT5 << (3U*ADC_CHANNEL_1));
			ADC_SAMPT1 |= (uint32_t)(ADC_SAMPLETIME_55POINT5 << (3U*ADC_CHANNEL_2));
			ADC_SAMPT1 |= (uint32_t)(ADC_SAMPLETIME_55POINT5 << (3U*ADC_CHANNEL_3));
//    adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE);
//    /* ADC scan function enable */
//    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
//    /* ADC data alignment config */
//    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
//    /* ADC channel length config */
//    adc_channel_length_config(ADC_REGULAR_CHANNEL, 3U);

//    adc_tempsensor_vrefint_enable(); // enable the temp sensor

//    /* ADC regular channel config */
//    adc_regular_channel_config(0, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);
//    adc_regular_channel_config(1, ADC_CHANNEL_6, ADC_SAMPLETIME_55POINT5);
//    adc_regular_channel_config(2, ADC_CHANNEL_16, ADC_SAMPLETIME_55POINT5);

//    /* ADC trigger config */
//    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL,
//        ADC_EXTTRIG_REGULAR_NONE);
//    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

//    /* enable ADC interface */
    adc_enable();
    delayMicros(1000);
//    /* ADC calibration and reset calibration */
    adc_calibration_enable();

//    /* ADC DMA function enable */
    adc_dma_mode_enable();
//    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}
