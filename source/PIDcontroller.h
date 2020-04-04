/*
 * PIDcontroller.h
 *
 *  Created on: 21 mar. 2020
 *      Author: leandro
 */

#ifndef PIDCONTROLLER_H_
#define PIDCONTROLLER_H_

/*************************************************************************
 * Definitions
 ************************************************************************/
#define PITCH_REFERENCE	0U
#define ROLL_REFERENCE	0U



/*********************************************************************************************
 * @brief PID controller for pitch angle
 *
 * @param new pitch angle
 * @param last pitch angle
 *
 * @return action control for pitch angle
 *********************************************************************************************/
float getPitchPID(float pitch_new, float pitch_last);


/*********************************************************************************************
 * @brief PID controller for roll angle
 *
 * @param new roll angle
 * @param last roll angle
 *
 * @return action control for roll angle
 *********************************************************************************************/
float getRollPID(float roll_new, float roll_last);



/*********************************************************************************************
 * @brief Estimates the proportional error
 *
 * @param input angle
 * @param output angle
 *
 * @return proportional error
 *********************************************************************************************/
float pError(float input, float output);



/*********************************************************************************************
 * @brief Estimates the integral error
 *
 * @param last integral error
 * @param proportional error
 * @param differential time between errors
 *
 * @return integral error
 *********************************************************************************************/
float iError(float last, float p_error, float dt);


/*********************************************************************************************
 * @brief Estimates the derivative error
 *
 * @param proportional error
 * @param last proportional error
 *
 * @return derivative error
 *********************************************************************************************/
float dError(float p_error, float last_p_error);




#endif /* PIDCONTROLLER_H_ */
