/*
 * ws2812b.h
 *
 *  Created on: Oct 26, 2024
 *      Author: MiloszAdamek
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#pragma once

#include "stm32g4xx_hal.h"
#include "tim.h"

void ws2812b_init(void);

void ws2812b_set_color(uint32_t led, uint8_t red, uint8_t green, uint8_t blue);

void ws2812b_update(void);

void ws2812b_wait(void);

void ws2812b_reset(void);

#endif /* INC_WS2812B_H_ */
