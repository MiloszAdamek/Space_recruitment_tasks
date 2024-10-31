/*
 * uart_interface.c
 *
 *  Created on: Oct 23, 2024
 *      Author: MiloszAdamek
 */
#include "uart_interface.h"
#include <string.h>  // Dla funkcji strcmp, strtok itp.

static UART_HandleTypeDef *huart_global;  // Wskaźnik na UART
static uint8_t received_data;
static char rx_buffer[MAX_COMMAND_LENGTH];  // Bufor odbiorczy
static uint8_t rx_index = 0;

static Command commands[MAX_COMMANDS];  // Tablica komend
static int command_count = 0;  // Liczba zarejestrowanych komend

// Funkcja inicjalizująca UART i bibliotekę
void UART_Interface_Init(UART_HandleTypeDef *huart) {
    huart_global = huart;
    HAL_UART_Receive_IT(huart_global, &received_data, 1);  // Rozpoczęcie odbioru przez przerwania
}

// Funkcja do rejestrowania nowych komend
void UART_Register_Command(const char *name, void (*callback)(int argc, char *argv[])) {
    if (command_count < MAX_COMMANDS) {
        commands[command_count].name = name;
        commands[command_count].callback = callback;
        command_count++;
    }
}

// Funkcja przetwarzająca odbierane komendy
void UART_Process_Input() {
    char *cmd = strtok(rx_buffer, "=");  // Wyciągnięcie komendy
    char *args = strtok(NULL, "=");  // Wyciągnięcie argumentów

    char msg[] = "Odebrano komende:";
    HAL_UART_Transmit(huart_global, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    HAL_UART_Transmit(huart_global, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY);
    HAL_UART_Transmit(huart_global, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);

    // Przetworzenie argumentów
    char *argv[MAX_ARGS];
    int argc = 0;
    if (args) {
        argv[argc++] = strtok(args, ";");
        while ((argv[argc] = strtok(NULL, ";")) != NULL && argc < MAX_ARGS) {
            argc++;
        }
    }

    // Szukanie komendy w tablicy
    for (int i = 0; i < command_count; i++) {
        if (strcmp(cmd, commands[i].name) == 0) {
            commands[i].callback(argc, argv);  // Wywołanie callbacku
            return;
        }
    }

    // Jeśli nie znaleziono komendy
    char error_msg[] = "Unknown command\r\n";
    HAL_UART_Transmit(huart_global, (uint8_t *)error_msg, sizeof(error_msg) - 1, HAL_MAX_DELAY);
}

// Obsługa przerwań UART (należy wywołać wewnątrz HAL_UART_RxCpltCallback)
void UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == huart_global->Instance) {
        if (received_data != '\r' && received_data != '\n') {
            // Dodajemy znaki do bufora
            if (rx_index < MAX_COMMAND_LENGTH - 1) {
                rx_buffer[rx_index++] = received_data;
            }
        } else if (received_data == '\r' || received_data == '\n') {
            // Zakończono odbiór komendy
            rx_buffer[rx_index] = '\0';  // Zakończenie stringa
            UART_Process_Input();  // Przetworzenie komendy
            rx_index = 0;  // Reset bufora
        }

        // Ponownie włącz odbiór danych
        HAL_UART_Receive_IT(huart_global, &received_data, 1);
    }
}

// Komenda help - wypisuje wszystkie dostępne komendy
void help_command(int argc, char *argv[]) {
    for (int i = 0; i < command_count; i++) {
        HAL_UART_Transmit(huart_global, (uint8_t *)commands[i].name, strlen(commands[i].name), HAL_MAX_DELAY);
        HAL_UART_Transmit(huart_global, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
    }
}
