#include "menu.h"
#include "ssd1306.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

uint pos_y = 12;   // Variável para armazenar a posição do seletor no display
const uint menu_space = 8;  // Redução do espaçamento entre as opções

void inicializa_menu(ssd1306_t *disp)
{
    ssd1306_clear(disp);  // Limpa a tela

    // Desenha o título do menu
    ssd1306_draw_string(disp, 52, 2, 1, "Menu");
    ssd1306_draw_string(disp, 6, 18, 1, "[1] Iluminacao Auto");
    ssd1306_draw_string(disp, 6, 30, 1, "[2] Escolher Ilum.");
    ssd1306_draw_string(disp, 6, 42, 1, "[3] Desligar");
}

void atualizar_menu(ssd1306_t *disp, uint pos_y)
{
    ssd1306_clear(disp);  // Limpa a tela antes de redesenhar

    // Desenha o título e as opções de menu
    ssd1306_draw_string(disp, 52, 2, 1, "Menu");
    ssd1306_draw_empty_square(disp, 2, pos_y, 120, 18);  // Caixa ao redor da opção selecionada
    ssd1306_draw_string(disp, 6, 18, 1, "[1] Iluminacao Auto");
    ssd1306_draw_string(disp, 6, 30, 1, "[2] Escolher Ilum.");
    ssd1306_draw_string(disp, 6, 42, 1, "[3] Desligar");

    ssd1306_show(disp);  // Atualiza o display
}

void controle_menu(uint *pos_y, uint *countdown, uint *countup)
{
    // Lê o valor do joystick (eixo Y)
    adc_select_input(0);
    uint adc_y_raw = adc_read();
    const uint bar_width = 40;
    const uint adc_max = (1 << 12) - 1;               // valor máximo do ADC (4095)
    uint bar_y_pos = adc_y_raw * bar_width / adc_max; // escala o valor

    // Atualiza a posição do seletor com base na leitura do joystick.
    if (bar_y_pos < 18 && *countdown < 2)
    {
        *pos_y += 12;
        *countdown += 1;
        *countup -= 1;
    }
    else if (bar_y_pos > 19 && *countup < 2)
    {
        *pos_y -= 12;
        *countup += 1;
        *countdown -= 1;
    }
}
