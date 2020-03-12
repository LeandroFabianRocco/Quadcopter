/*
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
 */
 
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

/*******************************************************************************
 * Variable definition
 ******************************************************************************/
#define UART4_CLK_FREQ CLOCK_GetFreq(UART4_CLK_SRC)
#define UART4_IRQn UART4_RX_TX_IRQn
#define UART4_IRQHandler UART4_RX_TX_IRQHandler
#define RING_BUFFER_SIZE 16


/*******************************************************************************
 * Variable declaration
 ******************************************************************************/
uint8_t demoRingBuffer[RING_BUFFER_SIZE];
volatile uint16_t rxIndex;
//uart_handle_t g_uartHandle;


/*******************************************************************************
 * UART4 interrupt handler
 ******************************************************************************/
void UART4_IRQHandler(void)
{
    uint8_t data;
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART4))
    {
        data = UART_ReadByte(UART4);
		demoRingBuffer[rxIndex] = data;
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
 * MAIN function
 ******************************************************************************/
int main(void)
{
	BOARD_InitPins();
	BOARD_BootClockRUN();

	// Local variable declaration
	uart_config_t config;

	// UART4 configuration
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 9600;
	config.enableTx     = false;
	config.enableRx     = true;
	UART_Init(UART4, &config, UART4_CLK_FREQ);
	UART_EnableInterrupts(UART4, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
	EnableIRQ(UART4_IRQn);


	// Main loop
	while (1)
	{

	}
}














