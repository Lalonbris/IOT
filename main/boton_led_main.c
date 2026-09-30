#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define LED_PIN 2
#define BTN_PIN 4 // Corregido: Coincide con la conexión D4 de tu placa

void app_main(void) {
  int led_state = 0;

  // Configurar el pin del LED
  gpio_reset_pin(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

  // Configurar el pin del botón
  gpio_reset_pin(BTN_PIN);
  gpio_set_direction(BTN_PIN, GPIO_MODE_INPUT);

  // ¡NUEVO!: Activar la resistencia pull-down interna
  gpio_set_pull_mode(BTN_PIN, GPIO_PULLDOWN_ONLY);

  gpio_set_level(LED_PIN, led_state);

  while (1) {
    // Leer el estado del botón y asignarlo al LED
    led_state = gpio_get_level(BTN_PIN);
    gpio_set_level(LED_PIN, led_state);

    // Un pequeño retraso para evitar que la tarea acapare el procesador y salte
    // el Watchdog
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}