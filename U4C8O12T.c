#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "inc/ssd1306.h"

#define PORTA_I2C i2c1
#define SDA_I2C 14
#define SCL_I2C 15
#define ENDERECO_SSD1306 0x3C

// Joystick ADC (GPIO26 = ADC0, GPIO27 = ADC1)
#define JOYSTICK_X_ADC 0
#define JOYSTICK_Y_ADC 1

// Botões
#define BOTAO_JOYSTICK 22
#define ZONA_MORTA 250
#define BOTAO_A 5
#define TEMPO_DEBOUNCE 200

// LED RGB (PWM nos LEDs Vermelho e Azul; Verde digital)
#define LED_VERMELHO 13
#define LED_AZUL 12
#define LED_VERDE 11

#define LIMITE_PWM 4095

volatile uint32_t ultimo_botao_a = 0;
volatile uint32_t ultimo_botao_joystick = 0;
volatile bool pwm_ativo = true;
volatile int estilo_borda = 0;
volatile bool estado_led_verde = false;

ssd1306_t ssd;

void callback_gpio(uint gpio, uint32_t eventos)
{
  uint32_t agora = to_ms_since_boot(get_absolute_time());
  if (gpio == BOTAO_A)
  {
    if (agora - ultimo_botao_a < TEMPO_DEBOUNCE)
      return;
    pwm_ativo = !pwm_ativo;
    ultimo_botao_a = agora;
  }
  if (gpio == BOTAO_JOYSTICK)
  {
    if (agora - ultimo_botao_joystick < TEMPO_DEBOUNCE)
      return;
    estado_led_verde = !estado_led_verde;
    estilo_borda = (estilo_borda + 1) % 2;
    gpio_put(LED_VERDE, estado_led_verde);
    ultimo_botao_joystick = agora;
  }
}

int main()
{
  stdio_init_all();

  // Inicialização I2C/Display
  i2c_init(PORTA_I2C, 400 * 1000);
  gpio_set_function(SDA_I2C, GPIO_FUNC_I2C);
  gpio_set_function(SCL_I2C, GPIO_FUNC_I2C);
  gpio_pull_up(SDA_I2C);
  gpio_pull_up(SCL_I2C);
  ssd1306_init(&ssd, 128, 64, false, ENDERECO_SSD1306, PORTA_I2C);
  ssd1306_config(&ssd);
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Inicialização ADC
  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(27);

  // Configuração PWM LED Vermelho
  gpio_set_function(LED_VERMELHO, GPIO_FUNC_PWM);
  uint fatia_vermelho = pwm_gpio_to_slice_num(LED_VERMELHO);
  pwm_set_wrap(fatia_vermelho, LIMITE_PWM);
  pwm_set_chan_level(fatia_vermelho, PWM_CHAN_A, 0);
  pwm_set_enabled(fatia_vermelho, true);

  // Configuração PWM LED Azul
  gpio_set_function(LED_AZUL, GPIO_FUNC_PWM);
  uint fatia_azul = pwm_gpio_to_slice_num(LED_AZUL);
  pwm_set_wrap(fatia_azul, LIMITE_PWM);
  pwm_set_chan_level(fatia_azul, PWM_CHAN_B, 0);
  pwm_set_enabled(fatia_azul, true);

  // Configuração LED Verde
  gpio_init(LED_VERDE);
  gpio_set_dir(LED_VERDE, GPIO_OUT);
  gpio_put(LED_VERDE, estado_led_verde);

  // Configuração dos Botões
  gpio_init(BOTAO_A);
  gpio_set_dir(BOTAO_A, GPIO_IN);
  gpio_pull_up(BOTAO_A);
  gpio_init(BOTAO_JOYSTICK);
  gpio_set_dir(BOTAO_JOYSTICK, GPIO_IN);
  gpio_pull_up(BOTAO_JOYSTICK);
  gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &callback_gpio);
  gpio_set_irq_enabled(BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true);

  while (true)
  {
    // Leitura ADC
    adc_select_input(JOYSTICK_X_ADC);
    uint16_t adc_x = adc_read();
    adc_select_input(JOYSTICK_Y_ADC);
    uint16_t adc_y = adc_read();

    int diferenca_x = abs((int)adc_x - 2048);
    int diferenca_y = abs((int)adc_y - 2048);
    uint16_t intensidade_vermelho = (diferenca_x * LIMITE_PWM) / 2048;
    uint16_t intensidade_azul = (diferenca_y * LIMITE_PWM) / 2048;

    if (!pwm_ativo || diferenca_x < ZONA_MORTA)
      intensidade_vermelho = 0;
    if (!pwm_ativo || diferenca_y < ZONA_MORTA)
      intensidade_azul = 0;

    pwm_set_chan_level(fatia_vermelho, PWM_CHAN_A, intensidade_vermelho);
    pwm_set_chan_level(fatia_azul, PWM_CHAN_B, intensidade_azul);

    // Atualiza Display
    ssd1306_fill(&ssd, false);
    uint8_t quadrado_x = (adc_y * (128 - 8)) / 4095;
    uint8_t quadrado_y = (adc_x * (64 - 8)) / 4095;
    for (int x = quadrado_x; x < quadrado_x + 8; x++)
      for (int y = quadrado_y; y < quadrado_y + 8; y++)
        ssd1306_pixel(&ssd, x, 63 - y, true);

    if (estilo_borda == 0)
    {
      ssd1306_line(&ssd, 0, 63, 127, 63, true);
      ssd1306_line(&ssd, 0, 63, 0, 0, true);
      ssd1306_line(&ssd, 127, 63, 127, 0, true);
      ssd1306_line(&ssd, 0, 0, 127, 0, true);
    }
    else
    {
      ssd1306_line(&ssd, 2, 61, 125, 61, true);
      ssd1306_line(&ssd, 2, 61, 2, 2, true);
      ssd1306_line(&ssd, 125, 61, 125, 2, true);
      ssd1306_line(&ssd, 2, 2, 125, 2, true);
    }
    ssd1306_send_data(&ssd);
    sleep_ms(100);
  }
  return 0;
}
