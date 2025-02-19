# Controle de LEDs WS2812 com Interação Serial e Botões

## Descrição do Projeto

Controle de LEDs WS2812 usando o microcontrolador RP2040 (BitDogLab). A interação ocorre via monitor serial e dois botões físicos, com exibição de caracteres no display OLED SSD1306 e controle de LED RGB.

## Componentes Utilizados

- **LED RGB** – GPIOs 11 (vermelho), 12 (verde), 13 (azul).
- **Botão A** – GPIO 5.
- **Botão B** – GPIO 6.
- **Display OLED SSD1306 (128x64)** – I2C (GPIO 14 e 15).

## DEMONSTRAÇÃO

[Vídeo de Demonstração](https://youtube.com/shorts/gjlzXJP4lAk)

## Funcionalidades

- **Customização da biblioteca `font.h`:** Inclusão de caracteres minúsculos.
- **Interrupções (IRQ):** Implementadas para ambos os botões com tratamento de debouncing via software.
- **Controle de LEDs:** Manipulação dos LEDs WS2812 e do LED RGB.
- **Comunicação I2C:** Exibição de caracteres no display SSD1306.
- **Comunicação UART:** Registro e envio de eventos ao PC.

## Requisitos do Projeto

1. Utilizar rotinas de interrupção para os botões.
2. Implementar debouncing em software.
3. Controlar LEDs comuns e WS2812.
4. Operar o display SSD1306 via I2C com suporte a maiúsculas e minúsculas.
5. Enviar informações via UART.
6. Organizar e comentar o código de forma técnica.

## Compilação

- Configure o ambiente de desenvolvimento para RP2040.
- Compile com um compilador compatível (ex.: `arm-none-eabi-gcc`).
- Gere os arquivos `.uf2` e `.elf`.

## Execução

1. Carregue o arquivo `.uf2` no Raspberry Pi Pico W.
2. Use o Wokwi para simulação (arquivo `diagram.json`).
3. Teste a entrada via monitor serial do VS Code.

## Desenvolvedores

**Equipe 2 - Grupo 3 - EmbarcaTech**

- [Luiz Marcelo](https://github.com/devluinix)
