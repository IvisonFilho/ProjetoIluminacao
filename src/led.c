#include <stdio.h>
#include <stdlib.h>  
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "led.h"

const uint PIN_LED_RED = 13; // Pino do LED conectado
const uint PIN_LED_GREEN = 11;
const uint16_t PERIOD = 2000;   // Período do PWM (valor máximo do contador)
const float DIVIDER_PWM = 16.0; // Divisor fracional do clock para o PWM
const uint16_t LED_STEP = 100;  // Passo de incremento/decremento para o duty cycle do LED
uint16_t led_level = 0;         // Nível inicial do PWM (duty cycle)
uint16_t led_level_change = 0;


// Simula um sensor de luminosidade (valores entre 0 e 100)
uint8_t simular_luminosidade()
{
    return rand() % 101; // Gera um número aleatório entre 0 e 100
}

void setup_pwm()
{
    uint slice_green;
    uint slice_red;

    gpio_set_function(PIN_LED_GREEN, GPIO_FUNC_PWM); // Configura o pino do LED para função PWM
    gpio_set_function(PIN_LED_RED, GPIO_FUNC_PWM);

    slice_green = pwm_gpio_to_slice_num(PIN_LED_GREEN); // Obtém o slice do PWM associado ao pino do LED
    slice_red = pwm_gpio_to_slice_num(PIN_LED_RED);

    pwm_set_clkdiv(slice_green, DIVIDER_PWM); // Define o divisor de clock do PWM
    pwm_set_clkdiv(slice_red, DIVIDER_PWM);

    pwm_set_wrap(slice_green, PERIOD); // Configura o valor máximo do contador (período do PWM)
    pwm_set_wrap(slice_red, PERIOD);

    pwm_set_gpio_level(PIN_LED_GREEN, led_level); // Define o nível inicial do PWM para o pino do LED
    pwm_set_gpio_level(PIN_LED_RED, led_level);

    led_level_change = led_level;

    pwm_set_enabled(slice_green, true); // Habilita o PWM no slice correspondente
    pwm_set_enabled(slice_red, true);
}

// Função para alterar o brilho. Brightness recebe % entre 0 a 100
void set_brightness(uint8_t brightness)
{
    uint16_t led_level_change = (brightness * PERIOD) / 100; // Converte para duty cycle

    // Ajusta o brilho dos LEDs vermelho e verde para a cor amarela
    pwm_set_gpio_level(PIN_LED_GREEN, led_level_change);
    pwm_set_gpio_level(PIN_LED_RED, led_level_change);
}

void set_brightness_smooth(uint8_t brightness)
{
    uint16_t target_level = (brightness * PERIOD) / 100; // Converte para duty cycle

    // Caso o LED esteja apagado, zera a variável de controle
    if (target_level == 0)
    {
        led_level_change = 0;
    }

    if (led_level_change > target_level)
    {
        for (uint16_t i = led_level_change; i >= target_level; i--)
        {
            pwm_set_gpio_level(PIN_LED_GREEN, i);
            pwm_set_gpio_level(PIN_LED_RED, i);
            sleep_ms(2); // Pequeno delay para não tremer a aplicação
        }
    }
    else
    {
        for (uint16_t i = led_level_change; i <= target_level; i++)
        {
            pwm_set_gpio_level(PIN_LED_GREEN, i);
            pwm_set_gpio_level(PIN_LED_RED, i);
            sleep_ms(2);
        }
    }
    led_level_change = target_level;
}
