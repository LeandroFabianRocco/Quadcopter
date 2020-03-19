/*
 * PWM_functions.c
 *
 *  Created on: 10 ene. 2020
 *      Author: leandro
 */

#include "PWM_functions.h"

/*************************************************************************
 * Función para setear el valor de CnV de los canales del FTM
 ************************************************************************/
void set_pwm_CnV(uint32_t value, uint8_t ch)
{
	if (value > 100)
		value = 100;
	float x1 = value + 97;
	uint32_t v = (uint32_t)x1;
	FTM_MODULE->CONTROLS[ch].CnV = v;
	FTM_SetSoftwareTrigger(FTM_MODULE, true);
}


/*************************************************************************
 * Configura el ESC para el rango de velocidades del PWM
 ************************************************************************/
void throttle_init(void)
{
	//GPIO_PortClear(BLUE_LED_GPIO, 1u << BLUE_LED);
	//SysTick_DelayTicks(5000U); // Tiempo de espera para conectar la batería
	// Configuro máximo throttle
	set_pwm_CnV(MAX_THROTTLE, PWM_CH0);
	set_pwm_CnV(MAX_THROTTLE, PWM_CH1);
	SysTick_DelayTicks(5000U);
	GPIO_PortSet(BLUE_LED_GPIO, 1u << BLUE_LED);
	set_pwm_CnV(MIN_THROTTLE, PWM_CH0);
	set_pwm_CnV(MIN_THROTTLE, PWM_CH1);
	SysTick_DelayTicks(1000U);
}


void FTM0_init(void)
{
	ftm_chnl_pwm_signal_param_t ftmParam[4];
	ftm_config_t ftmInfo;

	ftmParam[0].chnlNumber            = (ftm_chnl_t)PWM_CH0;
	ftmParam[0].level                 = kFTM_HighTrue;
	ftmParam[0].dutyCyclePercent      = 5U;
	ftmParam[0].firstEdgeDelayPercent = 0U;
	ftmParam[0].enableDeadtime        = false;

	ftmParam[1].chnlNumber            = (ftm_chnl_t)PWM_CH1;
	ftmParam[1].level                 = kFTM_HighTrue;
	ftmParam[1].dutyCyclePercent      = 5U;
	ftmParam[1].firstEdgeDelayPercent = 0U;
	ftmParam[1].enableDeadtime        = false;

	ftmParam[2].chnlNumber            = (ftm_chnl_t)PWM_CH2;
	ftmParam[2].level                 = kFTM_HighTrue;
	ftmParam[2].dutyCyclePercent      = 5U;
	ftmParam[2].firstEdgeDelayPercent = 0U;
	ftmParam[2].enableDeadtime        = false;

	ftmParam[3].chnlNumber            = (ftm_chnl_t)PWM_CH3;
	ftmParam[3].level                 = kFTM_HighTrue;
	ftmParam[3].dutyCyclePercent      = 5U;
	ftmParam[3].firstEdgeDelayPercent = 0U;
	ftmParam[3].enableDeadtime        = false;

	FTM_GetDefaultConfig(&ftmInfo);
	FTM_Init(FTM_MODULE, &ftmInfo);
	FTM_SetupPwm(FTM_MODULE, ftmParam, 4U, kFTM_EdgeAlignedPwm, 50U, FTM_SOURCE_CLOCK);
	FTM_StartTimer(FTM_MODULE, kFTM_FixedClock);
}












