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
#include "Delays.h"
#include "RGB_LEDS.h"
#include "PWM_functions.h"
#include "MPU6050.h"
//#include "FXOS8700CQ.h"
#include "PIDcontroller.h"
#include "fsl_lptmr.h"

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
#define LPTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_LpoClk)
#define LPTMR_USEC_COUNT 1000000U


// Pre-processor definitions
//#define SG_filter		1
//#define FIR_filter 	1


// Bluetooth data definitions
#define HEADER_1_ASCII	0xAA
#define HEADER_2_ASCII	0xA8
#define MEDIUM_ASCII	0x40
#define TAIL_1_ASCII	0xB8
#define TAIL_2_ASCII	0xA9

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* UART user callback */
void UART_UserCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData);

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
uint8_t rxRingBuffer[RX_RING_BUFFER_SIZE] = {0};
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


// Savitzky-Golay filter variables
float rollCircularBuffer[SG_FILTER_SIZE] = {0};
float pitchCircularBuffer[SG_FILTER_SIZE] = {0};
uint8_t rollWriteIndex = 0;
uint8_t pitchWriteIndex = 0;
float sg_coef[SG_FILTER_SIZE] = {89.0, 84.0, 69.0, 44.0, 9.0, -36.0, 9.0, 44.0, 69.0, 84.0, 89.0};
float sg_h = 429.0;


/*******************************************************************************
 * Function to increment index
 ******************************************************************************/
uint8_t incrementIndex(uint8_t index)
{
	if (index >= (SG_FILTER_SIZE - 1))
		index = 0;
	else
		index++;

	return index;
}

/*******************************************************************************
 * Roll and pitch Savitzky-Golay filter using circular buffer
 ******************************************************************************/
#ifdef SG_filter
float roll_sgolayfilt(float data)
{
	uint8_t i;
	float sum = 0.0;

	rollCircularBuffer[rollWriteIndex] = data;

	uint8_t rollReadIndex = rollWriteIndex;

	rollWriteIndex = incrementIndex(rollWriteIndex);

	for (i = 0; i < SG_FILTER_SIZE; i++)
	{
		sum += rollCircularBuffer[rollReadIndex] * sg_coef[i];
		rollReadIndex = incrementIndex(rollReadIndex);
	}

	sum = sum / sg_h;

	return sum;
}

float pitch_sgolayfilt(float data)
{
	uint8_t i;
	float sum = 0.0;

	pitchCircularBuffer[pitchWriteIndex] = data;

	uint8_t pitchReadIndex = pitchWriteIndex;

	pitchWriteIndex = incrementIndex(pitchWriteIndex);

	for (i = 0; i < SG_FILTER_SIZE; i++)
	{
		sum += pitchCircularBuffer[pitchReadIndex] * sg_coef[i];
		pitchReadIndex = incrementIndex(pitchReadIndex);
	}

	sum = sum / sg_h;

	return sum;
}
#elif FIR_filter
float roll_FIRfilt(float data)
{
	uint8_t i;
	float sum = 0.0;

	rollCircularBuffer[rollWriteIndex] = data;

	uint8_t rollReadIndex = rollWriteIndex;

	rollWriteIndex = incrementIndex(rollWriteIndex);

	for (i = 0; i < SG_FILTER_SIZE; i++)
	{
		sum += rollCircularBuffer[rollReadIndex] * sg_coef[i];
		rollReadIndex = incrementIndex(rollReadIndex);
	}

	sum = sum / sg_h;

	return sum;
}

float pitch_FIRfilt(float data)
{
	uint8_t i;
	float sum = 0.0;

	pitchCircularBuffer[pitchWriteIndex] = data;

	uint8_t pitchReadIndex = pitchWriteIndex;

	pitchWriteIndex = incrementIndex(pitchWriteIndex);

	for (i = 0; i < SG_FILTER_SIZE; i++)
	{
		sum += pitchCircularBuffer[pitchReadIndex] * sg_coef[i];
		pitchReadIndex = incrementIndex(pitchReadIndex);
	}

	sum = sum / sg_h;

	return sum;
}
#endif


/*******************************************************************************
 * UART4 interrupt handler
 ******************************************************************************/
