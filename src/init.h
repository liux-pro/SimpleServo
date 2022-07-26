#ifndef INIT_H_
#define INIT_H_
#include "fw_hal.h"

extern volatile uint16_t battery;
extern uint16_t angle0;
extern uint16_t angle1;
extern uint8_t adc_channel;


void init_pwm(void);
void init_adc();
INTERRUPT(ADC_Routine, EXTI_VectADC);

void init_led();

void led_red_on();
void led_red_off();
void led_blue_on();
void led_blue_off();

#endif /* INIT_H_ */
