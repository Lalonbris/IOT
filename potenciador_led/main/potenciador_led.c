#include "driver/ledc.h" // Librería necesaria para el PWM
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>


#define EJEMPLO_ADC_GPIO ADC_CHANNEL_4 // Canal 4 (GPIO 32 en ESP32 clásico)
#define LED_PWM_GPIO 2 // Pin donde conectaste el LED (cable blanco)

void app_main(void) {
  // =======================================================
  // 1. CONFIGURACIÓN DEL ADC (ENTRADA DEL POTENCIÓMETRO)
  // =======================================================
  adc_oneshot_unit_handle_t adc1_handle;
  adc_oneshot_unit_init_cfg_t init_config1 = {
      .unit_id = ADC_UNIT_1,
      .clk_src = 0,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH_DEFAULT, // Resolución de 12 bits (0 - 4095)
      .atten = ADC_ATTEN_DB_12,         // Rango de voltaje para ESP-IDF
  };
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc1_handle, EJEMPLO_ADC_GPIO, &config));

  // =======================================================
  // 2. CONFIGURACIÓN DEL PWM / LEDC (SALIDA PARA EL LED)
  // =======================================================

  // Configurar el Timer del PWM
  ledc_timer_config_t ledc_timer = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_num = LEDC_TIMER_0,
      .duty_resolution =
          LEDC_TIMER_12_BIT, // Resolución a 12 bits (igual que el ADC)
      .freq_hz = 5000,       // Frecuencia de 5 kHz
      .clk_cfg = LEDC_AUTO_CLK};
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  // Configurar el Canal del PWM
  ledc_channel_config_t ledc_channel = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = LEDC_CHANNEL_0,
                                        .timer_sel = LEDC_TIMER_0,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = LED_PWM_GPIO,
                                        .duty = 0, // Inicia apagado
                                        .hpoint = 0};
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

  // =======================================================
  // 3. BUCLE PRINCIPAL
  // =======================================================
  int raw_value = 0;

  while (1) {
    // Leer el valor del potenciómetro
    ESP_ERROR_CHECK(
        adc_oneshot_read(adc1_handle, EJEMPLO_ADC_GPIO, &raw_value));

    printf("Valor ADC leído: %d | ", raw_value);
    printf("Voltaje aprox: %.2f V\n", raw_value * 3.3 / 4095.0);

    // Actualizar el brillo del LED usando el valor leído (0 a 4095)
    ESP_ERROR_CHECK(
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, raw_value));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

    // Esperar 100 milisegundos (mejor que 1 segundo para ver el cambio más
    // fluido)
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}