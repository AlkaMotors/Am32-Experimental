///*
// * serial_telemetry.c
// *
// *  Created on: May 13, 2020
// *      Author: Alka
// */

#include "serial_telemetry.h"
#include "targets.h"

uint8_t aTxBuffer[10];
uint8_t nbDataToTransmit = sizeof(aTxBuffer);

void send_telem_DMA()
{ // set data length and enable channel to start transfer
    DMA1_CHANNEL2->dtcnt = nbDataToTransmit;
    DMA1_CHANNEL2->ctrl_bit.chen = TRUE;
}

//char get_crc8(const uint8_t *buffer, int length) {
//	static const char table[] = {
//		0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
//		0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65, 0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
//		0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
//		0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
//		0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2, 0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
//		0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
//		0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
//		0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42, 0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
//		0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
//		0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
//		0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c, 0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
//		0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
//		0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
//		0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b, 0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
//		0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
//		0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3,
//	};
//	char crc = 0;
//	while (length--) crc = table[crc ^ *buffer++];
//	return crc;
//}
uint8_t
update_crc8(uint8_t crc, uint8_t crc_seed)
{
    uint8_t crc_u, i;
    crc_u = crc;
    crc_u ^= crc_seed;
    for (i = 0; i < 8; i++)
        crc_u = (crc_u & 0x80) ? 0x7 ^ (crc_u << 1) : (crc_u << 1);
    return (crc_u);
}

uint8_t
get_crc8(uint8_t* Buf, uint8_t BufLen)
{
    uint8_t crc = 0, i;
    for (i = 0; i < BufLen; i++)
        crc = update_crc8(Buf[i], crc);
    return (crc);
}

void makeTelemPackage(uint8_t temp, uint16_t voltage, uint16_t current,
    uint16_t consumption, uint16_t e_rpm)
{
    aTxBuffer[0] = temp; // temperature

    aTxBuffer[1] = (voltage >> 8) & 0xFF; // voltage hB
    aTxBuffer[2] = voltage & 0xFF; // voltage   lowB

    aTxBuffer[3] = (current >> 8) & 0xFF; // current
    aTxBuffer[4] = current & 0xFF; // divide by 10 for Amps

    aTxBuffer[5] = (consumption >> 8) & 0xFF; // consumption
    aTxBuffer[6] = consumption & 0xFF; //  in mah

    aTxBuffer[7] = (e_rpm >> 8) & 0xFF; //
    aTxBuffer[8] = e_rpm & 0xFF; // eRpM *100

    aTxBuffer[9] = get_crc8(aTxBuffer, 9);
}

//void telem_UART_Init(void)
//{
//    gpio_init_type gpio_init_struct;

//    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
//    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
//    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);

//    /* configure the usart2 tx pin */
//    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
//    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
//    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
//    gpio_init_struct.gpio_pins = GPIO_PINS_6;
//    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
//    gpio_init(GPIOB, &gpio_init_struct);
//    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE6, GPIO_MUX_0);

//    dma_reset(DMA1_CHANNEL2);

//    dma_init_type dma_init_struct;
//    dma_default_para_init(&dma_init_struct);
//    dma_init_struct.buffer_size = nbDataToTransmit;
//    dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
//    dma_init_struct.memory_base_addr = (uint32_t)&aTxBuffer;
//    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
//    dma_init_struct.memory_inc_enable = TRUE;
//    dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt;
//    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
//    dma_init_struct.peripheral_inc_enable = FALSE;
//    dma_init_struct.priority = DMA_PRIORITY_LOW;
//    dma_init_struct.loop_mode_enable = FALSE;
//    dma_init(DMA1_CHANNEL2, &dma_init_struct);

//    DMA1_CHANNEL2->ctrl |= DMA_FDT_INT;
//    DMA1_CHANNEL2->ctrl |= DMA_DTERR_INT;

//    /* configure usart1 param */
//    usart_init(USART1, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
//    usart_transmitter_enable(USART1, TRUE);
//    usart_receiver_enable(USART1, TRUE);
//    usart_single_line_halfduplex_select(USART1, TRUE);
//    usart_dma_transmitter_enable(USART1, TRUE);
//    usart_enable(USART1, TRUE);

//    nvic_irq_enable(DMA1_Channel3_2_IRQn, 3, 0);
//}

void telem_UART_Init(void)
{
#ifdef USE_PA14_TELEMETRY
  gpio_init_type gpio_init_struct;


  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  
	  /* configure the usart2 tx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_14;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOA, &gpio_init_struct);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE14, GPIO_MUX_1);

		  
  dma_reset(DMA1_CHANNEL4);
	
	dma_init_type dma_init_struct;
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = nbDataToTransmit;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)&aTxBuffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&USART2->dt;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_LOW;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA1_CHANNEL4, &dma_init_struct);
		
//	DMA1_CHANNEL4->ctrl |= DMA_FDT_INT;
//	DMA1_CHANNEL4->ctrl |= DMA_DTERR_INT;
	
	
  /* configure usart1 param */
  usart_init(USART2, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(USART2, TRUE);
  usart_receiver_enable(USART2, TRUE);
  usart_single_line_halfduplex_select(USART2, TRUE);
	usart_dma_transmitter_enable(USART2, TRUE);
  usart_enable(USART2, TRUE);
	
 // nvic_irq_enable(DMA1_Channel5_4_IRQn, 3, 0);
  #else
	
  gpio_init_type gpio_init_struct;


  crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  
	  /* configure the usart2 tx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOB, &gpio_init_struct);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE6, GPIO_MUX_0);

		  
  dma_reset(DMA1_CHANNEL2);
	
	dma_init_type dma_init_struct;
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = nbDataToTransmit;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)&aTxBuffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_LOW;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA1_CHANNEL2, &dma_init_struct);
		
//	DMA1_CHANNEL2->ctrl |= DMA_FDT_INT;
//	DMA1_CHANNEL2->ctrl |= DMA_DTERR_INT;
	
	
  /* configure usart1 param */
  usart_init(USART1, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(USART1, TRUE);
  usart_receiver_enable(USART1, TRUE);
  usart_single_line_halfduplex_select(USART1, TRUE);
	usart_dma_transmitter_enable(USART1, TRUE);
  usart_enable(USART1, TRUE);
	
 // nvic_irq_enable(DMA1_Channel3_2_IRQn, 3, 0);
	
	#endif
}