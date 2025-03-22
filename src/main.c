#include <stdio.h>
#include <stdlib.h>     
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "led.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "menu.h"
#include "joystick.h"
#include "pico/time.h"  // Para gerenciamento de tempo
#include <time.h>
#include "thingspeak.h" // módulo de envio ao ThingSpeak e a função connect_wifi()
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include <string.h>
#include "pico/cyw43_arch.h"

// Definições do I2C e Display
#define I2C_PORT i2c1
#define PINO_SCL 15
#define PINO_SDA 14

// Definições dos botões
#define BUTTON_A 5    // GPIO conectado ao Botão A
#define BUTTON_B 6    // GPIO conectado ao Botão B

// Definições do buzzer
#define BUZZER_PIN 21
#define BUZZER_FREQUENCY 100

// Variável global para o display
ssd1306_t disp;

//Funções do Buzzer
void pwm_init_buzzer(uint pin) {
    // Configura o pino como saída PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configura o PWM para a frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); 
    pwm_init(slice_num, &config, true);

    // Inicializa com duty cycle zero (buzzer desligado)
    pwm_set_gpio_level(pin, 0);
}

void beep(uint pin, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    // Define duty cycle de 50% para ligar o buzzer
    pwm_set_gpio_level(pin, 2048);
    sleep_ms(duration_ms);
    // Desliga o buzzer
    pwm_set_gpio_level(pin, 0);
    // Pequena pausa entre beeps
    sleep_ms(100);
}
//Fim das funções do Buzzer

void inicializa() {
    stdio_init_all();

    // Inicializa I2C e o display SSD1306
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SCL);
    gpio_pull_up(PINO_SDA);

    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);
}

