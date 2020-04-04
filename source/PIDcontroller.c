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
 * @param input for controller --> reference
 * @param output angle from quadcopter
 * @param last integral error
 * @param differential of time
 * @param last proportional error
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
 * @param input for controller --> reference
 * @param output angle from quadcopter
 * @param last integral error
 * @param differential of time
 * @param last proportional error
 *
 * @return action control for roll angle
 *********************************************************************************************/
float getRollPID(float input, float output, float last_i_error, float dt, float last_p_error)
{
	float p_error = pError(input, output);
	float i_error = iError(last_i_error, p_error, dt);
	float d_error = dError(p_error, last_p_error);
	float rollPIDoutput = KP_PITCH * p_error + KI_PITCH * i_error + KD_PITCH * d_error;
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


















