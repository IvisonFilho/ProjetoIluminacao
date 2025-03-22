# RESUMO 

 Este projeto propõe o desenvolvimento de um protótipo IoT que simula e controla a iluminação interna de um ambiente, utilizando a placa BitDogLab, baseada no microcontrolador RP2040. O objetivo central consiste em aplicar conceitos de automação residencial e sistemas embarcados, integrando diversos periféricos (botões, LED RGB, display LCD, buzzer e joystick) e utilizando comunicação sem fio (Wi-Fi) para envio de dados a um serviço na nuvem (ThingSpeak). A metodologia envolveu a simulação da medição de intensidade luminosa através de recursos locais, implementando três modos de operação: iluminação automática, manual e desligada. Os testes demonstraram que o sistema atende aos requisitos propostos, apresentando respostas coerentes e feedback visual e sonoro adequados, o que reforça seu potencial para aplicações práticas em ambientes tecnológicos e residenciais. 

Palavras-chave: BitDogLab. IoT. Iluminação. Automação. Simulação. 

# INTRODUÇÃO  

 A crescente demanda por soluções de automação residencial tem impulsionado a inovação na integração entre hardware e software para melhorar o conforto e a eficiência energética dos ambientes. Neste projeto, optou-se por desenvolver um sistema inteligente de controle de iluminação utilizando a BitDogLab, que se destaca por sua versatilidade e capacidade de integração com diversos componentes periféricos. 
 A escolha deste tema se justifica pela oportunidade de aplicar e consolidar conhecimentos práticos em sistemas embarcados, utilizando recursos como ADC, PWM, interfaces gráficas e comunicação via Wi-Fi. Além disso, o projeto aborda o desafio de simular a medição de intensidade luminosa na ausência de um sensor físico, contornando essa limitação por meio de uma implementação inovadora. 
Objetivo Geral: Desenvolver um protótipo IoT que possibilite o controle inteligente da iluminação interna, demonstrando a integração dos diversos componentes e a comunicação com serviços na nuvem. 

# METODOLOGIA  

 A metodologia adotada para o desenvolvimento do protótipo envolveu a integração de componentes específicos (joystick, LEDs, display OLED, entre outros) e a utilização de tecnologias modernas de comunicação (Wi-Fi e protocolo HTTP). Para organizar o desenvolvimento e facilitar a manutenção, o projeto foi dividido em módulos, cada um responsável por uma funcionalidade específica. Na pasta inc/, encontram-se os arquivos de cabeçalho (.h) que declaram funções e estruturas, enquanto na pasta src/ estão as implementações (.c). 

A seguir, são descritos os módulos: 

- joystick.h / joystick.c 
Módulo que gerencia as leituras analógicas do joystick e o tratamento de interrupções do botão. Permite a navegação pelos menus e a seleção de opções. 

led.h / led.c 
Responsável pelo controle de LEDs via PWM, incluindo funções de transição suave de brilho e simulação de luminosidade em diferentes cenários. 

menu.h / menu.c 
Implementa a interface no display OLED, exibindo as opções de configuração e possibilitando a navegação por meio do joystick. 

thingspeak.h / thingspeak.c 
Realiza a conexão Wi-Fi e o envio de dados para o serviço ThingSpeak, utilizando requisições HTTP. Esse módulo é fundamental para a integração IoT do projeto. 

ssd1306.h / ssd1306.c 
Conjunto de funções para inicialização e escrita no display OLED (SSD1306). Abrange rotinas de desenho de texto e elementos gráficos. 

font.h 
Define as fontes utilizadas na exibição de caracteres no display OLED, servindo como base para a renderização de texto. 

lwipopts.h 
Contém as configurações de rede para a biblioteca lwIP (Lightweight IP), utilizada na camada de comunicação TCP/IP, ajustando parâmetros como buffers e número de conexões simultâneas. 

main.c 
Ponto de entrada do programa. Orquestra os demais módulos, inicializa o hardware, gerencia o loop principal e define a lógica de transição entre os modos de operação (iluminação automática, manual e desligada). 

Essa abordagem modular permite que cada funcionalidade (por exemplo, controle de LED ou envio de dados via Wi-Fi) seja desenvolvida e testada de forma independente, resultando em um código mais limpo, de fácil manutenção e pronto para futuras expansões. 

A seguir, são descritos os principais módulos e recursos empregados: 

Componentes Específicos e Tecnologias de Comunicação 

Placa BitDogLab (RP2040): Unidade central do projeto, responsável pelo processamento dos sinais e controle dos periféricos. 

Periféricos: 

Joystick e Botões: Permitem a interação do usuário, capturando movimentos (via ADC) e acionamento de botões (com suporte a interrupções e debounce). 

LED RGB: Configurado via PWM para simular variações de brilho, possibilitando a reprodução de uma iluminação amarela (através da combinação de LEDs vermelho e verde). 

Display OLED (SSD1306): Exibe o menu e as opções de controle, facilitando a interação do usuário com o sistema. 

Buzzer: Fornece feedback sonoro durante a operação (ligar/desligar e confirmações de seleção). 

Comunicação Sem Fio: 

Módulo Wi-Fi do Raspberry Pi Pico W: Estabelece a conexão com a rede, permitindo a transmissão dos dados de luminosidade simulada para a nuvem. 

Protocolos Utilizados: 

HTTP: Para envio de requisições ao serviço ThingSpeak. 

DNS: Para resolução do endereço do servidor ThingSpeak. 

# Descrição dos Módulos 

