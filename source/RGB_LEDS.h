/*
 * RGB_LEDS.h
 *
 *  Created on: 14 abr. 2019
 *      Author: leandro
 */

#ifndef RGB_LEDS_H_
#define RGB_LEDS_H_


#include "pin_mux.h"
#include "board.h"


/******************************************************************************************************************
 * Definiciones
 ******************************************************************************************************************/
// GPIO
#define RED_LED_GPIO BOARD_LED_RED_GPIO
#define GREEN_LED_GPIO BOARD_LED_GREEN_GPIO
#define BLUE_LED_GPIO BOARD_LED_BLUE_GPIO
#define SW2_MAX_GPIO BOARD_SW2_GPIO
#define SW3_MIN_GPIO BOARD_SW3_GPIO
// Pines
#define RED_LED BOARD_LED_RED_GPIO_PIN
#define GREEN_LED BOARD_LED_GREEN_GPIO_PIN
#define BLUE_LED BOARD_LED_BLUE_GPIO_PIN
#define SW2_MAX_PIN BOARD_SW2_PIN
#define SW3_MIN_PIN BOARD_SW3_PIN

/******************************************************************************************************************
 * Funciones macros
 ******************************************************************************************************************/
void RGB_LED_init(void);

#endif /* RGB_LEDS_H_ */
