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
float PitchPID(float pitch_new, float pitch_last);


/*********************************************************************************************
 * @brief PID controller for roll angle
 *
 * @param new roll angle
 * @param last roll angle
 *
 * @return action control for roll angle
 *********************************************************************************************/
float RollPID(float roll_new, float roll_last);






#endif /* PIDCONTROLLER_H_ */
