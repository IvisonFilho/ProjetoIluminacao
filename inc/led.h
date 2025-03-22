#ifndef LED_H
#define LED_H

void setup_pwm();
void set_brightness(uint8_t brightness);
void set_brightness_smooth(uint8_t brightness);
uint8_t simular_luminosidade();

#endif