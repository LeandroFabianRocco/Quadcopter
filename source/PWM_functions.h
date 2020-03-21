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
 * Definitions
 ************************************************************************/
#define FTM_MODULE FTM0
#define PWM_CH0 0U
#define PWM_CH1 1U
#define PWM_CH2 2U
#define PWM_CH3 3U

#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_McgFixedFreqClk)

#define MAX_THROTTLE 196U
#define MIN_THROTTLE 97U

/*********************************************************************************************
 * @brief Set the CnV field for PWM channels
 *
 * @param FTM peripheral base address.
 * @param value received from Quadcopter control
 * @param PWM channel
 *
 * @return void
 *********************************************************************************************/
void set_pwm_CnV(FTM_Type *base, uint32_t value, uint8_t ch);


/*********************************************************************************************
 * @brief FTM channels initialization
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
void FTM0_init(void);


#endif /* PWM_FUNCTIONS_H_ */
