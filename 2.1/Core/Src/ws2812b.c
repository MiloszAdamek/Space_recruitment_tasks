/*
 * ws2812b.c
 *
 *  Created on: Oct 26, 2024
 *      Author: MiloszAdamek
 */
#include "ws2812b.h"

// TIM3 zlicza do 100, okres sygnału 1250ns -> 1250/100 + 12.5ns na cykl

// Protokół WS2812B

#define BIT_0_TIME		32 // '0' 400 ns = 12.5ns * 32
#define BIT_1_TIME		64 // '1' 800 ns = 12.5ns * 64

#define RESET_LEN		40 // Reset > 50000ns = 1250ns * 40
#define LED_N			1 // Liczba diod w szeregu

// Bufor danych
static uint8_t led_buffer[RESET_LEN + 24 * LED_N + 1];

// Funkcja ustawiająca 8 bitów w buforze LED dla konkretnego koloru
static void set_byte(uint32_t pos, uint8_t value)
{
  int i;
  for (i = 0; i < 8; i++) {
    if (value & 0x80) { // Sprawdzenie najstarszego bitu (MSB)
      led_buffer[pos + i] = BIT_1_TIME; // Wpisanie BIT_1_TIME, jeśli MSB jest 1
    } else {
      led_buffer[pos + i] = BIT_0_TIME; // Wpisanie BIT_0_TIME, jeśli MSB jest 0
    }
    value <<= 1; // Przesunięcie w lewo, aby sprawdzić kolejny bit
  }
}

// Funkcja inicjalizująca diody LED
void ws2812b_init(void)
{
  int i;
  for (i = 0; i < RESET_LEN; i++)
    led_buffer[i] = 0; // Wypełnienie resetowego bufora zerami

  for (i = 0; i < 24 * LED_N; i++)
    led_buffer[RESET_LEN + i] = BIT_0_TIME; // Ustawienie początkowych wartości BIT_0_TIME

  led_buffer[RESET_LEN + 24 * LED_N] = 100; // Czas końcowy sygnału resetu

  HAL_TIM_Base_Start(&htim3); // Start timera TIM3
  ws2812b_update(); // Zaktualizowanie LEDów
}

// Funkcja aktualizująca kolory na diodach LED za pomocą DMA
void ws2812b_update(void)
{
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*)led_buffer, sizeof(led_buffer));
}

// Funkcja oczekująca na zakończenie operacji DMA
void ws2812b_wait(void)
{
	while (HAL_TIM_GetChannelState(&htim3, TIM_CHANNEL_1) == HAL_TIM_CHANNEL_STATE_BUSY)
		{}
}

// Funkcja ustawiająca kolor dla pojedynczej diody LED
void ws2812b_set_color(uint32_t led, uint8_t red, uint8_t green, uint8_t blue)
{
	if (led < LED_N) // Zapisywanie w buforze informacji dla poszczególnych kolorów
	{
		set_byte(RESET_LEN + 24 * led, green);
		set_byte(RESET_LEN + 24 * led + 8, red);
		set_byte(RESET_LEN + 24 * led + 16, blue);
	}
}

// Funkcja resetująca diody LED
void ws2812b_reset(void)
{
    // Zerowanie bufora
    for (int i = 0; i < sizeof(led_buffer); i++) {
        led_buffer[i] = 0;
    }

    // Wysłanie bufora zerowego przez DMA
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*)led_buffer, sizeof(led_buffer));
    ws2812b_wait(); // Oczekiwanie na zakończenie resetu
}