void UART_UserCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    uart_transfer_t receiveXfer;
    size_t receivedBytes;
    receiveXfer.data     = rxRingBuffer;
    receiveXfer.dataSize = RX_RING_BUFFER_SIZE;

    if (kStatus_UART_RxIdle == status)
    {
        rxOnGoing = false;
        UART_TransferReceiveNonBlocking(UART4, &uartHandle, &receiveXfer, &receivedBytes);
    }
}


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
	uint8_t t[3] = {0};
	uint8_t j[3] = {0};

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
	else if (t[1] = t[2])
		throttle = t[1];

	if (j[0] == j[1])
		joystick = j[0];
	else if (j[0] == j[2])
		joystick = j[2];
	else if (j[1] = j[2])
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
	}
	// Back motor
	Mback = throttle - pitchPID; // - yawPID;
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

	// Initialization of low power timer
	lptmr_config_t lptmrConfig;
	LPTMR_GetDefaultConfig(&lptmrConfig);
	LPTMR_Init(LPTMR0, &lptmrConfig);
	LPTMR_SetTimerPeriod(LPTMR0, USEC_TO_COUNT(LPTMR_USEC_COUNT, LPTMR_SOURCE_CLOCK));

	// UART4 configuration
	uart_config_t config;
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 9600U;
	config.enableTx     = false;
	config.enableRx     = true;
	UART_Init(UART4, &config, UART_CLK_FREQ);
	UART_TransferCreateHandle(UART4, &uartHandle, UART_UserCallback, NULL);
	UART_TransferStartRingBuffer(UART4, &uartHandle, rxRingBuffer, RX_RING_BUFFER_SIZE);
	/*uart_transfer_t receiveXfer;
	size_t receivedBytes;
	receiveXfer.data     = rxRingBuffer;
	receiveXfer.dataSize = RX_RING_BUFFER_SIZE;*/
	//uint8_t i;

	// FXOS8700 initialization and configuration
	//FXOS8700CQ_Init();
	//FXOS8700CQ_Configure_Device();
	//isThereAccelFX = FXOS8700CQ_ReadSensorWhoAmI();

	// MPU6050 angles structure
	struct MPU6050_angles mpu_angles;
	mpu_angles.x = 0;
	mpu_angles.y = 0;
	mpu_angles.dt = 0.01;


	// Pitch structure inicialization
	struct pitchStruct pitchData;
	pitchData.reference = pitch_ref;
	pitchData.angle = pitchAngle;
	pitchData.last_iError = i_error;
	pitchData.last_pError = p_error;
	pitchData.dt = 0.02;

	// Roll structure inicialization
	struct rollStruct rollData;
	rollData.reference = roll_ref;
	rollData.angle = rollAngle;
	rollData.last_iError = i_error;
	rollData.last_pError = p_error;
	rollData.dt = 0.02;

	// Main loop
	while (1)
	{
		SysTick_DelayTicks(20U);
		/******************************************************************
		 * Read commands from bluetooth module
		 ******************************************************************/
		get_J_and_T();
		/******************************************************************
		 * Update motors from commands
		 ******************************************************************/
		commands_to_reference(joystick);
		/******************************************************************
		 * Read angles from MPU6050
		 ******************************************************************/
		if (isThereAccelMPU)
		{
			LPTMRtime = LPTMR_GetCurrentTimerCount(LPTMR0);
			LPTMR_StopTimer(LPTMR0);
			dt_sec = (float)(LPTMRtime) * 0.001;
			mpu_angles.dt = dt_sec;
			MPU6050_ComplementaryFilterAngles(&mpu_angles);
#ifdef SG_filter
			rollData.angle = roll_sgolayfilt(mpu_angles.y);
			pitchData.angle = pitch_sgolayfilt(mpu_angles.x);
#elif FIR_filter
			rollData.angle = roll_FIRfilt(mpu_angles.y);
			pitchData.angle = pitch_FIRfilt(mpu_angles.x);
#else
			rollData.angle = mpu_angles.y;
			pitchData.angle = mpu_angles.x;
#endif
			LPTMR_StartTimer(LPTMR0);
		}
		/******************************************************************
		 * PID controller for pitch angle
		 ******************************************************************/
		pitchData.dt = dt_sec;
		rollData.dt = dt_sec;
		if (throttle >= 5) // If throttle is below 25, quadcopter is on the floor
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
		//PRINTF("front = %3d, back = %3d, left = %3d, right = %3d\r\n", Mfront, Mback, Mleft, Mright);
		PRINTF("throttle = %3d, roll = %3.2f, filteredRoll = %3.2f, rollPID = %3.2f, Mleft = %3d, Mright = %3d\r\n",
				throttle, mpu_angles.y ,rollData.angle, rollPID, Mleft, Mright);
	}
}









