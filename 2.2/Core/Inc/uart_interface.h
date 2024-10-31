/*
 * uart.h
 *
 *  Created on: Oct 23, 2024
 *      Author: MiloszAdamek
 */

#ifndef UART_INTERFACE_H

#define UART_INTERFACE_H

#include "stm32g4xx_hal.h"  // Dostosuj do swojego modelu STM32

#define MAX_COMMANDS 10     // Maksymalna liczba komend
#define MAX_ARGS 5          // Maksymalna liczba argumentów
#define MAX_COMMAND_LENGTH 50 // Maksymalna długość komendy

typedef struct {
    const char *name;                             // Nazwa komendy
    void (*callback)(int argc, char *argv[]);     // Funkcja callback
} Command;

// Funkcja inicjalizująca UART i bibliotekę
void UART_Interface_Init(UART_HandleTypeDef *huart);

// Funkcja do rejestrowania komend
void UART_Register_Command(const char *name, void (*callback)(int argc, char *argv[]));

// Funkcja obsługi odbioru danych
void UART_Process_Input();

// Przerwanie UART (należy wywołać wewnątrz HAL_UART_RxCpltCallback)
void UART_RxCpltCallback(UART_HandleTypeDef *huart);

// Definicja przykładowej komendy "help"
void help_command(int argc, char *argv[]);

#endif // UART_INTERFACE_H

