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
#include "FXOS8700CQ.h"
#include "PIDcontroller.h"

/*******************************************************************************
 * Variable definition
 ******************************************************************************/

// UART4 definitions
#define UART UART4
#define UART_CLKSRC UART4_CLK_SRC
#define UART_CLK_FREQ CLOCK_GetFreq(UART4_CLK_SRC)
#define RX_RING_BUFFER_SIZE 10U

// Move constant
#define MOOVE 10

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* UART user callback */
void UART_UserCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData);


/*******************************************************************************
 * Variables declaration
 ******************************************************************************/

// UART4 variables
uart_handle_t uartHandle;
uint8_t rxRingBuffer[RX_RING_BUFFER_SIZE] = {0};
volatile bool rxOnGoing = false;

// Joystick and throttle values
uint8_t joystick, throttle;

// Variables to controlling the BLDC motors
volatile uint8_t Mfront, Mfront_last;	// Front motor
volatile uint8_t Mleft, Mleft_last;		// Left motor
volatile uint8_t Mback, Mback_last;		// Back motor
volatile uint8_t Mright, Mright_last;	// Right motor

// Reference for pitch angle
volatile uint8_t pitch_ref = 0;

// Reference for roll angle
volatile uint8_t roll_ref = 0;


// Pitch PID output
volatile float pitchPID = 0;

// Roll PID output
volatile float rollPID = 0;

// Roll and pitch angles
float pitchAngle_new = 0, pitchAngle_last = 0;
float rollAngle_new = 0, rollAngle_last = 0;

// MPU6050 Who_Am_I flag
bool isThereAccelMPU = false;
// FXOS8700CQ Who_Am_I angles
bool isThereAccelFX = false;

// Pitch and roll angles
float pitch, roll;


// PIT flag
bool pitflag = false;

// UART4 flag
bool uart4flag = false;

/*******************************************************************************
 * UART4 interrupt handler
 ******************************************************************************/
void UART_UserCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    if (kStatus_UART_RxIdle == status)
    {
        rxOnGoing = false;
    }
}


/*******************************************************************************
 * 20 milliseconds interrupt
 ******************************************************************************/
void PIT_0_IRQHANDLER(void)
{
	// Clear all PIT flags
	PIT_ClearStatusFlags(PIT_PERIPHERAL, kPIT_Chnl_0, kPIT_TimerFlag);

	pitflag = true;

	__DSB();
}


/*******************************************************************************
 * Update motors values as function of joystick values
 ******************************************************************************/
void commands_to_motors(uint8_t joistick)
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
void MotorUpdate(uint8_t throttle, uint8_t pitchPID, uint8_t rollPID)
{
	// Front motor
	Mfront = throttle + pitchPID;// - yawPID;
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
	}
	// Left motor
	Mleft = throttle + rollPID; // + yawPID;
	if (Mleft_last != Mleft)
	{
		set_pwm_CnV(FTM0, Mleft, PWM_CH1);
		Mleft_last = Mleft;
	}
	// Right motor
	Mright = throttle - rollPID; // + yawPID;
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

	// UART4 configuration
	uart_config_t config;
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 9600U;
	config.enableTx     = false;
	config.enableRx     = true;
	UART_Init(UART4, &config, UART_CLK_FREQ);
	UART_TransferCreateHandle(UART4, &uartHandle, UART_UserCallback, NULL);
	UART_TransferStartRingBuffer(UART4, &uartHandle, rxRingBuffer, RX_RING_BUFFER_SIZE);
	uart_transfer_t receiveXfer;
	size_t receivedBytes;
	receiveXfer.data     = rxRingBuffer;
	receiveXfer.dataSize = RX_RING_BUFFER_SIZE;
	uint8_t i;

	// FXOS8700 initialization and configuration
	//FXOS8700CQ_Init();
	//FXOS8700CQ_Configure_Device();
	//isThereAccelFX = FXOS8700CQ_ReadSensorWhoAmI();



    //SysTick_DelayTicks(1000U);
    // Start PIT interrupt for each 20ms if MPU sensor is found
    if (isThereAccelMPU)
    {
    	PIT_StartTimer(PIT_PERIPHERAL, PIT_0);
    }

	// Main loop
	while (1)
	{
		if (!rxOnGoing)
		{
			PIT_StopTimer(PIT_PERIPHERAL, PIT_0);
			rxOnGoing = true;
			UART_TransferReceiveNonBlocking(UART4, &uartHandle, &receiveXfer, &receivedBytes);

			for(i=0; i<10; i++)
			{
				if (rxRingBuffer[i] == 0x23)
				{
					if (rxRingBuffer[i+3] == 0x2F)
					{
						joystick = rxRingBuffer[i+2];
						throttle = rxRingBuffer[i+1];
					}
				}
			}
			PRINTF("joystick = 0x%x, throttle = %3d\r\n", joystick, throttle);
			//PIT_StartTimer(PIT_PERIPHERAL, PIT_0);
		}
		commands_to_motors(joystick);
		if ((pitflag == true) && (isThereAccelMPU))
		{
			pitflag = false;
			// Get angles
			pitch = MPU6050_GetYAngle();
			roll = MPU6050_GetXAngle();
			PRINTF("roll = %4.2f, pitch = %4.2f\r\n", pitch, roll);
			//MotorUpdate(throttle, pitchPID, rollPID);
		}
	}
}














