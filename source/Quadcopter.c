/********************************************************************************************
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ********************************************************************************************/
 
/**
 * @file    Quadcopter.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "fsl_uart.h"
//#include "Delays.h"
#include "RGB_LEDS.h"
#include "PWM_functions.h"
#include "MPU6050.h"
//#include "FXOS8700CQ.h"
#include "PIDcontroller.h"
#include "fsl_lptmr.h"
#include "UART4_DMA.h"
#include "fsl_pit.h"
/*******************************************************************************
 * Variable definition
 ******************************************************************************/

// UART4 definitions
#define UART UART4
#define UART_CLKSRC UART4_CLK_SRC
#define UART_CLK_FREQ CLOCK_GetFreq(UART4_CLK_SRC)
#define RX_RING_BUFFER_SIZE 11U

// Savitzky-Golay filter parameters
#define SG_FILTER_SIZE 11


// Move constant
#define MOOVE 10

// LPTMR0 timer definitions
#define LPTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_McgInternalRefClk)
#define LPTMR_USEC_COUNT 1000U


// Bluetooth data definitions
#define HEADER_1_ASCII	0xAA
#define HEADER_2_ASCII	0xA8
#define MEDIUM_ASCII	0x40
#define TAIL_1_ASCII	0xB8
#define TAIL_2_ASCII	0xA9


// Time differential
#define DT 0.00012288
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void MotorUpdate(uint8_t throttle, int8_t pitchPID, int8_t rollPID);

void commands_to_reference(uint8_t joystick);

float roll_sgolayfilt(float data);

float pitch_sgolayfilt(float data);

uint8_t increment_buff_index(uint8_t index);
/*******************************************************************************
 * Variables declaration
 ******************************************************************************/

// UART4 variables
uart_handle_t uartHandle;
uint8_t rxTempBuffer[UART_RING_BUFFER_SIZE] = {0};
uint8_t rxRingBuffer[UART_RING_BUFFER_SIZE] = {0};
volatile bool rxOnGoing = true;

// Joystick and throttle values
uint8_t joystick, throttle;

// Variables to controlling the BLDC motors
volatile int8_t Mfront, Mfront_last;	// Front motor
volatile int8_t Mleft, Mleft_last;		// Left motor
volatile int8_t Mback, Mback_last;		// Back motor
volatile int8_t Mright, Mright_last;	// Right motor

// Reference for pitch and roll angles
volatile uint8_t pitch_ref = 0;
volatile uint8_t roll_ref = 0;
// PID outputs for pitch and roll
volatile float pitchPID = 0;
volatile float rollPID = 0;
// Pitch and roll current angles
volatile float pitchAngle, rollAngle;
// Integral and proportional errors
volatile float i_error = 0;
volatile float p_error = 0;
// Time differential
volatile float dt, dt_sec;

// MPU6050 Who_Am_I flag
bool isThereAccelMPU = false;
// FXOS8700CQ Who_Am_I angles
bool isThereAccelFX = false;

// Timer variable
uint32_t LPTMRtime = 0;



volatile bool pitIsrFlag = false;



/*******************************************************************************
 * Get Joystick and throttle values
 ******************************************************************************/
uint8_t increment_buff_index(uint8_t index)
{
	if (index >= (RX_RING_BUFFER_SIZE - 1))
		index = 0;
	else
		index++;

	return index;
}

