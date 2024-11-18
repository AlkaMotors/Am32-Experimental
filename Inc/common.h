#pragma once
#include "main.h"
#include "targets.h"

extern uint8_t stick_calibration;
extern char serial_mode;
extern uint8_t eepromBuffer[256];
extern uint16_t TIMER1_MAX_ARR;
extern uint16_t dshot_frametime_high;
extern uint16_t dshot_frametime_low;
extern uint32_t gcr[37];
extern uint8_t buffer_padding;
extern uint16_t adjusted_input;
extern uint32_t dma_buffer[64];
extern uint8_t dshotcommand;
extern uint16_t armed_count_threshold;
extern char forward;
extern uint8_t running;
extern uint16_t zero_input_count;
extern uint16_t signaltimeout;
extern uint16_t input;
extern uint16_t newinput;
extern char play_tone_flag;
extern uint32_t current_GPIO_PIN;
extern char ic_timer_prescaler;
extern uint8_t buffersize;
extern char output_timer_prescaler;
extern uint8_t compute_dshot_flag;
#ifdef STMICRO
extern GPIO_TypeDef* current_GPIO_PORT;
#ifndef MCU_F031
extern COMP_TypeDef* active_COMP;
#endif
#endif
extern char input_ready;

#ifdef GIGADEVICES
extern uint32_t current_GPIO_PORT;
#endif
#ifdef ARTERY
extern uint32_t current_GPIO_PORT;
#endif
extern uint32_t current_EXTI_LINE;
extern int e_com_time;
extern char dshot_extended_telemetry;
extern char EDT_ARM_ENABLE;
extern char EDT_ARMED;
extern uint16_t send_extended_dshot;
extern uint8_t crsf_buffer[100];
extern uint8_t crsf_input_channel;
extern uint8_t crsf_second_input_channel;
extern uint16_t crsf_second_input;
extern uint8_t crsf_output_PWM_channel;
extern char armed;

typedef struct fastPID {
    int32_t error;
    uint32_t Kp;
    uint32_t Ki;
    uint32_t Kd;
    int32_t integral;
    int32_t derivative;
    int32_t last_error;
    int32_t pid_output;
    int32_t integral_limit;
    int32_t output_limit;
} fastPID;
