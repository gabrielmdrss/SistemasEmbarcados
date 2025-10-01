// main.c

// 1. Inclusão das bibliotecas
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define DELAY_500_MS          500
#define DELAY_1_S             1000
#define DELAY_10_MS           10

#define LEDC_TIMER            LEDC_TIMER_0
#define LEDC_MODE             LEDC_LOW_SPEED_MODE
#define LEDC_RESOLUTION       LEDC_TIMER_12_BIT
#define LED_RES               4096
#define LEDC_FREQUENCY_HZ     1000 
#define NUM_LEDS              4
#define LEDC_MAX_DUTY         ((1 << LEDC_RESOLUTION) - 1)

#define FREQ_MIN              500
#define FREQ_MAX              2000
#define FREQ_STEP             15
#define DELAY_10_MS           10
#define BUZZER_PIN            14
#define LEDC_TIMER_BUZZER     LEDC_TIMER_1
#define LEDC_MODE_BUZZER      LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_BUZZER   LEDC_CHANNEL_4
#define LEDC_INITIAL_FREQ_BUZZER (1000)

void func_fase1(void)
{
  // --- Loop para aumentar o brilho (Fade Up) ---
  for (int duty = 0; duty <= LED_RES; duty++) {
      // Aplica o mesmo duty cycle para TODOS os canais
      for (int i = 0; i < NUM_LEDS; i++) {
          ledc_set_duty(LEDC_MODE, i, duty);
         ledc_update_duty(LEDC_MODE, i);
      }
      // A pausa acontece DEPOIS de atualizar todos os LEDs
      vTaskDelay(pdMS_TO_TICKS(5));
  }

vTaskDelay(DELAY_500_MS / portTICK_PERIOD_MS);

  // --- Loop para diminuir o brilho (Fade Down) ---
  for (int duty = LED_RES; duty >= 0; duty--) {
      // Aplica o mesmo duty cycle para TODOS os canais
      for (int i = 0; i < NUM_LEDS; i++) {
          ledc_set_duty(LEDC_MODE, i, duty);
          ledc_update_duty(LEDC_MODE, i);
      }
      vTaskDelay(pdMS_TO_TICKS(5));
  }
}
    
void func_fase2(void)
{
  for (int i = 0; i < 4; i++)
  {
    // --- Loop para aumentar o brilho (Fade Up) ---
    for (int duty = 0; duty <= LED_RES; duty+=256) {
        ledc_set_duty(LEDC_MODE, i, duty);
        ledc_update_duty(LEDC_MODE, i);
        vTaskDelay(pdMS_TO_TICKS(22));
    }

    // --- Loop para diminuir o brilho (Fade Down) ---
    for (int duty = LED_RES; duty >= 0; duty-=256) {
      ledc_set_duty(LEDC_MODE, i, duty);
      ledc_update_duty(LEDC_MODE, i);
      vTaskDelay(pdMS_TO_TICKS(22));
    }
  }
  
  for (int i = 3; i > -1; i--)
  {
    // --- Loop para aumentar o brilho (Fade Up) ---
    for (int duty = 0; duty <= LED_RES; duty+=256) {
        ledc_set_duty(LEDC_MODE, i, duty);
        ledc_update_duty(LEDC_MODE, i);
        vTaskDelay(pdMS_TO_TICKS(22));
    }

    // --- Loop para diminuir o brilho (Fade Down) ---
    for (int duty = LED_RES; duty >= 0; duty-=256) {
      ledc_set_duty(LEDC_MODE, i, duty);
      ledc_update_duty(LEDC_MODE, i);
      vTaskDelay(pdMS_TO_TICKS(22));
    }
  }
}

void func_fase3(void)
{
  // Frequência Decrescente
  for (uint32_t freq = FREQ_MIN; freq <= FREQ_MAX; freq += FREQ_STEP)
  {
    ledc_set_freq(LEDC_MODE_BUZZER, LEDC_TIMER_BUZZER, freq);
    ledc_set_duty(LEDC_MODE_BUZZER, LEDC_CHANNEL_BUZZER, LEDC_MAX_DUTY / 2);
    ledc_update_duty(LEDC_MODE_BUZZER, LEDC_CHANNEL_BUZZER);
    vTaskDelay(pdMS_TO_TICKS(DELAY_10_MS));
  }

  // Frequência Decrescente
  for (uint32_t freq = FREQ_MAX; freq >= FREQ_MIN; freq -= FREQ_STEP)
  {
    ledc_set_freq(LEDC_MODE_BUZZER, LEDC_TIMER_BUZZER, freq);
    ledc_set_duty(LEDC_MODE_BUZZER, LEDC_CHANNEL_BUZZER, LEDC_MAX_DUTY / 2);
    ledc_update_duty(LEDC_MODE_BUZZER, LEDC_CHANNEL_BUZZER);
    vTaskDelay(pdMS_TO_TICKS(DELAY_10_MS));
  }

  // Desliga o buzzer
  ledc_set_duty(LEDC_MODE_BUZZER, LEDC_CHANNEL_BUZZER, 0);
  ledc_update_duty(LEDC_MODE_BUZZER, LEDC_CHANNEL_BUZZER);
}

void app_main(void)
{
  // Array com os pinos GPIO que serão utilizados
  int led_gpios[NUM_LEDS] = {4, 5, 6, 7};

  printf("1. Configuracao do LED\n");
  ledc_timer_config_t ledc_timer = {
      .speed_mode       = LEDC_MODE,
      .duty_resolution  = LEDC_RESOLUTION,
      .timer_num        = LEDC_TIMER,
      .freq_hz          = LEDC_FREQUENCY_HZ,
      .clk_cfg          = LEDC_AUTO_CLK
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  for (int i = 0; i < NUM_LEDS; i++) {
      ledc_channel_config_t ledc_channel = {
          .gpio_num   = led_gpios[i],   // Pega o pino do nosso array
          .speed_mode = LEDC_MODE,
          .channel    = i,              // Usa um canal diferente para cada LED (0, 1, 2, 3)
          .timer_sel  = LEDC_TIMER,     // Todos os canais usam o MESMO timer
          .duty       = 0,
          .hpoint     = 0
      };
      ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
  }

  printf("1. Configuracao do TIMER\n");
  ledc_timer_config_t ledc_timer_buzzer = {
    .speed_mode       = LEDC_MODE_BUZZER,
    .timer_num        = LEDC_TIMER_BUZZER,
    .duty_resolution  = LEDC_RESOLUTION,
    .freq_hz          = LEDC_INITIAL_FREQ_BUZZER,
    .clk_cfg          = LEDC_AUTO_CLK
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_buzzer));

  // 4. Configura o canal do Buzzer
  ledc_channel_config_t ledc_channel_buzzer = {
    .speed_mode     = LEDC_MODE_BUZZER,
    .channel        = LEDC_CHANNEL_BUZZER,
    .timer_sel      = LEDC_TIMER_BUZZER, // Usa o timer do buzzer
    .intr_type      = LEDC_INTR_DISABLE,
    .gpio_num       = BUZZER_PIN,
    .duty           = 0, // Inicia com duty 0 (silenciado)
    .hpoint         = 0
  };
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_buzzer));

    while(1) {
      func_fase1();
      vTaskDelay(DELAY_1_S / portTICK_PERIOD_MS);
      func_fase2();
      vTaskDelay(DELAY_1_S / portTICK_PERIOD_MS);
      func_fase3();
      vTaskDelay(DELAY_1_S / portTICK_PERIOD_MS);
    }
}