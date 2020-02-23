/*
 * PWM_functions.h
 *
 *  Created on: 10 ene. 2020
 *      Author: leandro
 */

#ifndef PWM_FUNCTIONS_H_
#define PWM_FUNCTIONS_H_

/*************************************************************************
 * Includes
 ************************************************************************/
#include "fsl_ftm.h"
#include "stdint.h"
#include "Delays.h"
#include "RGB_LEDS.h"

/*************************************************************************
 * Definiciones
 ************************************************************************/
#define FTM_MODULE FTM0
#define PWM_CH0 0U
#define PWM_CH1 1U
#define PWM_CH2 2U
#define PWM_CH3 3U

#define MAX_THROTTLE 196U
#define MIN_THROTTLE 97U

/*************************************************************************
 * Prototipos
 ************************************************************************/
void set_pwm_CnV(uint32_t value, uint8_t ch);

void throttle_init(void);

#endif /* PWM_FUNCTIONS_H_ */
