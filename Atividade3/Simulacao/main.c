#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED1 4
#define LED2 5
#define LED3 6
#define LED4 7
#define DELAY_MS 500

void contador_Binario(void)
{
  int contador_aux = 0;

  for(int i=0; i<16; i++)
  {
    contador_aux = i+1;

    if(contador_aux >= 8){ gpio_set_level(LED4, 1); contador_aux -= 8;}
    else gpio_set_level(LED4, 0);

    if(contador_aux >= 4){ gpio_set_level(LED3, 1); contador_aux -= 4;}
    else gpio_set_level(LED3, 0);

    if(contador_aux >= 2){ gpio_set_level(LED2, 1); contador_aux -= 2;}
    else gpio_set_level(LED2, 0);

    if(contador_aux >= 1){ gpio_set_level(LED1, 1); contador_aux -= 1;}
    else gpio_set_level(LED1, 0);

    vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
  }
}

void led_Deslizante(int sentido)
{
  gpio_set_level(LED1, 0);
  gpio_set_level(LED2, 0);
  gpio_set_level(LED3, 0);
  gpio_set_level(LED4, 0);
  vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);

  if(sentido == 1)
  {
    for(int i = 0; i<4; i++)
    {
      gpio_set_level(i+4, 1);
      vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
      gpio_set_level(i+4, 0);
    }
  
  } else 
  {
    for(int i = 0; i<4; i++)
    {
      gpio_set_level(7-i, 1);
      vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
      gpio_set_level(7-i, 0);
    }
  }
}

void app_main() {

  gpio_reset_pin(LED1);
  gpio_reset_pin(LED2);
  gpio_reset_pin(LED3);
  gpio_reset_pin(LED4);

  /* Define a direção do GPIO - Saída */
  gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED3, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED4, GPIO_MODE_OUTPUT);

  while (true) {

    contador_Binario();
    led_Deslizante(1);
    led_Deslizante(-1);
  }
}