int main() {

    // Inicializa o Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();
    connect_wifi();

    // Aguarda até que a conexão seja estabelecida
    while (!cyw43_arch_wifi_connect_async) {
        printf("Tentando reconectar Wi-Fi...\n");
        connect_wifi();
        cyw43_arch_poll();
        sleep_ms(1000);
    }

    inicializa();
    inicializa_joystick(); // Inicializa o joystick

    // Habilita a interrupção para o pino SW (botão usado para sair de modos)
    gpio_set_irq_enabled_with_callback(SW, GPIO_IRQ_EDGE_FALL, true, button_irq_callback);

    // Inicializa os botões para ajuste manual do brilho
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    // Inicializa o buzzer
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    pwm_init_buzzer(BUZZER_PIN);

    setup_pwm();

    uint countdown = 0;
    uint countup = 2;
    uint pos_y = 12; // Posição inicial do menu

    // Intervalo para atualização
    uint32_t interval = 120000;
    inicializa_menu(&disp);
    srand(to_us_since_boot(get_absolute_time()));

    absolute_time_t next_wake_time = delayed_by_us(get_absolute_time(), interval * 1000);

    while (true) {
        // Mantém o Wi-Fi ativo; se a conexão cair, tenta reconectar
        cyw43_arch_poll();
        if (!cyw43_arch_wifi_connect_async) {
            printf("Conexão perdida! Esperando reconectar...\n");
            while (!cyw43_arch_wifi_connect_async) {
                connect_wifi();
                cyw43_arch_poll();
                sleep_ms(1000);
            }
            printf("Conexão reestabelecida!\n");
        }

        // Lê a posição do joystick e atualiza o menu
        uint bar_y_pos = ler_joystick_y();
        controle_menu(&pos_y, &countdown, &countup);
        atualizar_menu(&disp, pos_y);
        sleep_ms(100);

        // Se o botão SW for pressionado...
        if (gpio_get(SW) == 0) {
            // Aguarda o botão ser liberado
            while (gpio_get(SW) == 0) {
                sleep_ms(1000);
            }

            switch (pos_y) {
            case 12: // [1] Iluminação Automática
            {
                printf("Modo Automático Ativado!\n");
                beep(BUZZER_PIN, 200); // Beep para entrada no modo automático

                reset_exit_mode();
                gpio_set_irq_enabled_with_callback(SW, GPIO_IRQ_EDGE_FALL, true, button_irq_callback);

                // Ativação imediata:
                uint8_t sensor_luminosidade = rand() % 101;
                printf("Luminosidade inicial: %d%%\n", sensor_luminosidade);
                set_brightness_smooth(sensor_luminosidade);

                // Envio da luminosidade para o ThingSpeak
                cyw43_arch_poll();
                if (!cyw43_arch_wifi_connect_async) {
                    printf("Conexão perdida! Esperando reconectar...\n");
                    while (!cyw43_arch_wifi_connect_async) {
                        connect_wifi();
                        cyw43_arch_poll();
                        sleep_ms(1000);
                    }
                    printf("Conexão reestabelecida!\n");
                }
                enviar_luminosidade_ao_thingspeak(sensor_luminosidade);

                // Reagenda o temporizador para a próxima atualização
                next_wake_time = delayed_by_us(get_absolute_time(), interval * 1000);

                while (true) {
                    // Atualiza a luminosidade periodicamente
                    if (time_reached(next_wake_time)) {
                        printf("Timer atingido! Atualizando luminosidade.\n");
                        sensor_luminosidade = rand() % 101;
                        printf("Luminosidade detectada: %d%%\n", sensor_luminosidade);
                        set_brightness_smooth(sensor_luminosidade);

                        cyw43_arch_poll();
                        if (!cyw43_arch_wifi_connect_async) {
                            printf("Conexão perdida! Esperando reconectar...\n");
                            while (!cyw43_arch_wifi_connect_async) {
                                connect_wifi();
                                cyw43_arch_poll();
                                sleep_ms(1000);
                            }
                            printf("Conexão reestabelecida!\n");
                        }
                        enviar_luminosidade_ao_thingspeak(sensor_luminosidade);

                        // Reagenda para a próxima execução
                        next_wake_time = delayed_by_us(get_absolute_time(), interval * 1000);
                    }
                    
                    // Se for solicitado sair do modo...
                    if (get_exist_mode()) {
                        printf("Saindo do Modo Automático.\n");
                        beep(BUZZER_PIN, 200); // Beep ao sair do modo automático
                        gpio_set_irq_enabled(SW, GPIO_IRQ_EDGE_FALL, false);
                        reset_exit_mode();
                        // Aguarda o botão ser liberado (debounce)
                        while (gpio_get(SW) == 0) {
                            sleep_ms(10);
                        }
                        sleep_ms(300);
                        break;
                    }
                    sleep_ms(1); // Delay breve para não sobrecarregar a CPU
                }
                break;
            }
            case 24: // [2] Escolher Iluminação (modo manual – valores fixos)
            {
                printf("Modo de Iluminação Manual Ativado!\n");
                beep(BUZZER_PIN, 200); // Beep para confirmar entrada no modo manual

                int brightness = 50;  // Valor inicial do brilho (%)
                set_brightness_smooth(brightness);

                // Envia valor inicial ao ThingSpeak
                cyw43_arch_poll();
                if (!cyw43_arch_wifi_connect_async) {
                    printf("Conexão perdida! Esperando reconectar...\n");
                    while (!cyw43_arch_wifi_connect_async) {
                        connect_wifi();
                        cyw43_arch_poll();
                        sleep_ms(1000);
                    }
                    printf("Conexão reestabelecida!\n");
                }
                enviar_luminosidade_ao_thingspeak(brightness);

                while (true) {
                    // Se pressionar BUTTON_A, diminui 5%
                    if (gpio_get(BUTTON_A) == 0) {
                        sleep_ms(50); // debounce
                        brightness -= 5;
                        if (brightness < 0)
                            brightness = 0;
                        printf("Brilho diminuído: %d%%\n", brightness);
                        set_brightness_smooth(brightness);

                        cyw43_arch_poll();
                        if (!cyw43_arch_wifi_connect_async) {
                            printf("Conexão perdida! Esperando reconectar...\n");
                            while (!cyw43_arch_wifi_connect_async) {
                                connect_wifi();
                                cyw43_arch_poll();
                                sleep_ms(1000);
                            }
                            printf("Conexão reestabelecida!\n");
                        }
                        enviar_luminosidade_ao_thingspeak(brightness);
                        beep(BUZZER_PIN, 100); // Feedback sonoro para ajuste

                        while (gpio_get(BUTTON_A) == 0)
                            sleep_ms(10);
                        sleep_ms(300); // debounce extra
                    }

                    // Se pressionar BUTTON_B, aumenta 5%
                    if (gpio_get(BUTTON_B) == 0) {
                        sleep_ms(50); // debounce
                        brightness += 5;
                        if (brightness > 100)
                            brightness = 100;
                        printf("Brilho aumentado: %d%%\n", brightness);
                        set_brightness_smooth(brightness);

                        cyw43_arch_poll();
                        if (!cyw43_arch_wifi_connect_async) {
                            printf("Conexão perdida! Esperando reconectar...\n");
                            while (!cyw43_arch_wifi_connect_async) {
                                connect_wifi();
                                cyw43_arch_poll();
                                sleep_ms(1000);
                            }
                            printf("Conexão reestabelecida!\n");
                        }
                        enviar_luminosidade_ao_thingspeak(brightness);
                        beep(BUZZER_PIN, 100); // Feedback sonoro para ajuste

                        while (gpio_get(BUTTON_B) == 0)
                            sleep_ms(10);
                        sleep_ms(300); // debounce extra
                    }

                    // Se pressionar SW, sai do modo manual
                    if (gpio_get(SW) == 0) {
                        // Aguarda o botão ser liberado e emite beep de saída
                        while (gpio_get(SW) == 0) {
                            sleep_ms(10);
                        }
                        sleep_ms(300);
                        beep(BUZZER_PIN, 200); // Beep ao sair do modo manual
                        break;
                    }
                    sleep_ms(10);
                }
                break;
            }
            case 36: // [3] Desligando
            {
                printf("Desligando Iluminação...\n");
                beep(BUZZER_PIN, 200); // Indica com beep o desligamento
                set_brightness_smooth(0);

                cyw43_arch_poll();
                if (!cyw43_arch_wifi_connect_async) {
                    printf("Conexão perdida! Esperando reconectar...\n");
                    while (!cyw43_arch_wifi_connect_async) {
                        connect_wifi();
                        cyw43_arch_poll();
                        sleep_ms(1000);
                    }
                    printf("Conexão reestabelecida!\n");
                }
                enviar_luminosidade_ao_thingspeak(0);
                break;
            }
            default:
                printf("Nenhuma opção válida selecionada.\n");
                break;
            }
        }
    }

    return 0;
}