void get_J_and_T(void)
{
	uint8_t i;
	uint8_t idx, head;
	uint8_t t[3] = {throttle, throttle, throttle};
	uint8_t j[3] = {joystick, joystick, joystick};

	for (i = 0; i < RX_RING_BUFFER_SIZE; i++)
	{
		if (rxRingBuffer[i] == HEADER_1_ASCII)
		{
			idx = i;
			head = i;
			break;
		}
	}

	if (rxRingBuffer[idx] == HEADER_1_ASCII) // First header received?
	{
		idx = increment_buff_index(idx);
		if (rxRingBuffer[idx] == HEADER_2_ASCII) // Second header received?
		{
			idx = increment_buff_index(idx);
			idx = increment_buff_index(idx);
			idx = increment_buff_index(idx);
			idx = increment_buff_index(idx);
			if (rxRingBuffer[idx] == MEDIUM_ASCII) // Medium character received
			{
				idx = increment_buff_index(idx);
				idx = increment_buff_index(idx);
				idx = increment_buff_index(idx);
				idx = increment_buff_index(idx);
				if (rxRingBuffer[idx] == TAIL_1_ASCII) // First tail received
				{
					idx = increment_buff_index(idx);
					if (rxRingBuffer[idx] == TAIL_2_ASCII) // Second tail received
					{
						head = increment_buff_index(head);
						head = increment_buff_index(head);
						t[0] = rxRingBuffer[head];
						head = increment_buff_index(head);
						t[1] = rxRingBuffer[head];
						head = increment_buff_index(head);
						t[2] = rxRingBuffer[head];
						head = increment_buff_index(head);
						head = increment_buff_index(head);
						j[0] = rxRingBuffer[head];
						head = increment_buff_index(head);
						j[1] = rxRingBuffer[head];
						head = increment_buff_index(head);
						j[2] = rxRingBuffer[head];
					}
				}
			}
		}
	}

	if (t[0] == t[1])
		throttle = t[0];
	else if (t[0] == t[2])
		throttle = t[2];
	else if (t[1] == t[2])
		throttle = t[1];

	if (j[0] == j[1])
		joystick = j[0];
	else if (j[0] == j[2])
		joystick = j[2];
	else if (j[1] == j[2])
		joystick = j[1];

}


/*******************************************************************************
 * Update motors values as function of joystick values
 ******************************************************************************/
void commands_to_reference(uint8_t joystick)
{
	switch(joystick)
	{
		case 0x01: // UP
			RedLEDon();
			GreenLEDoff();
			BlueLEDoff();
			break;
		case 0x02: // UP-RIGHT
			RedLEDon();
			GreenLEDon();
			BlueLEDoff();
			break;
		case 0x04: // RIGHT
			RedLEDoff();
			GreenLEDon();
			BlueLEDoff();
			break;
		case 0x08: // DOWN-RIGHT
			RedLEDon();
			GreenLEDoff();
			BlueLEDon();
			break;
		case 0x10: // DOWN
			RedLEDoff();
			GreenLEDoff();
			BlueLEDon();
			break;
		case 0x20: // DOWN-LEFT
			RedLEDoff();
			GreenLEDon();
			BlueLEDon();
			break;
		case 0x40: // LEFT
			RedLEDon();
			GreenLEDon();
			BlueLEDon();
			break;
		case 0x80: // UP-LEFT
			RedLEDon();
			GreenLEDoff();
			BlueLEDon();
			break;
		default:
			RedLEDoff();
			GreenLEDoff();
			BlueLEDoff();
	}
}


/*******************************************************************************
 * Update motors values
 ******************************************************************************/
void MotorUpdate(uint8_t throttle, int8_t pitchPID, int8_t rollPID)
{
	// Front motor
	/*Mfront = throttle + pitchPID;// - yawPID;
	if (Mfront_last != Mfront)
	{
		set_pwm_CnV(FTM0, Mfront, PWM_CH0);
		Mfront_last = Mfront;
	}*/
	// Back motor
	/*Mback = throttle - pitchPID; // - yawPID;
	if (Mback_last != Mback)
	{
		set_pwm_CnV(FTM0, Mback, PWM_CH2);
		Mback_last = Mback;
	}*/
	Mfront = 0;
	Mback = 0;
	// Left motor
	Mleft = throttle - rollPID; // + yawPID;
	if (Mleft_last != Mleft)
	{
		set_pwm_CnV(FTM0, Mleft, PWM_CH1);
		Mleft_last = Mleft;
	}
	// Right motor
	Mright = throttle + rollPID; // + yawPID;
	if (Mright_last != Mright)
	{
		set_pwm_CnV(FTM0, Mright, PWM_CH3);
		Mright_last = Mright;
	}
}


/*******************************************************************************
 * PIT module interrupt handler
 ******************************************************************************/
void PIT_0_IRQHANDLER(void)
{
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
	pitIsrFlag = true;
	__DSB();
}


/*******************************************************************************
 * MAIN function
 ******************************************************************************/
