#ifndef JOYSTICK_H
#define JOYSTICK_H
#include <stdbool.h>
#include <stdint.h>  // Inclua este cabeçalho para definir uint32_t

// Definição dos pinos do joystick
#define SW 22       // Pino do botão do joystick
#define PINO_X 26   // Pino do eixo X do joystick (se aplicável)
#define PINO_Y 27   // Pino do eixo Y do joystick (se aplicável)

// Função para inicializar o joystick e seus pinos
void inicializa_joystick();

// Função para ler a posição do eixo Y do joystick
int ler_joystick_y();

// Callback da interrupção para o botão do joystick
void button_irq_callback(unsigned int gpio, uint32_t events);

// Função para obter a flag de saída do modo
bool get_exist_mode();

// Função para resetar a flag de saída
void reset_exit_mode();

#endif
