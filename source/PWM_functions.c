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
	FTM_MODULE->CONTROLS[ch].CnV = (uint32_t) (value * 18.55 + 97);
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

