#include "joystick.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

// Flag volátil usada para sinalizar a saída do modo "Joystick Led"
static volatile bool exit_mode = false;

// Função para inicializar o joystick e seus pinos
void inicializa_joystick()
{
    // Inicializa o ADC para leitura dos pinos do joystick
    adc_init();
    adc_gpio_init(PINO_X);  // Inicializa o pino X
    adc_gpio_init(PINO_Y);  // Inicializa o pino Y

    // Configura o pino do botão do joystick
    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);
}

// Função para ler a posição do eixo Y do joystick
int ler_joystick_y()
{
    // Lê o valor do joystick no eixo Y
    adc_select_input(1);  // Seleciona o pino do eixo Y (ajuste conforme necessário)
    uint adc_y_raw = adc_read();
    const uint bar_width = 40;
    const uint adc_max = (1 << 12) - 1;  // valor máximo do ADC (4095)
    uint bar_y_pos = adc_y_raw * bar_width / adc_max;  // Escala o valor para o tamanho desejado
    return bar_y_pos;
}

// Callback da interrupção para o botão (SW)
// Quando o botão for pressionado, esta função é chamada e seta a flag exit_mode.
void button_irq_callback(unsigned int gpio, uint32_t events)
{
    static absolute_time_t last_interrupt_time = {0};
    absolute_time_t now = get_absolute_time();
    
    // Se a última interrupção foi há menos de 200ms, ignora
    if (absolute_time_diff_us(last_interrupt_time, now) < 200000) {
        return;
    }
    last_interrupt_time = now;
    
    exit_mode = true;
}

// Retorna o estado da flag exit_mode
bool get_exist_mode()
{
    return exit_mode;
}

// Reseta a flag exit_mode para false
void reset_exit_mode()
{
    exit_mode = false;
}