int main(void)
{
	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitBootPeripherals();
	SysTick_init();
	// Inicializo el LED RGB
	RGB_LED_init();
	// FTM init
	FTM0_init();

	// MPU6050 initialization and configuration
	MPU6050_Init();
	MPU6050_Configure_Device();
	isThereAccelMPU = MPU6050_ReadSensorWhoAmI();

	uint32_t byteCount = 0U;
	uint8_t rxIndex = 0U;
	uint8_t i;
	/* Initialize the UART configurations. */
	InitUART4();
	/* Initialize the EDMA configuration for UART trasnfer. */
	InitEDMA();
	/* Start ring buffer. */
	StartRingBufferEDMA();


	// FXOS8700 initialization and configuration
	//FXOS8700CQ_Init();
	//FXOS8700CQ_Configure_Device();
	//isThereAccelFX = FXOS8700CQ_ReadSensorWhoAmI();

	// MPU6050 angles structure
	struct MPU6050_angles mpu_angles;
	mpu_angles.x = 0;
	mpu_angles.y = 0;
	mpu_angles.dt = DT;


	// Pitch structure inicialization
	struct pitchStruct pitchData;
	pitchData.reference = pitch_ref;
	pitchData.angle = pitchAngle;
	pitchData.last_iError = i_error;
	pitchData.last_pError = p_error;
	pitchData.dt = DT;

	// Roll structure inicialization
	struct rollStruct rollData;
	rollData.reference = roll_ref;
	rollData.angle = rollAngle;
	rollData.last_iError = i_error;
	rollData.last_pError = p_error;
	rollData.dt = DT;



	PIT_StartTimer(PIT_PERIPHERAL, PIT_0);

	// Main loop
	while (1)
	{
		//SysTick_DelayTicks(10U);
		while (!pitIsrFlag){}
		pitIsrFlag = false;
		/******************************************************************
		 * Read commands from bluetooth module
		 ******************************************************************/
		byteCount = GetRingBufferLengthEDMA();
		if (0U != byteCount)
		{
			MoveDataToLocalBuffer(rxTempBuffer, byteCount);
			for (i = 0; i < byteCount; i++)
			{
				rxRingBuffer[rxIndex] = rxTempBuffer[i];
				rxIndex++;
				if (rxIndex == UART_RING_BUFFER_SIZE)
					rxIndex = 0;
			}
			get_J_and_T();
		}
		/******************************************************************
		 * Update motors from commands
		 ******************************************************************/
		commands_to_reference(joystick);
		/******************************************************************
		 * Read angles from MPU6050
		 ******************************************************************/
		if (isThereAccelMPU)
		{
			MPU6050_ComplementaryFilterAngles(&mpu_angles);
			rollData.angle = mpu_angles.y;
			pitchData.angle = mpu_angles.x;
		}
		/******************************************************************
		 * PID controller for pitch angle
		 ******************************************************************/
		if (throttle >= 5)
		{
			pitchPID = getPitchPID(&pitchData);
			rollPID = getRollPID(&rollData);
		}
		else
		{
			pitchPID = 0;
			rollPID = 0;
		}
		/******************************************************************
		 * Update Motors throttle
		 ******************************************************************/
		MotorUpdate(throttle, pitchPID, rollPID);

		/******************************************************************
		 * Prints
		 ******************************************************************/
		//PRINTF("front = %3d, back = %3d, left = %3d, right = %3d\r\n", Mfront, Mback, Mleft, Mright);
		/*PRINTF("throttle = %3d, pitch = %3.2f, filteredRoll = %3.2f, pitchPID = %3.2f, Mfront = %3d, Mback = %3d\r\n",
				throttle, mpu_angles.x ,pitchData.angle, pitchPID, Mfront, Mback);*/
		/*PRINTF("throttle = %3d, pitch = %f, roll = %f, pitchPID = %f, rollPID = %f, Mfront = %3d, Mleft = %3d, Mback = %3d, Mright = %3d\r\n",
				throttle,
				pitchData.angle,
				rollData.angle,
				pitchPID,
				rollPID,
				Mfront,
				Mleft,
				Mback,
				Mright);*/
		PRINTF("throttle = %3d, roll = %f, rollPID = %f, Mleft = %3d, Mright = %3d\r\n",
				throttle,
				rollData.angle,
				rollPID,
				Mleft,
				Mright);
	}
}