Módulo Joystick (joystick.c): 
	Gerencia a leitura dos valores analógicos do joystick e o tratamento de eventos do botão: 

Inicialização: Configura os pinos do ADC e do botão com pull-up. 

Leitura: A função ler_joystick_y() lê e escala o valor do eixo Y. 

Interrupção: A função button_irq_callback() trata o pressionamento do botão, utilizando debounce para evitar leituras indevidas. 

	Fig. 2 – Trecho de código do módulo joystick para ler posição do eixo Y. 

Módulo LED (led.c): 

 
	Responsável pelo controle do brilho dos LEDs utilizando PWM: 

 

Simulação de Luminosidade: A função simular_luminosidade() gera valores aleatórios entre 0 e 100 para simular um sensor de luz. 

Configuração PWM: setup_pwm() configura os pinos e ativa o PWM com parâmetros ajustados (PERIOD e DIVIDER_PWM). 

Ajuste de Brilho: Funções set_brightness() e set_brightness_smooth() permitem ajustar o brilho de forma imediata ou gradual. 

	Fig. 2 – Trecho de código do módulo led que ajusta brilho de forma gradual. 

Módulo Menu (menu.c): 
	Implementa a interface gráfica no display OLED para seleção dos modos de operação: 

Inicialização e Desenho: inicializa_menu() desenha o menu com três opções: iluminação automática, manual e desligada. 

Atualização: atualizar_menu() redesenha o menu destacando a opção selecionada. 

Controle: controle_menu() utiliza os valores lidos do joystick para navegar entre as opções. 

	Fig. 3– Trecho de código do módulo menu que atualiza o display. 

Módulo ThingSpeak (thingspeak.c): 
	Gerencia a comunicação com a nuvem, enviando os dados simulados de luminosidade: 

Conexão Wi-Fi: connect_wifi() realiza a conexão à rede utilizando as credenciais pré-definidas. 

Envio de Dados: enviar_luminosidade_ao_thingspeak() prepara e envia uma requisição HTTP GET ao servidor ThingSpeak, utilizando a chave de API e o valor de luminosidade. 

Callbacks: Funções como dns_callback(), http_connected_callback() e http_recv_callback() gerenciam a resolução de DNS, estabelecimento da conexão TCP e recebimento da resposta. 

	Fig. 4 - Trecho de código do módulo thingspeak para conectar ao wi-fi. 

Integração Geral (main.c): 
	Coordena a execução dos módulos, controlando o fluxo entre a interação do usuário, o ajuste dos LEDs e a comunicação com a nuvem. O fluxo geral inclui: 

Inicialização dos módulos de hardware (joystick, LEDs, display OLED e Wi-Fi). 

Exibição e controle do menu para seleção dos modos de operação. 

Execução dos modos escolhidos, com ajustes de brilho e envio dos dados para o ThingSpeak. 

 

	Fig. 5 - Visualização do fluxo geral de funcionamento.  Apesar de ocultado, o buzzer é ativado toda vez que entra no modo selecionado e toda vez que sai do modo selecionado. 

# RESULTADOS ALCANÇADOS e Discussões  

Durante os testes, o protótipo demonstrou desempenho consistente em todos os modos de operação: 

Iluminação Automática: 
O sistema ajusta o brilho de forma autônoma com base em uma rotina pré-definida, simulando o comportamento de um sensor de luz real. O LED RGB varia o brilho conforme os valores simulados, evidenciando a eficácia do algoritmo implementado. 

Iluminação Manual: 
A interface gráfica via display OLED, combinada com o joystick e os botões, permite ao usuário selecionar o nível de iluminação desejado. O feedback visual (alteração do brilho) confirma a interação correta. 

Desligamento da Iluminação: 
Ao optar por desligar a iluminação, o sistema desativa os LEDs e exibe a confirmação da ação, validando o controle e a resposta imediata do sistema. 

Além disso, a integração com o ThingSpeak possibilitou o envio dos dados de luminosidade simulada via HTTP, demonstrando a viabilidade da comunicação com a nuvem e a possibilidade de monitoramento remoto. 

	Fig. 6 - Visualização dos dados enviados à nuvem do ThingSpeak. 

Os resultados obtidos comprovam que o protótipo atende aos requisitos propostos, superando o desafio da ausência de um sensor físico e integrando de forma robusta os componentes específicos e as tecnologias de comunicação. 

 

#	CONCLUSÃO 

O projeto “Controle Inteligente de Iluminação com BitDogLab” atingiu seus objetivos ao demonstrar a integração eficiente entre hardware e software em um ambiente IoT. A utilização da placa BitDogLab, aliada à implementação de módulos de controle via joystick, ajuste de brilho com PWM, interface gráfica no display OLED e comunicação via Wi-Fi, resultou em um sistema interativo e funcional para automação residencial. 

Os desafios enfrentados, como a simulação da medição de luminosidade na ausência de um sensor físico, foram superados por meio da implementação de funções específicas e técnicas de transição suave do brilho. As limitações atuais, como a interface básica do display, apontam para oportunidades de aprimoramento futuro, como a inclusão de sensores reais e uma interface de usuário mais sofisticada. Em suma, o protótipo contribui de forma prática e teórica para o avanço em automação residencial, servindo tanto como plataforma de aprendizado quanto como base para futuras inovações em soluções IoT. 

# REFERÊNCIAS 

BITDOGLAB. BitDogLab-C. 2024. Disponível em: https://github.com/BitDogLab/BitDogLab-C/tree/main. Acesso em: 10 fev. 2025. 
