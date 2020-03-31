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
#define UART4_CLK_FREQ CLOCK_GetFreq(UART4_CLK_SRC)
#define UART4_IRQn UART4_RX_TX_IRQn
#define UART4_IRQHandler UART4_RX_TX_IRQHandler
#define RING_BUFFER_SIZE 4

// Move constant
#define MOOVE 10

/*******************************************************************************
 * Variable declaration
 ******************************************************************************/

// UART4 variables
uint8_t RingBuffer[RING_BUFFER_SIZE];
volatile uint16_t rxIndex;

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
void UART4_IRQHandler(void)
{
	uart4flag = true;
    //uint8_t data;
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART4))
    {
    	RingBuffer[rxIndex] = UART_ReadByte(UART4);
		rxIndex++;
		rxIndex %= RING_BUFFER_SIZE;
    }
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
          exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
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
 * Get throttle and jystick values
 ******************************************************************************/
void GetThrottle_and_Joystick(void)
{
	// 0x23, 0xXX, 0xXX, 0x2F
	if (RingBuffer[0] == 0x23)
	{
		throttle = RingBuffer[1];
		joystick = RingBuffer[2];
	}
	else if (RingBuffer[1] == 0x23)
	{
		throttle = RingBuffer[2];
		joystick = RingBuffer[3];
	}
	else if (RingBuffer[2] == 0x23)
	{
		throttle = RingBuffer[3];
		joystick = RingBuffer[0];
	}
	else if (RingBuffer[3] == 0x23)
	{
		throttle = RingBuffer[0];
		joystick = RingBuffer[1];
	}
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
	config.baudRate_Bps = 9600;
	config.enableTx     = false;
	config.enableRx     = true;
	config.rxFifoWatermark = 1;
	UART_Init(UART4, &config, UART4_CLK_FREQ);
	UART_EnableInterrupts(UART4, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
	//UART_EnableInterrupts(UART4, kUART_RxDataRegFullInterruptEnable);
	EnableIRQ(UART4_IRQn);

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
		if (uart4flag == true)
		{
			GetThrottle_and_Joystick();
			uart4flag = false;
		}
		commands_to_motors(joystick);
		//PRINTF("throttle = %3.5d\r\n", throttle);
		PRINTF("0x%x, 0x%x, 0x%x, 0x%x\r\n", RingBuffer[3], RingBuffer[2], RingBuffer[1], RingBuffer[0]);
		/*if ((pitflag == true) && (isThereAccelMPU))
		{
			pitflag = false;
			// Get angles
			pitch = MPU6050_GetYAngle();
			roll = MPU6050_GetXAngle();
			//PRINTF("roll = %4.2f, pitch = %4.2f\r\n", pitch, roll);
			MotorUpdate(throttle, pitchPID, rollPID);
		}*/
	}
}














