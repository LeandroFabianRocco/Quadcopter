/*
 * PIDcontroller.c
 *
 *  Created on: 21 mar. 2020
 *      Author: leandro
 */


#include "PIDcontroller.h"


/*********************************************************************************************
 * @brief PID controller for pitch angle
 *
 * @param Pitch data structure
 *
 * @return action control for pitch angle
 *********************************************************************************************/
float getPitchPID(struct pitchStruct *pitchData)
{
	float p_error = pError(pitchData->reference, pitchData->angle);
	float i_error = iError(pitchData->last_iError, p_error, pitchData->dt);
	float d_error = dError(p_error, pitchData->last_pError);

	float pitchPIDoutput = KP_PITCH * p_error + KI_PITCH * i_error + KD_PITCH * d_error;

	pitchData->last_iError = i_error;
	pitchData->last_pError = p_error;

	return pitchPIDoutput;
}


/*********************************************************************************************
 * @brief PID controller for roll angle
 *
 * @param Roll data structure
 *
 * @return action control for roll angle
 *********************************************************************************************/
float getRollPID(struct rollStruct *rollData)
{
	float p_error = pError(rollData->reference, rollData->angle);
	float i_error = iError(rollData->last_iError, p_error, rollData->dt);
	float d_error = dError(p_error, rollData->last_pError);

	float rollPIDoutput = KP_ROLL * p_error + KI_ROLL * i_error + KD_ROLL * d_error;

	rollData->last_iError = i_error;
	rollData->last_pError = p_error;

	return rollPIDoutput;
}


/*********************************************************************************************
 * @brief Estimates the proportional error
 *
 * @param input angle
 * @param output angle
 *
 * @return proportional error
 *********************************************************************************************/
float pError(float input, float output)
{
	return (input - output);
}


/*********************************************************************************************
 * @brief Estimates the integral error
 *
 * @param last integral error
 * @param proportional error
 * @param differential time between errors
 *
 * @return integral error
 *********************************************************************************************/
float iError(float last, float p_error, float dt)
{
	return (last + p_error * dt);
}



/*********************************************************************************************
 * @brief Estimates the derivative error
 *
 * @param proportional error
 * @param last proportional error
 *
 * @return derivative error
 *********************************************************************************************/
float dError(float p_error, float last_p_error)
{
	return (p_error - last_p_error);
}


















