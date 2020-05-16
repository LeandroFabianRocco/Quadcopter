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
#include "fsl_debug_console.h"
/*************************************************************************
 * Definitions
 ************************************************************************/
#define FTM_MODULE FTM0
#define PWM_CH0  0U
#define PWM_CH1  1U
#define PWM_CH2  2U
#define PWM_CH3  3U
#define LOOPTIME 4U

#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_McgFixedFreqClk)


#define CnV_MAX	782U
#define CnV_MIN	391U


/* FTM0 interrupt vector ID (number). */
#define FTM0_IRQN FTM0_IRQn
/* FTM0 interrupt handler identifier. */
#define FTM0_IRQHANDLER FTM0_IRQHandler

/*********************************************************************************************
 * @brief Set the CnV field for PWM channels
 *
 * @param FTM peripheral base address.
 * @param value received from Quadcopter control
 * @param PWM channel
 *
 * @return void
 *********************************************************************************************/
void set_pwm_CnV(FTM_Type *base, int32_t value, uint8_t ch);


/*********************************************************************************************
 * @brief FTM channels initialization
 *
 * @param FTM module pointer
 *
 * @return void
 *********************************************************************************************/
void FTM0_init(FTM_Type *base);


#endif /* PWM_FUNCTIONS_H_ */
