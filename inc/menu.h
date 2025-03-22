#ifndef MENU_H
#define MENU_H

#include "ssd1306.h"

// Função para inicializar o menu
void inicializa_menu(ssd1306_t *disp);

// Função para atualizar o menu no display
void atualizar_menu(ssd1306_t *disp, uint pos_y);

// Função para lidar com o controle do menu
void controle_menu(uint *pos_y, uint *countdown, uint *countup);

#endif
