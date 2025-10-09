#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // Usado para vTaskDelay
#include "driver/gpio.h"
#include "esp_timer.h"    // Não é mais usado para debounce, mas bom manter para futuras expansões

#define LED_BIT0_GPIO   GPIO_NUM_7
#define LED_BIT1_GPIO   GPIO_NUM_6
#define LED_BIT2_GPIO   GPIO_NUM_5
#define LED_BIT3_GPIO   GPIO_NUM_4
#define BOTAO_A_GPIO    GPIO_NUM_11 // Botão de Incremento
#define BOTAO_B_GPIO    GPIO_NUM_12 // Botão de Troca de Passo

// --- Constantes ---
#define DEBOUNCE_TIME_MS 50     // Intervalo para ignorar "bounces" no acionamento inicial
#define AUTO_RESET_TIME_MS 20   // Intervalo para rearmar o botão após um acionamento

// --- Variáveis Globais ---
uint8_t contador = 0;
uint8_t incremento = 1;

/**
 * @brief Atualiza o estado dos 4 LEDs para refletir o valor do contador.
 * @param valor O valor de 4 bits (0-15) a ser exibido.
 */
void ajustar_leds(uint8_t valor) {
    gpio_set_level(LED_BIT0_GPIO, (valor >> 0) & 1); // Bit 0 (LSB)
    gpio_set_level(LED_BIT1_GPIO, (valor >> 1) & 1); // Bit 1
    gpio_set_level(LED_BIT2_GPIO, (valor >> 2) & 1); // Bit 2
    gpio_set_level(LED_BIT3_GPIO, (valor >> 3) & 1); // Bit 3 (MSB)
}

/**
 * @brief Lê o estado dos botões usando uma flag de estado não-bloqueante.
 * Esta função é chamada repetidamente no loop principal.
 */
void ler_botoes() {
    // Flags estáticas para controlar se a ação do botão já foi executada
    static bool botao_a_processado = false;
    static bool botao_b_processado = false;

    // Variáveis estáticas para guardar o tempo do último acionamento válido
    static int64_t tempo_ultimo_acionamento_a = 0;
    static int64_t tempo_ultimo_acionamento_b = 0;
    int64_t tempo_atual = esp_timer_get_time() / 1000; // Tempo atual em milissegundos

    // --- Lógica para o Botão A (Incremento) ---
    int estado_atual_a = gpio_get_level(BOTAO_A_GPIO);

    // 1. Detecta o pressionamento com debounce inicial de 50ms
    if (estado_atual_a == 1 && !botao_a_processado) {
        // Debounce: Apenas considera o primeiro pulso após um período de calma
        if (tempo_atual - tempo_ultimo_acionamento_a > DEBOUNCE_TIME_MS) {
            botao_a_processado = true;
            tempo_ultimo_acionamento_a = tempo_atual;

            contador = (contador + incremento) % 16;
            printf("Botão A: Contador incrementado para %d\n", contador);
            ajustar_leds(contador);
        }
    }

    // 2. LÓGICA ALTERADA: Rearma a flag automaticamente após 20ms
    // Se a flag está ativa E já se passaram 20ms desde o acionamento...
    if (botao_a_processado && estado_atual_a == 0 &&(tempo_atual - tempo_ultimo_acionamento_a > AUTO_RESET_TIME_MS)) {
        botao_a_processado = false; // Rearma a flag, permitindo um novo acionamento
    }

    // --- Lógica para o Botão B (Troca de Incremento) ---
    int estado_atual_b = gpio_get_level(BOTAO_B_GPIO);

    if (estado_atual_b == 1 && !botao_b_processado) {
        if (tempo_atual - tempo_ultimo_acionamento_b > DEBOUNCE_TIME_MS) {
            botao_b_processado = true;
            tempo_ultimo_acionamento_b = tempo_atual;

            incremento = (incremento == 1) ? 2 : 1;
            printf("Botão B: Incremento alterado para +%d\n", incremento);
        }
    }
    
    // Rearma a flag do botão B automaticamente após 20ms
    if (botao_b_processado && estado_atual_b == 0 && (tempo_atual - tempo_ultimo_acionamento_b > AUTO_RESET_TIME_MS)) {
        botao_b_processado = false;
    }
}

void app_main(void) {
    printf("Iniciando o contador binário (modo polling com flag de estado).\n");

    // --- Configuração dos GPIOs dos LEDs ---
    gpio_config_t io_conf_leds = {
        .pin_bit_mask = (1ULL << LED_BIT0_GPIO) | (1ULL << LED_BIT1_GPIO) | (1ULL << LED_BIT2_GPIO) | (1ULL << LED_BIT3_GPIO),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf_leds);

    // --- Configuração dos GPIOs dos Botões ---
    gpio_config_t io_conf_botoes = {
        .pin_bit_mask = (1ULL << BOTAO_A_GPIO) | (1ULL << BOTAO_B_GPIO),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE, // Interrupções desabilitadas
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf_botoes);
    
    ajustar_leds(contador);

    while (1) {
        ler_botoes(); // Chama a função de leitura dos botões repetidamente
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}