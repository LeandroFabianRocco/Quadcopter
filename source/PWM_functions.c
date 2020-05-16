/*
 * PWM_functions.c
 *
 *  Created on: 10 ene. 2020
 *      Author: leandro
 */

#include "PWM_functions.h"

/*********************************************************************************************
 * @brief Set the CnV field for PWM channels
 *
 * @param FTM peripheral base address.
 * @param value received from Quadcopter control
 * @param PWM channel
 *
 * @return void
 *********************************************************************************************/
void set_pwm_CnV(FTM_Type *base, int32_t value, uint8_t ch)
{
	if (value > CnV_MAX)
		value = CnV_MAX;
	if (value < CnV_MIN)
		value = CnV_MIN;
	base->CONTROLS[ch].CnV = value;
	FTM_SetSoftwareTrigger(base, true);
}


/*********************************************************************************************
 * @brief FTM channels initialization
 *
 * @param FTM module pointer
 *
 * @return void
 *********************************************************************************************/
void FTM0_init(FTM_Type *base)
{
	ftm_chnl_pwm_signal_param_t ftmParam[4];
	ftm_config_t ftmInfo;

	// 400Hz
	uint8_t duty = 40U;
	uint16_t period = 400U;

	// 250Hz
	/*uint8_t duty = 25U;
	uint16_t period = 250U;*/

	// 50Hz
	/*uint8_t duty = 5U;
	uint8_t period = 50U;*/

	ftmParam[0].chnlNumber            = (ftm_chnl_t)PWM_CH0;
	ftmParam[0].level                 = kFTM_HighTrue;
	ftmParam[0].dutyCyclePercent      = duty;
	ftmParam[0].firstEdgeDelayPercent = 0U;
	ftmParam[0].enableDeadtime        = false;

	ftmParam[1].chnlNumber            = (ftm_chnl_t)PWM_CH1;
	ftmParam[1].level                 = kFTM_HighTrue;
	ftmParam[1].dutyCyclePercent      = duty;
	ftmParam[1].firstEdgeDelayPercent = 0U;
	ftmParam[1].enableDeadtime        = false;

	ftmParam[2].chnlNumber            = (ftm_chnl_t)PWM_CH2;
	ftmParam[2].level                 = kFTM_HighTrue;
	ftmParam[2].dutyCyclePercent      = duty;
	ftmParam[2].firstEdgeDelayPercent = 0U;
	ftmParam[2].enableDeadtime        = false;

	ftmParam[3].chnlNumber            = (ftm_chnl_t)PWM_CH3;
	ftmParam[3].level                 = kFTM_HighTrue;
	ftmParam[3].dutyCyclePercent      = duty;
	ftmParam[3].firstEdgeDelayPercent = 0U;
	ftmParam[3].enableDeadtime        = false;

	FTM_GetDefaultConfig(&ftmInfo);
	FTM_Init(base, &ftmInfo);
	FTM_SetupPwm(base, ftmParam, 4U, kFTM_EdgeAlignedPwm, period, FTM_SOURCE_CLOCK);
	FTM_StartTimer(base, kFTM_FixedClock);
}












